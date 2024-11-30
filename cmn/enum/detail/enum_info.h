#pragma once

#include <tuple>
#include <array>
#include <type_traits>
#include <algorithm>

#include <boost/fusion/container/vector.hpp>
#include <boost/fusion/algorithm/iteration/fold.hpp>
#include <boost/fusion/view/zip_view.hpp>
// ReSharper disable CppUnusedIncludeDirective
#include <boost/fusion/adapted/std_tuple.hpp>
#include <boost/fusion/adapted/std_array.hpp>
// ReSharper restore CppUnusedIncludeDirective

#include <cmn/meta/concepts.h>
#include <cmn/enum/feature.h>
#include <cmn/enum/kind.h>  // op_t

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
    using mask_type = typename record_type::mask_type;
    using masks_type = std::array<mask_type, sizeof... (Szs_)>;

    op_type                 m_ops;
    groups_type             m_groups;
    masks_type              m_masks;

    template <typename... Groups>
    consteval enum_info(op_type ops, Groups &&... groups):
        m_ops { ops },
        m_groups{ std::forward<Groups>(groups)... },
        m_masks{ calc_masks() }
    {
        if constexpr (c::c_enum<enum_type>)
            // tune operations, drop interoperable flag
            m_ops = reset_feature(ops, op_interoperable);
    }
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

public:
    constexpr auto exec(enum_type en, auto const &op, mask_type addditional_mask = no_mask<enum_type>) noexcept
        -> mask_type // return remainder
    {
        namespace fus = boost::fusion;
        using sequences_type = fus::vector<std::add_lvalue_reference_t<groups_type>, std::add_lvalue_reference_t<masks_type>>;

        return fus::fold
        (
            fus::zip_view<sequences_type>{ sequences_type{ m_groups, m_masks } },
            static_cast<mask_type>(en) & addditional_mask,
            [&op, addditional_mask](mask_type remainder, auto const &group_mask)
            {
                auto const &current_group = fus::at_c<0>(group_mask);
                mask_type const current_mask = fus::at_c<1>(group_mask);

                if (current_mask & addditional_mask)
                    return current_group.exec(current_mask, remainder, op);
                else
                    return remainder;
            }
        );
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

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // enum: 1 group with n records
    // bitfield: n groups with 1 record in each
    // combo: n groups with m records in each
    //
    ////////////////////////////////////////////////////////////////////////////////
    consteval auto kind() const-> kind_t
    {
        using enum kind_t;
        return sizeof... (Szs_) == 1?
            enum_:
            ((Szs_ == 1) && ...)? bitfield: combo;
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


}
