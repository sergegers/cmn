#pragma once

#include <tuple>
#include <array>
#include <type_traits>
#include <algorithm>

#include <boost/fusion/algorithm/iteration/iter_fold.hpp>
#include <boost/fusion/iterator/deref.hpp>
#include <boost/fusion/sequence/intrinsic/begin.hpp>
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

    template <typename... Groups>
    consteval enum_info(op_type ops, Groups &&... groups) noexcept(false):
        m_ops{ ops },
        m_groups{ std::forward<Groups>(groups)... },
        m_masks{ calc_masks() }
    {
    }
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
constexpr auto fold
(
      Groups const &groups
    , std::array<mask_type_t<E>, boost::fusion::result_of::size_v<Groups>> const &masks
    , E en
    , std::invocable<record_info<E> const&, mask_type_t<E>> auto const &op
    , mask_type_t<E> addditional_mask = no_mask<E>
) noexcept
-> E // return remainder
{
    // Can't use fus::zip_view() here due to fusion bugs

    namespace fus = boost::fusion;

    return fus::iter_fold
    (
        groups,
        get_feature(en, addditional_mask),
        [&op, addditional_mask, &masks, begin = fus::begin(groups)]
        (E remainder, auto const &it) constexpr
        {
            auto const idx = fus::distance(begin, it);
            auto const &group = fus::deref(it);
            auto const &mask = masks[idx];

            if (!empty(mask & addditional_mask))
                return group_::find(group, remainder, op, mask);
            else
                return remainder;
        }
    );
}

}
