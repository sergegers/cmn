#pragma once

#include <type_traits>

#include <cmn/meta/concepts.h>

#include "qualified_name.h"

namespace cmn::enum_
{

///////////////////////////////////////////////////////////////////////////////
template <c::enum_ auto En_>
struct record_info
{
    using enum_type = decltype(En_);
    using mask_type = interop_type_t<enum_type>;

    static constexpr auto enum_value = En_;

    qualified_member_name       m_name;
    wqualified_member_name      m_wname;

    consteval record_info():
        m_name{ int_<En_>{} },
        m_wname{ int_<En_>{} }
    {}

    template <typename Char, typename CharTraits>
    auto &get_name() const
    {
        if constexpr (std::is_same_v<Char, char>)
            return m_name;
        else if constexpr (std::is_same_v<Char, wchar_t>)
            return m_wname;
        else
            static_assert(!std::is_same_v<Char, Char>, "Not implemented");
    }

    static constexpr enum_type value = En_;
    static constexpr mask_type value_as_mask = static_cast<mask_type>(En_);
};

namespace record_
{

template <typename /*util::c::record_info*/ T> using enum_type_t = typename T::enum_type;
template <typename /*util::c::record_info*/ T> using mask_type_t = interop_type_t<enum_type_t<T>>;

template <c::enum_ auto En_>
constexpr auto get_value(record_info<En_> const &rec) -> decltype(En_)
{
    return rec.value;
}

template <c::enum_ auto En_>
constexpr auto get_value_as_mask(record_info<En_> const &rec) -> interop_type_t<decltype(En_)>
{
    return rec.value_as_mask;
}

}

}
