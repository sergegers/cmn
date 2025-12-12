#pragma once

#include <concepts>

#include <boost/mp11.hpp>

#if __has_include(<boost/mp11/type_traits.hpp>)
#   include <boost/mp11/type_traits.hpp>
#else
#   include <cmn/meta/boost/mp11/type_traits.hpp>
#endif

namespace cmn::c
{

namespace detail
{

using namespace boost::mp11;

using char_types_t = mp_list
<
    char
    , unsigned char
    , signed char
    , wchar_t
    , char8_t
    , char16_t
    , char32_t
>;

template <typename T>
consteval auto is_char() -> bool
{
    return mp_find_v<char_types_t, std::remove_cvref_t<T>> < mp_size_v<char_types_t>;
}

}

template <typename T> concept char_ = std::integral<T> && detail::is_char<T>();

}
