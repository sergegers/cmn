#pragma once

#include <type_traits>
#include <compare>

#include <cmn/meta/concepts.h>

#include "qualified_name.h"

namespace cmn::enum_
{

///////////////////////////////////////////////////////////////////////////////
template <c::enum_ E>
struct record_info
{
    using enum_type = E;
    using mask_type = interop_type_t<enum_type>;

    enum_type                   m_value;
    qualified_member_name       m_name;
    wqualified_member_name      m_wname;

    template <E En_>
    consteval record_info(std::integral_constant<E, En_>) noexcept:
        m_value{ En_ },
        m_name{ int_<En_>{} },
        m_wname{ int_<En_>{} }
    {}

    template <typename Char, typename CharTraits>
    constexpr auto &get_name() const noexcept
    {
        if constexpr (std::is_same_v<Char, char>)
            return m_name;
        else if constexpr (std::is_same_v<Char, wchar_t>)
            return m_wname;
        else
            static_assert(!std::is_same_v<Char, Char>, "Not implemented");
    }

    constexpr auto as_mask() const noexcept -> interop_type_t<E>
    {
        return static_cast<mask_type>(m_value);
    }

    constexpr auto operator <=> (record_info const &other) const noexcept -> std::strong_ordering
    {
        return as_mask() <=> other.as_mask();
    }

    constexpr auto operator == (record_info const &other) const noexcept
    {
        return (*this <=> other) == std::strong_ordering::equivalent;
    }
};

namespace record_
{

template <util::c::record_info T> using enum_type_t = typename T::enum_type;
template <util::c::record_info T> using mask_type_t = interop_type_t<enum_type_t<T>>;

}

}
