#pragma once

#include <concepts>
#include <type_traits>
#include <cstddef>
#include <array>
#include <algorithm>
#include <utility>
#include <compare>

#include <cmn/meta/concepts.h>
#include <cmn/meta/type_traits.h>
#include <cmn/util/feature.h>

#include "record_info.h"

namespace cmn::enum_::detail
{

template <c::enum_ E, std::size_t Sz_>
struct group_info
{
    using enum_type = E;
    using record_type = record_info<E>;
    using mask_type = record_type::mask_type;
    using interop_type = record_type::interop_type;
    using records_type = std::array<record_type, Sz_>;

    static constexpr std::size_t size = Sz_;

    // sorted by value record infos
    records_type    m_records;
    mask_type       m_mask;

private:
    consteval auto check_constraints() const noexcept(false) -> void
    {
        // MSVC bug
        constexpr auto check_limit = 340;

        if (Sz_ < check_limit && std::ranges::adjacent_find(m_records, {}, &record_type::as_interop) != std::ranges::end(m_records))
            throw std::logic_error{ "There are duplicate values in group" };
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

public:

    template <c::enum_ auto... Ens_>
    consteval group_info(int_<Ens_>... ens) noexcept(false):
        m_records{ record_type{ ens }... },
        m_mask{ calc_mask() }
    {
        std::ranges::sort(m_records, {}, &record_type::as_interop);
        check_constraints();
    }

    template <typename... Records>
    consteval group_info(Records &&... records)
        requires (std::same_as<Records, record_type> && ...)
    :
        m_records{ std::forward<Records>(records)... },
        m_mask{ calc_mask() }
    {
        std::ranges::sort(m_records, {}, &record_type::as_interop);
        check_constraints();
    }

    constexpr auto operator <=> (group_info const &other) const noexcept -> std::strong_ordering
    {
        return m_records <=> other.m_records;
    }

    constexpr auto operator == (group_info const &other) const noexcept -> bool
    {
        return (*this <=> other) == std::strong_ordering::equal;
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

    [[nodiscard]] constexpr auto contains(interop_type en) const -> bool
    {
        return std::ranges::binary_search(m_records, en, {}, &record_info<E>::as_interop);
    }

    [[nodiscard]] constexpr auto contains(enum_type en) const -> bool
    {
        return this->contains(interop_cast(en));
    }
};

//-----------------------------------------------------------------------------
template <c::enum_ E, E... Ens_>
consteval group_info(std::integral_constant<E, Ens_>... ens) -> group_info<E, sizeof... (Ens_)>;

template <typename Record, typename... Records>
    requires (std::same_as<Records, Record> && ...)
consteval group_info(Record &&, Records &&...) -> 
    group_info<record_::enum_type_t<std::remove_cvref_t<Record>>, sizeof... (Records) + 1>;


namespace group_
{

template <typename T> using enum_type_t = T::enum_type;
template <typename T> using mask_type_t = T::mask_type;
template <typename T> using interop_type_t = T::interop_type;
template <typename T> using record_type_t = T::record_type;

template <typename T> constexpr std::size_t size_v = T::size;


template <c::enum_ auto En_, decltype(En_) ... Ens_>
[[nodiscard]] consteval auto make() -> group_info<decltype(En_), (sizeof...(Ens_) + 1)>
{
    return { int_<En_>{}, int_<Ens_>{}... };
}

}

}
