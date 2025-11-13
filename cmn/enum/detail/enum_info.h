#pragma once

#include <tuple>
#include <array>
#include <type_traits>
#include <algorithm>

#include <boost/fusion/algorithm/iteration/fold.hpp>
// ReSharper disable CppUnusedIncludeDirective
#include <boost/fusion/adapted/std_tuple.hpp>
// ReSharper restore CppUnusedIncludeDirective

#include <cmn/fwd.h>  // kind_t, op_t
#include <cmn/meta/concepts.h>
#include <cmn/algorithm/find.h>
#include <cmn/util/util.h>

#include "record_info.h"
#include "group_info.h"

namespace cmn::enum_::detail
{

template <c::enum_ E, std::size_t... Szs_>
struct enum_info
{
    //-----------------------------------------------------------------------------
    //
    // enum_info concept
    //
    static constexpr auto size = (Szs_ + ...);

    using enum_type = E;
    using record_type = record_info<E>;
    using op_type = interop_type_t<op_t>;
    using groups_type = std::tuple<group_info<E, Szs_>...>;
    using mask_type = record_type::mask_type;
    using masks_type = std::array<mask_type, sizeof... (Szs_)>;
    using interop_type = record_type::interop_type;
    using elements_type = std::array<enum_type, size>;

    op_type                 m_ops;
    groups_type             m_groups;
    // keep after m_groups
    masks_type              m_masks;
    elements_type           m_elements; // sorted

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

    [[nodiscard]] consteval auto min_value() const noexcept -> enum_type
    {
        return this->min_enum_value_impl(std::make_index_sequence<sizeof... (Szs_)>{});
    }

    [[nodiscard]] consteval auto max_value() const noexcept -> enum_type
    {
        return this->max_value_impl(std::make_index_sequence<sizeof... (Szs_)>{});
    }

    [[nodiscard]] consteval auto nullable() const noexcept -> bool
    {
        return find_if_fus
        (
            m_groups,
            [](auto const& group) noexcept -> bool { return group.contains(0); }
        )
            != -1
        ;

    }

    [[nodiscard]] consteval auto unique() const noexcept -> bool
    {
        namespace rng = std::ranges;
        return rng::adjacent_find(m_elements) == rng::end(m_elements);
    }

    //
    // end enum_info concept
    //
    //-----------------------------------------------------------------------------

    template <typename... Groups>
    consteval enum_info(op_type ops, Groups &&... groups) noexcept(false):
        m_ops{ ops },
        m_groups{ std::forward<Groups>(groups)... },
        m_masks{ calc_masks() },
        m_elements{ calc_elements() }
    {
    }
private:
    template <std::size_t... Idss_>
    consteval auto calc_masks_impl(std::index_sequence<Idss_...>) const noexcept -> masks_type
    {
        return { std::get<Idss_>(m_groups).m_mask... };
    }

    consteval auto calc_masks() const noexcept -> masks_type
    {
        return calc_masks_impl(std::make_index_sequence<sizeof... (Szs_)>{});
    }

    //-----------------------------------------------------------------------------
    template <std::size_t... Idss_>
    consteval auto min_enum_value_impl(std::index_sequence<Idss_...>) const noexcept -> enum_type
    {
        return static_cast<enum_type>(std::min({ static_cast<mask_type>(std::get<Idss_>(m_groups).min_value())... }));
    }

    template <std::size_t... Idss_>
    consteval auto max_value_impl(std::index_sequence<Idss_...>) const noexcept -> enum_type
    {
        return static_cast<enum_type>(std::max({ static_cast<mask_type>(std::get<Idss_>(m_groups).max_value())... }));
    }

    //-----------------------------------------------------------------------------
    consteval auto calc_elements() const noexcept -> elements_type
    {
        elements_type elems;

        std::ignore = boost::fusion::fold
        (
            m_groups,
            0ul,
            [&elems](std::size_t idx, auto const &group)
            {
                return std::ranges::fold_left
                (
                    group.m_records,
                    idx,
                    [&elems](std::size_t idx, auto const& record)
                    {
                        return elems[idx++] = record.m_value, idx;
                    }
                );
            }
        );

        std::ranges::sort
        (
            elems, 
            [](auto const lhs, auto const rhs) { return interop_cast(lhs) < interop_cast(rhs); }
        );
        return elems;
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
