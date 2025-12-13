#pragma once

#include <concepts>

#include <boost/type_traits/promote.hpp>

#include "int.h"

namespace cmn
{

///////////////////////////////////////////////////////////////////////////////
template <typename T>
struct interop_type: std::type_identity<T> {};

template <typename T> requires std::integral<T>
struct interop_type<T>: boost::promote<T> {};

template <typename T> requires c::c_enum<T>
struct interop_type<T>: boost::promote<T> {};

template <typename T> requires c::scoped_enum<T>
struct interop_type<T>: std::underlying_type<T> {};

template <typename T> requires c::unit<T>
struct interop_type<T>: std::type_identity<typename T::underlying_type> {};

template <typename T> requires c::interop_unit<T>
struct interop_type<T>: std::type_identity<typename T::difference_type> {};

// always integer
template <c::enumerable T> using interop_type_t = interop_type<T>::type;

///////////////////////////////////////////////////////////////////////////////
namespace c
{

namespace detail
{

template <typename Lhs, typename Rhs, typename Res>
concept bit_ops_with_ = requires(Lhs lhs, Rhs rhs)
{
    { lhs & rhs } /*noexcept*/ -> std::same_as<Res>;
    { lhs | rhs } /*noexcept*/ -> std::same_as<Res>;
    { lhs ^ rhs } /*noexcept*/ -> std::same_as<Res>;
    { ~lhs } noexcept -> std::same_as<Res>;
};

template <typename Lhs, typename Rhs, typename Res>
concept commutative_bit_ops_with_ =
    bit_ops_with_<Lhs, Rhs, Res>
 && bit_ops_with_<Rhs, Lhs, Res>
;

}

//-----------------------------------------------------------------------------
//
// for scoped bitfields
//
template <typename T>
concept strong_bitfield =
    enumerable<T>
 && std::equality_comparable<T>
 && detail::bit_ops_with_<T, T, T>
;

//-----------------------------------------------------------------------------
//
// for any bitfields
//
template <typename T>
concept bitfield =
    strong_bitfield<T>
 ||
 (
     enumerable<T>
     && std::equality_comparable<T>
     && std::equality_comparable_with<T, interop_type_t<T>>
     &&
     (
         // for integral types & C enums with implicit conversion to int types
         detail::commutative_bit_ops_with_<T, interop_type_t<T>, interop_type_t<T>>
         // for integral types & scoped enums with overloaded operators
         || detail::commutative_bit_ops_with_<T, interop_type_t<T>, T>
         )
     )
;

///////////////////////////////////////////////////////////////////////////////
namespace detail
{

template <typename Lhs, typename Rhs, typename Res>
concept commutative_ariphmetic_ops_with__ = requires(Lhs lhs, Rhs rhs)
{
    { lhs + rhs } /*noexcept*/ -> std::same_as<Res>;
    { lhs* rhs } /*noexcept*/ -> std::same_as<Res>;
};

template <typename Lhs, typename Rhs, typename Res>
concept commutative_ariphmetic_ops_with_ =
    commutative_ariphmetic_ops_with__<Lhs, Rhs, Res>
    && commutative_ariphmetic_ops_with__<Rhs, Lhs, Res>
    ;

template <typename Lhs, typename Rhs, typename Res>
concept noncommutative_ariphmetic_ops_with_ = requires(Lhs lhs, Rhs rhs)
{
    { lhs - rhs } /*noexcept*/ -> std::same_as<Res>;
    { lhs / rhs } /*noexcept*/ -> std::same_as<Res>;
    { lhs% rhs } /*noexcept*/ -> std::same_as<Res>;
};

}

template <typename T>
concept ptr_ariphmetic =
    enumerable<T>
    && std::equality_comparable<T>
    // for integral, unit types or scoped enums with overloaded operators
    && detail::commutative_ariphmetic_ops_with_<T, interop_type_t<T>, T>
    && detail::noncommutative_ariphmetic_ops_with_<T, interop_type_t<T>, T>
    ;

template <typename T>
concept strong_ariphmetic =
    enumerable<T>
 && std::equality_comparable<T>
 && detail::commutative_ariphmetic_ops_with__<T, T, T>
 && detail::noncommutative_ariphmetic_ops_with_<T, T, T>
;

template <typename T>
concept ariphmetic =
    strong_ariphmetic<T>
 ||
    enumerable<T>
 && std::equality_comparable<T>
 && std::equality_comparable_with<T, interop_type_t<T>>
 (
     // for integral types & C enums with implicit conversion to int types
     detail::commutative_ariphmetic_ops_with_<T, interop_type_t<T>, interop_type_t<T>>
     && detail::noncommutative_ariphmetic_ops_with_<T, interop_type_t<T>, interop_type_t<T>>
     ||
     // for integral, unit types or scoped enums with overloaded operators
     detail::commutative_ariphmetic_ops_with_<T, interop_type_t<T>, T>
     && detail::noncommutative_ariphmetic_ops_with_<T, interop_type_t<T>, T>
 )
;

}

}
