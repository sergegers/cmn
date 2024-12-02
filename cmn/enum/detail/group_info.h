#pragma once

#include <cstddef>
#include <array>
#include <ranges>
#include <utility>
#include <compare>

#include "record_info.h"

namespace cmn::enum_::detail
{

// sorted by value record infos
template <c::enum_ E, std::size_t Sz_>
struct group_info
{
    using enum_type = E;
    using record_type = record_info<E>;
    using mask_type = typename record_type::mask_type;
    using interop_type = typename record_type::interop_type;
    using records_type = std::array<record_type, Sz_>;

    static constexpr std::size_t size = Sz_;

    records_type    m_records;

    template <c::enum_ auto... Ens_>
    consteval group_info(int_<Ens_>... ens):
        m_records{ record_type{ ens }... }
    {
        std::ranges::sort(m_records, {}, &record_type::as_interop);
    }

    constexpr auto operator <=> (group_info const &other) const noexcept -> std::strong_ordering
    {
        return m_records <=> other.m_records;
    }

    constexpr auto operator == (group_info const &other) const noexcept -> bool
    {
        return (*this <=> other) == std::strong_ordering::equal;
    }

    consteval auto calc_mask() const -> mask_type
    {
        return static_cast<mask_type>
        (
            std::ranges::fold_left
            (
                m_records, 
                0, 
                [](interop_type mask, record_type const &rec) -> interop_type
                { return mask | rec.as_interop(); }
            )
        );
    }

    consteval auto min_value() const -> enum_type
    {
        return m_records.front().m_value;
    }

    consteval auto max_value() const -> enum_type
    {
        return m_records.back().m_value;
    }

    constexpr auto get_values() const noexcept -> std::array<enum_type, Sz_>
    {
        using result_type = std::array<enum_type, Sz_>;
        return
            [this]<std::size_t... Idss_>(std::index_sequence<Idss_...>) constexpr -> result_type
            {
                return { m_records[Idss_].m_value... };
            }
            (std::make_index_sequence<Sz_>{})
        ;
    }

    ///////////////////////////////////////////////////////////////////////////////
    //
    // execute operation Op if enum value chunk belongs to group and return unprocessed
    // enum value remainder
    //
    ///////////////////////////////////////////////////////////////////////////////
    constexpr auto exec(enum_type en, auto const &op, mask_type group_mask = no_mask<enum_type>) const -> interop_type
    {
        auto reminder = to_interop(en);
        auto const mval = to_interop(en) & to_interop(group_mask);

        std::ignore = std::ranges::find_if
        (
            m_records,
            [&reminder, mval, &op](record_type const &rec) constexpr -> bool
            {
                static_assert(std::same_as<decltype(rec.as_interop()), interop_type>);
                auto x = rec.as_interop();
                auto y = x == mval;
                bool const found = (rec.as_interop() == mval);
                if (found)
                {
                    op(rec);
                    reminder &= ~mval;
                }
                return found;
            }
        );

        return reminder;
    }
};

//-----------------------------------------------------------------------------
template <c::enum_ E, E... Ens_>
consteval group_info(std::integral_constant<E, Ens_>... ens) -> group_info<E, sizeof... (Ens_)>;


namespace group_
{

template <typename T> using enum_type_t = typename T::enum_type;
template <typename T> using mask_type_t = typename T::mask_type;
template <typename T> using interop_type_t = typename T::interop_type;
template <typename T> using record_type_t = typename T::record_type;

template <typename T> constexpr std::size_t size_v = T::size;


template <c::enum_ auto En_, decltype(En_) ... Ens_> consteval auto make()
    -> group_info<decltype(En_), (sizeof...(Ens_) + 1)>
{
    return { int_<En_>{}, int_<Ens_>{}... };
}

}

}
