#pragma once

#include <cstddef>
#include <concepts>
#include <string>
#include <iosfwd>
#include <exception>
#include <utility>
#include <ios>
#include <format>
#include <utility>

#include <boost/mp11.hpp>
#include <boost/type_traits/promote.hpp>

#if __has_include(<boost/mp11/concepts.hpp>)
#   include <boost/mp11/concepts.hpp>
#else
#   include <cmn/meta/boost/mp11/concepts.hpp>
#endif

#include <cmn/fwd.h>
#include <cmn/meta/concepts/traits.h>
#include <cmn/meta/concepts/type_info.h>
#include <cmn/meta/concepts/format_info.h>
#include <cmn/meta/concepts/slot_manip.h>
#include <cmn/meta/concepts/exception.h>

namespace cmn
{

namespace c
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
    return mp_find<char_types_t, std::remove_cvref_t<T>>::value < mp_size<char_types_t>::value;
}

}

template <typename T> concept char_ = std::integral<T> && detail::is_char<T>();

///////////////////////////////////////////////////////////////////////////////
//
// unit concepts
//
///////////////////////////////////////////////////////////////////////////////
template <typename T>
concept unit = 
    std::derived_from<T, strong_typedef_tag>
 && requires
    {
        typename T::underlying_type;
    }
 && std::constructible_from<typename T::underlying_type>
;

// rich format support
template <typename T>
concept fmt_unit =
    unit<T>
 && io::strong_typedef_fmt_traits<T>::enable_luxury_io
;

//-----------------------------------------------------------------------------
template <typename T>
concept interop_unit =
    unit<T>
 && requires
    {
        typename T::difference_type;
    }
;

///////////////////////////////////////////////////////////////////////////////
template <typename T>
concept enumerable = std::integral<T> || enum_<T> || unit<T>;

///////////////////////////////////////////////////////////////////////////////
//
// Integral constant concepts
//
///////////////////////////////////////////////////////////////////////////////
namespace detail
{

template <enumerable I, typename T>
struct instance_of_enumerable_: std::false_type {};

template <enumerable I, I I_, template <typename, auto> typename IntConstntT>
struct instance_of_enumerable_<I, IntConstntT<I, I_>>: std::true_type {};

}

template <typename T, typename I>
concept instance_of_enumerable =
    enumerable<I>
 && detail::instance_of_enumerable_<I, T>::value
;

template <typename T>
concept instance_of_bool = instance_of_enumerable<T, bool>;

template <typename T>
concept instance_of_unsigned = instance_of_enumerable<T, unsigned>;

}

///////////////////////////////////////////////////////////////////////////////
template <typename T>
struct interop_type : std::type_identity<T> {};

template <typename T> requires std::integral<T>
struct interop_type<T> : boost::promote<T> {};

template <typename T> requires c::c_enum<T>
struct interop_type<T> : boost::promote<T> {};

template <typename T> requires c::scoped_enum<T>
struct interop_type<T> : std::underlying_type<T> {};

template <typename T> requires c::unit<T>
struct interop_type<T> : std::type_identity<typename T::underlying_type> {};

template <typename T> requires c::interop_unit<T>
struct interop_type<T> : std::type_identity<typename T::difference_type> {};

// always integer
template <c::enumerable T> using interop_type_t = interop_type<T>::type;

///////////////////////////////////////////////////////////////////////////////
namespace c
{

namespace detail
{

template <typename Lhs, typename Rhs, typename Res>
concept bit_ops_ = requires(Lhs lhs, Rhs rhs)
{
    { lhs & rhs } /*noexcept*/ -> std::same_as<Res>;
    { lhs | rhs } /*noexcept*/ -> std::same_as<Res>;
    { lhs ^ rhs } /*noexcept*/ -> std::same_as<Res>;
    { ~lhs } noexcept -> std::same_as<Res>;
};

template <typename Lhs, typename Rhs, typename Res>
concept commutative_bit_ops_ = 
    bit_ops_<Lhs, Rhs, Res>
 && bit_ops_<Rhs, Lhs, Res>
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
 && detail::bit_ops_<T, T, T>
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
           detail::commutative_bit_ops_<T, interop_type_t<T>, interop_type_t<T>>
            // for integral types & scoped enums with overloaded operators
        || detail::commutative_bit_ops_<T, interop_type_t<T>, T>
     )  
 )
;

///////////////////////////////////////////////////////////////////////////////
namespace detail
{

template <typename Lhs, typename Rhs, typename Res>
concept commutative_ariphmetic_ops__ = requires(Lhs lhs, Rhs rhs)
{
    { lhs + rhs } /*noexcept*/ -> std::same_as<Res>;
    { lhs * rhs } /*noexcept*/ -> std::same_as<Res>;
};

template <typename Lhs, typename Rhs, typename Res>
concept commutative_ariphmetic_ops_ =
    commutative_ariphmetic_ops__<Lhs, Rhs, Res>
 && commutative_ariphmetic_ops__<Rhs, Lhs, Res>
;

template <typename Lhs, typename Rhs, typename Res>
concept noncommutative_ariphmetic_ops_ = requires(Lhs lhs, Rhs rhs)
{
    { lhs - rhs } /*noexcept*/ -> std::same_as<Res>;
    { lhs / rhs } /*noexcept*/ -> std::same_as<Res>;
    { lhs % rhs } /*noexcept*/ -> std::same_as<Res>;
};

}

template <typename T>
concept ptr_ariphmetic = 
    enumerable<T>
 && std::equality_comparable<T>
    // for integral, unit types or scoped enums with overloaded operators
 && detail::commutative_ariphmetic_ops_<T, interop_type_t<T>, T>
 && detail::noncommutative_ariphmetic_ops_<T, interop_type_t<T>, T>
;

template <typename T>
concept strong_ariphmetic = 
    enumerable<T>
 && std::equality_comparable<T>
 && detail::commutative_ariphmetic_ops__<T, T, T>
 && detail::noncommutative_ariphmetic_ops_<T, T, T>
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
        detail::commutative_ariphmetic_ops_<T, interop_type_t<T>, interop_type_t<T>>
     && detail::noncommutative_ariphmetic_ops_<T, interop_type_t<T>, interop_type_t<T>>
     ||
        // for integral, unit types or scoped enums with overloaded operators
        detail::commutative_ariphmetic_ops_<T, interop_type_t<T>, T>
     && detail::noncommutative_ariphmetic_ops_<T, interop_type_t<T>, T>
 )
;

}

}
