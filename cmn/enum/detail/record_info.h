#pragma once

#include <type_traits>
#include <compare>
#include <string_view>

#include <cmn/meta/concepts.h>

#include "qualified_name.h"

namespace cmn::enum_::detail
{

///////////////////////////////////////////////////////////////////////////////
template <c::enum_ E>
struct record_info
{
    using enum_type = E;
    using mask_type = mask_type_t<enum_type>;
    using interop_type = interop_type_t<enum_type>;

    enum_type                   m_value;
    qualified_member_name       m_name;
    wqualified_member_name      m_wname;

    template <E En_>
    consteval record_info(std::integral_constant<E, En_>) noexcept:
        m_value{ En_ },
        m_name{ int_<En_>{} },
        m_wname{ int_<En_>{} }
    {}

    template <E En_>
    consteval record_info
    (
          std::integral_constant<E, En_>
        , std::string_view enum_member_name
        , std::wstring_view enum_member_wname
    ) noexcept:
        m_value{ En_ },
        m_name{ int_<En_>{}, enum_member_name },
        m_wname{ int_<En_>{}, enum_member_wname }
    {}

    constexpr auto as_mask() const noexcept -> mask_type
    {
        return static_cast<mask_type>(m_value);
    }

    constexpr auto as_interop() const noexcept -> interop_type
    {
        return static_cast<interop_type>(m_value);
    }

    constexpr auto operator <=> (record_info const &other) const noexcept -> std::strong_ordering
    {
        return as_interop() <=> other.as_interop();
    }

    constexpr auto operator == (record_info const &other) const noexcept
    {
        return (*this <=> other) == std::strong_ordering::equivalent;
    }

    //-----------------------------------------------------------------------------
    template <typename Char, typename CharTraits = std::char_traits<Char>>
    constexpr auto &name() const noexcept
    {
        if constexpr (std::is_same_v<Char, char>) return m_name;
        else if constexpr (std::is_same_v<Char, wchar_t>) return m_wname;
        else static_assert(!std::is_same_v<Char, Char>, "Not implemented");
    }

    template <typename Char, typename CharTraits>
    constexpr auto &name(std::basic_ios<Char, CharTraits> const &) const noexcept
    {
        return this->name<Char, CharTraits>();
    }
};

namespace record_
{

template <typename T> using enum_type_t = T::enum_type;
template <typename T> using mask_type_t = T::mask_type;
template <typename T> using interop_type_t = T::interop_type;

template <c::enum_ auto E_> consteval auto make() -> record_info<decltype(E_)> { return { int_<E_>{} }; }

}

}
