#pragma once

#include <type_traits>
#include <utility>
#include <limits>

#include <boost/mp11.hpp>

#include <cmn/meta/concepts.h>

namespace cmn
{

template<typename T>
struct remove_rvalue_reference
{
    using type = T;
};

template<typename T>
struct remove_rvalue_reference<T&&>
{
    using type = T;
};

template<typename T>
using remove_rvalue_reference_t = typename remove_rvalue_reference<T>::type;

//-----------------------------------------------------------------------------
//
// Trait template <typename T> copy_const
//
//-----------------------------------------------------------------------------
template <typename Src, typename Dst> struct copy_const: std::type_identity<Dst> {};
template <typename Src, typename Dst> struct copy_const<Src, Dst const>: std::type_identity<Dst> {};
template <typename Src, typename Dst> struct copy_const<Src const, Dst>: std::type_identity<Dst const> {};

template <typename Src, typename Dst> using copy_const_t = typename copy_const<Src, Dst>::type;
//-----------------------------------------------------------------------------
//
// Trait template <typename T> copy_volatile
//
//-----------------------------------------------------------------------------
template <typename Src, typename Dst> struct copy_volatile: std::type_identity<Dst> {};
template <typename Src, typename Dst> struct copy_volatile<Src, Dst volatile>: std::type_identity<Dst> {};
template <typename Src, typename Dst> struct copy_volatile<Src volatile, Dst>: std::type_identity<Dst volatile> {};

template <typename Src, typename Dst> using copy_volatile_t = typename copy_volatile<Src, Dst>::type;

//-----------------------------------------------------------------------------
//
// Trait template <typename T> copy_lvalue_reference
//
//-----------------------------------------------------------------------------
template <typename Src, typename Dst> struct copy_lvalue_reference: std::type_identity<Dst> {};
template <typename Src, typename Dst> struct copy_lvalue_reference<Src, Dst &>: std::type_identity<Dst> {};
template <typename Src, typename Dst> struct copy_lvalue_reference<Src &, Dst>: std::type_identity<Dst &> {};

template <typename Src, typename Dst> using copy_lvalue_reference_t = typename copy_lvalue_reference<Src, Dst>::type;

//-----------------------------------------------------------------------------
//
// Trait template <typename T> copy_rvalue_reference
//
//-----------------------------------------------------------------------------
template <typename Src, typename Dst> struct copy_rvalue_reference: std::type_identity<Dst> {};
template <typename Src, typename Dst> struct copy_rvalue_reference<Src, Dst &&>: std::type_identity<Dst> {};
template <typename Src, typename Dst> struct copy_rvalue_reference<Src &&, Dst>: std::type_identity<Dst &&> {};

template <typename Src, typename Dst> using copy_rvalue_reference_t = typename copy_rvalue_reference<Src, Dst>::type;

//-----------------------------------------------------------------------------
//
// Trait template <typename T> copy_reference
//
// NOTE: because references are collapsed to minimize type instantiation do not use
// copy_lvalue_reference_t, copy_rvalue_reference_t, but reimplement trait
//
//-----------------------------------------------------------------------------
template <typename Src, typename Dst> struct copy_reference: std::type_identity<Dst> {};
template <typename Src, typename Dst> struct copy_reference<Src, Dst &>: std::type_identity<Dst> {};
template <typename Src, typename Dst> struct copy_reference<Src &, Dst>: std::type_identity<Dst &> {};
template <typename Src, typename Dst> struct copy_reference<Src, Dst &&>: std::type_identity<Dst> {};
template <typename Src, typename Dst> struct copy_reference<Src &&, Dst>: std::type_identity<Dst &&> {};

template <typename Src, typename Dst> using copy_reference_t = typename copy_reference<Src, Dst>::type;

//-----------------------------------------------------------------------------
//
// Trait template <typename T> copy_cvr
//
//-----------------------------------------------------------------------------
template <typename Src, typename Dst>
using copy_cvr = copy_reference<Src, copy_volatile_t<Src, copy_const_t<Src, std::remove_reference_t<Dst>>>>;

template <typename Src, typename Dst>
using copy_cvr_t = typename copy_cvr<Src, Dst>::type;

//-----------------------------------------------------------------------------
//
// Trait template <typename T> deep_add_const
//
//-----------------------------------------------------------------------------
template <typename T> struct deep_add_const : std::type_identity<T const> {};
template <typename T> struct deep_add_const<T &> : std::type_identity<T const &> {};
template <typename T> struct deep_add_const<T *> : std::type_identity<T const * const> {};

template <typename T> using deep_add_const_t = typename deep_add_const<T>::type;

//-----------------------------------------------------------------------------
//
// Trait template <typename T> deep_remove_const
//
//-----------------------------------------------------------------------------
template <typename T> struct deep_remove_const : std::type_identity<T> {};
template <typename T> struct deep_remove_const<T const> : std::type_identity<T> {};
template <typename T> struct deep_remove_const<T const &> : std::type_identity<T &> {};
template <typename T> struct deep_remove_const<T const *> : std::type_identity<T *> {};
template <typename T> struct deep_remove_const<T const * const> : std::type_identity<T *> {};

template <typename T>
using deep_remove_const_t = typename deep_remove_const<T>::type;

//-----------------------------------------------------------------------------
//
// Trait template <typename T> deep_copy_const
//
//-----------------------------------------------------------------------------
template <typename Src, typename Dst> struct deep_copy_const : copy_const<Src, Dst> {};
template <typename Src, typename Dst> struct deep_copy_const<Src, Dst &> : std::add_lvalue_reference<copy_const_t<Src, Dst>> {};
template <typename Src, typename Dst> struct deep_copy_const<Src, Dst *> : std::add_pointer<copy_const_t<Src, Dst>> {};

template <typename Src, typename Dst> using deep_copy_const_t = typename deep_copy_const<Src, Dst>::type;

///////////////////////////////////////////////////////////////////////////////
//
// extended underlying_type
//

///////////////////////////////////////////////////////////////////////////////
template <typename T>
struct underlying_type : std::type_identity<T> {};

// enum specialization
template <c::enum_ T>
struct underlying_type<T>: std::underlying_type<T> {};

// unit specialization
template <c::unit T>
struct underlying_type<T>: std::type_identity<typename T::underlying_type> {};

template <c::enumerable T>
using underlying_type_t = typename underlying_type<T>::type;

///////////////////////////////////////////////////////////////////////////////
template <c::enumerable auto Int_> using int_ = std::integral_constant<decltype(Int_), Int_>;

template <typename Int> constexpr auto value_v = Int::value;

///////////////////////////////////////////////////////////////////////////////
namespace detail
{

using namespace boost::mp11;

using char_types_t = mp_list
<
      char
    , unsigned char
    , signed char
    , wchar_t
>;

template <typename T>
consteval auto is_char() -> bool
{
    return mp_find<char_types_t, std::remove_cvref_t<T>>::value < mp_size<char_types_t>::value;
}

}

template <typename T> constexpr bool is_char_v = detail::is_char<T>();

///////////////////////////////////////////////////////////////////////////////
//
// Usable with 'if constexpr' expression for conditional compilation
//
template <bool Val_, typename...>
constexpr bool dependent_v = Val_;

namespace enum_
{

template <c::enumerable T>
static constexpr auto no_mask = std::numeric_limits<interop_type_t<T>>::max();

}

}