#pragma once

#include <concepts>
#include <type_traits>
#include <tuple>
#include <cstdint>

#include <boost/mp11.hpp>

#include <cmn/meta/concepts.h>
#include <cmn/meta/type_traits.h>
#include <cmn/util/strong_typedef.h>

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

template <typename T>
concept promoted = int_<T> || float_<T>;

template <typename T>
concept va_promoted = promoted<T> || unit<T>;

template <typename T>
concept va_big = sizeof(T) > sizeof(std::intptr_t);

// may be there will be a more complex check
template <typename T>
concept va_argument = 
    c::int_<T>
 || c::float_<T>
 || c::unit<T>   
 || std::is_pod_v<T> 
 || std::is_pointer_v<T>
;

template <typename T>
concept eva_argument = 
        va_argument<T>
     || std::same_as<T, ellipsis>
;

}

namespace detail
{

template <typename EVaSig>
struct is_eva_signature: std::false_type {};

template <typename Res, c::eva_argument... EArgs>
struct is_eva_signature<auto (EArgs...) -> Res>:
    std::bool_constant
    <
        (boost::mp11::mp_find<std::tuple<EArgs...>, ellipsis>::value <
            boost::mp11::mp_size<std::tuple<EArgs...>>::value)
    >
{};

}

namespace c
{

// function type with va arguments and mandatory ellipsis
template <typename EVaSig>
concept eva_signature = detail::is_eva_signature<EVaSig>::value;

}

}
