#pragma once

#include <concepts>
#include <type_traits>
#include <tuple>
#include <cstdint>

#include <boost/mp11.hpp>
#include <boost/type_traits/promote.hpp>

#include <cmn/meta/concepts.h>
#include <cmn/meta/type_traits.h>
#include <cmn/util/strong_typedef.h>

// specialize unit promotion
namespace boost
{

template <cmn::c::unit T>
struct promote<T>: promote<cmn::underlying_type_t<T>> {};

}

namespace cmn
{

////////////////////////////////////////////////////////////////////////////////
// For va_tuple, va_tuple_view
////////////////////////////////////////////////////////////////////////////////
// NOTE: tuple template parameters must include ellipsis where ellipsis
// is located. Zero based parameter index skips ellipsis slot.
////////////////////////////////////////////////////////////////////////////////
struct ellipsis {};

namespace va
{

struct tuple_tag;

template <typename T>
struct unwrap: std::type_identity<T> {};

template <c::unit T>
struct unwrap<T>: underlying_type<T> {};

template <c::enum_ T>
struct unwrap<T>: std::underlying_type<T> {};

template <typename T>
using unwrap_t = typename unwrap<T>::type;

}

namespace c
{

////////////////////////////////////////////////////////////////////////////////
// allowed va_list argument types
////////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
// for promotion

// TODO: use boost type traits for integral & float promotions

// use in int_promotion
template <typename T>
concept int_ = std::integral<T> || std::same_as<T, bool> || enum_<T>;

// use in float promotion
template <typename T>
concept float_ = std::same_as<T, float> || std::same_as<T, double>;

// may be there will be a more complex check
template <typename T>
concept va_arg_ = 
    int_<T>
 || float_<T>
 || unit<T>   
 || (std::is_trivial_v<T> && std::is_standard_layout_v<T>)  // std::is_pod<> is deprecated
 || std::is_pointer_v<T>
;

template <typename T>
concept promoted_va_arg =
        va_arg_<T>
    && !std::same_as<boost::promote_t<T>, T>
;

//-----------------------------------------------------------------------------
//
// big objects or objects with not power 2 size are passed by reference
//
//#define __crt_va_arg(ap, t)                                               \
//    ((sizeof(t) > sizeof(__int64) || (sizeof(t) & (sizeof(t) - 1)) != 0) \
//        ? **(t**)((ap += sizeof(__int64)) - sizeof(__int64))             \
//        :  *(t* )((ap += sizeof(__int64)) - sizeof(__int64)))
//
//-----------------------------------------------------------------------------
template <typename T>
concept ref_va_arg =
    !unit<T>
 &&
 (
       sizeof(T) > sizeof(std::intptr_t)
    || ((sizeof(T) & (sizeof(T) - 1)) != 0)
 )
;

template <typename T>
concept eva_arg =
    va_arg_<T>
 || std::same_as<T, ellipsis>
;

}

}
