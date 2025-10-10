#pragma once

#include <tuple>
#include <array>
#include <type_traits>
#include <algorithm>
#include <ranges>

#include <boost/fusion/container/vector.hpp>
#include <boost/fusion/algorithm/iteration/fold.hpp>
#include <boost/fusion/view/zip_view.hpp>
// ReSharper disable CppUnusedIncludeDirective
#include <boost/fusion/adapted/std_tuple.hpp>
#include <boost/fusion/adapted/std_array.hpp>
// ReSharper restore CppUnusedIncludeDirective
#if __has_include(<boost/fusion/concepts.hpp>)
#   include <boost/fusion/concepts.hpp>
#else
#   include <cmn/meta/boost/fusion/concepts.hpp>
#endif
#if __has_include(<boost/fusion/type_traits.hpp>)
#   include <boost/fusion/type_traits.hpp>
#else
#   include <cmn/meta/boost/fusion/type_traits.hpp>
#endif

#include <cmn/fwd.h>  // kind_t, op_t
#include <cmn/meta/concepts.h>
#include <cmn/util/feature.h>

#include "group_info.h"

namespace cmn::enum_::detail
{

template <c::enum_ E, std::size_t... Szs_>
struct enum_info
{
    using enum_type = E;
    using record_type = record_info<E>;
    using op_type = interop_type_t<op_t>;
    using groups_type = std::tuple<group_info<E, Szs_>...>;
    using mask_type = record_type::mask_type;
    using masks_type = std::array<mask_type, sizeof... (Szs_)>;
    using interop_type = record_type::interop_type;

    template <typename... Groups>
    consteval enum_info(op_type ops, Groups &&... groups):
        m_ops { ops },
        m_groups{ std::forward<Groups>(groups)... },
        m_masks{ calc_masks() }
    {}

    //-----------------------------------------------------------------------------
    //
    // enum_info concept

    op_type                 m_ops;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // enum: 1 group with n records
    // bitfield: n groups with 1 record in each
    // combo: n groups with m records in each
    //
    ////////////////////////////////////////////////////////////////////////////////
    static consteval auto kind() -> kind_t
    {
        using enum kind_t;
        return sizeof... (Szs_) == 1?
            enum_:
            ((Szs_ == 1) && ...)? bitfield: combo;
    }

    groups_type             m_groups;
    masks_type              m_masks;

    //
    //-----------------------------------------------------------------------------
private:
    ///////////////////////////////////////////////////////////////////////////////
    //
    // get smallest enum value
    //

    template <std::size_t... Idss_>
    consteval auto min_enum_value_impl(std::index_sequence<Idss_...>) const -> enum_type
    {
        return static_cast<enum_type>(std::min({ static_cast<mask_type>(std::get<Idss_>(m_groups).min_value())... }));
    }

public:
    consteval auto min_value() const -> enum_type
    {
        return this->min_enum_value_impl(std::make_index_sequence<sizeof... (Szs_)>{});
    }
    ///////////////////////////////////////////////////////////////////////////////
    //
    // get largest enum value
    //
private:
    template <std::size_t... Idss_>
    consteval auto max_value_impl(std::index_sequence<Idss_...>) const -> enum_type
    {
        return static_cast<enum_type>(std::max({ static_cast<mask_type>(std::get<Idss_>(m_groups).max_value())... }));
    }
public:
    consteval auto max_value() const -> enum_type
    {
        return this->max_value_impl(std::make_index_sequence<sizeof... (Szs_)>{});
    }

    //
    //-----------------------------------------------------------------------------
private:
    template <std::size_t... Idss_>
    consteval auto calc_masks_impl(std::index_sequence<Idss_...>) const -> masks_type
    {
        return { std::get<Idss_>(m_groups).calc_mask()... };
    }

    consteval auto calc_masks() const -> masks_type
    {
        return calc_masks_impl(std::make_index_sequence<sizeof... (Szs_)>{});
    }
};

//-----------------------------------------------------------------------------
template <typename Group, typename... Groups>
consteval enum_info(interop_type_t<op_t> ops, Group &&, Groups &&... groups) ->
    enum_info
    <
          group_::enum_type_t<std::remove_reference_t<Group>>
        , group_::size_v<std::remove_reference_t<Group>>, group_::size_v<std::remove_reference_t<Groups>>...
    >;

///////////////////////////////////////////////////////////////////////////////
template
<
      boost::c::fus_sequence Groups
    , c::adapted_enum E
>
constexpr auto exec
(
      Groups const &groups
    , std::array<mask_type_t<E>, boost::fusion::result_of::size_v<Groups>> const &masks
    , E en
    , std::invocable<record_info<E> const &> auto const &op
    , mask_type_t<E> addditional_mask = no_mask<E>
) noexcept
    -> E // return remainder
{
    namespace fus = boost::fusion;
    namespace rfus = fus::result_of;

    using interop_type = interop_type_t<E>;
    using masks_type = std::array<mask_type_t<E>, rfus::size_v<Groups>>;
    using sequences_type = fus::vector<Groups const &, masks_type const &>;

    return fus::fold
    (
        fus::zip_view<sequences_type>{ sequences_type{ groups, masks } },
        value(en, addditional_mask),
        [&op, addditional_mask](E remainder, auto const &group_mask_pair)
        {
            auto const &current_group = fus::at_c<0>(group_mask_pair);
            auto const &current_mask = fus::at_c<1>(group_mask_pair);

            if (!empty(current_mask & addditional_mask))
                return group_::exec(current_group.m_records, remainder, op, current_mask);
            else
                return remainder;
        }
    );
}

template
<
    boost::c::fus_sequence Groups
    , c::adapted_enum E
>
constexpr auto exec2
(
    Groups const& groups
    , std::array<mask_type_t<E>, boost::fusion::result_of::size_v<Groups>> const& masks
    , E en
    , std::invocable<record_info<E> const&, mask_type_t<E>> auto const &op
    , mask_type_t<E> addditional_mask = no_mask<E>
) noexcept
-> E // return remainder
{
    namespace fus = boost::fusion;
    namespace rfus = fus::result_of;

    using masks_type = std::array<mask_type_t<E>, rfus::size_v<Groups>>;
    using sequences_type = fus::vector<Groups const&, masks_type const &>;

    return fus::fold
    (
        fus::zip_view<sequences_type>{ sequences_type{ groups, masks } },
        value(en, addditional_mask),
        [&op, addditional_mask](E remainder, auto const& group_mask_pair) constexpr
        {
            auto const& current_group = fus::at_c<0>(group_mask_pair);
            auto const& current_mask = fus::at_c<1>(group_mask_pair);

            if (!empty(current_mask & addditional_mask))
                return group_::exec2(current_group.m_records, remainder, op, current_mask);
            else
                return remainder;
        }
    );
}

}
