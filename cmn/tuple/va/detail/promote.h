#pragma once

#include <type_traits>

#include <cmn/meta/promote.h>
#include <cmn/meta/concepts.h>
#include <cmn/tuple/va/concepts.h>

namespace cmn::va
{

namespace detail
{

//-----------------------------------------------------------------------------
template <typename T, bool EllipsisArg_>
struct keep_type_;

// keep non ellipsis types as is
template <typename T>
struct keep_type_<T, false>: std::type_identity<T> {};

// large argument transfers by reference
template <typename T> requires !c::va_promoted<T> && c::va_big<T>
struct keep_type_<T, true>: std::type_identity<T *> {};

// small argument transfers by value
template <typename T> requires !c::va_promoted<T> && !c::va_big<T>
struct keep_type_<T, true>: std::type_identity<T> {};

template <c::float_ T>
struct keep_type_<T, true>: float_promotion<T> {};

template <c::int_ T>
struct keep_type_<T, true>: int_promotion<T> {};

template <c::unit T>
struct keep_type_<T, true>: underlying_type<T> {};

//-----------------------------------------------------------------------------
template <typename T, bool EllipsisArg_>
using keep_type = keep_type_<std::remove_cv_t<T>, EllipsisArg_>;

template <typename T, bool EllipsisArg_>
using keep_type_t = typename keep_type<T, EllipsisArg_>::type;

}

using detail::keep_type;
using detail::keep_type_t;

}
