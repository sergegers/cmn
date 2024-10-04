#pragma once

#include <concepts>

#include <cmn/meta/concepts.h>
#include <cmn/tuple/va/concepts.h>
#include <cmn/tuple/va/detail/promote.h>

namespace cmn::va::detail
{

//template <typename T, bool EllipsisArg_>
//constexpr auto tuple_get(std::bool_constant<EllipsisArg_>, keep_type_t<T, EllipsisArg_> const &t)
//    -> T = delete;

//-----------------------------------------------------------------------------
//
// non-ellipsis arguments
//
template <typename T>
constexpr auto tuple_get(std::false_type, keep_type_t<T, false> const &t) -> T { return t; }

//-----------------------------------------------------------------------------
//
// ellipsis arguments
//

// large argument transfers by reference
template <typename T>
    requires !c::va_promoted<T> && c::va_big<T>
constexpr auto tuple_get(std::true_type, keep_type_t<T, true> const &t) -> T
{
    return *static_cast<T *>(t);
}

// small argument transfers by value
template <typename T>
    requires !c::va_promoted<T> && !c::va_big<T>
constexpr auto tuple_get(std::true_type, keep_type_t<T, true> const &t) -> T
{
    return static_cast<T>(t);
}

template <typename T>
    requires c::int_<T> || c::float_<T>
constexpr auto tuple_get(std::true_type, keep_type_t<T, true> const &t) -> T
{
    return static_cast<T>(t);
}

template <typename T> requires c::unit<T>
constexpr auto tuple_get(std::true_type, keep_type_t<T, true> const &t) -> T
{
    return T { static_cast<underlying_type_t<T>>(t) };
}

}
