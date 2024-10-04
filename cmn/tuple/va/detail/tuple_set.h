#pragma once

#include <cmn/tuple/va/concepts.h>
#include <cmn/tuple/va/detail/promote.h>

namespace cmn::va::detail
{

//template <typename T, bool EllipsisArg_>
//constexpr auto tuple_set(std::bool_constant<EllipsisArg_>, T const &t)
//    -> keep_type_t<T, EllipsisArg_> = delete;

//-----------------------------------------------------------------------------
//
// non-ellipsis arguments
//
template <typename T>
constexpr auto tuple_set(std::false_type, T const &t) noexcept -> keep_type_t<T, false> { return { t }; }

//-----------------------------------------------------------------------------
//
// ellipsis arguments
//

// large argument transfers by reference
template <typename T> requires !c::va_promoted<T> && c::va_big<T>
constexpr auto tuple_set(std::true_type, T const &t) noexcept -> keep_type_t<T, true>
{ return &t; }

// small argument transfers by value
template <typename T> requires !c::va_promoted<T> && !c::va_big<T>
constexpr auto tuple_set(std::true_type, T const &t) noexcept -> keep_type_t<T, true>
{ return static_cast<keep_type_t<T, true>>(t); }

template <typename T> requires c::int_<T> || c::float_<T>
constexpr auto tuple_set(std::true_type, T const &t) noexcept -> keep_type_t<T, true>
{ return static_cast<keep_type_t<T, true>>(t); }

template <typename T> requires c::unit<T>
constexpr auto tuple_set(std::true_type, T const &t) noexcept -> keep_type_t<T, true>
{ return keep_type_t<T, true> { t.value() }; } // can't use copy list initialization due to explicit ctors

}
