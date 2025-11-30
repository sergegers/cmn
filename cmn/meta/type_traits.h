#pragma once

#include <type_traits>
#include <utility>
#include <string>
#include <string_view>

#include <cmn/meta/concepts.h>

namespace cmn
{

template<typename T> struct remove_rvalue_reference { using type = T; };
template<typename T> struct remove_rvalue_reference<T &&> { using type = T; };

template<typename T> using remove_rvalue_reference_t = remove_rvalue_reference<T>::type;

//-----------------------------------------------------------------------------
template<typename T> struct remove_lvalue_reference { using type = T; };
template<typename T> struct remove_lvalue_reference<T &> { using type = T; };

template<typename T> using remove_lvalue_reference_t = remove_lvalue_reference<T>::type;

//-----------------------------------------------------------------------------
//
// Trait template <typename T> copy_const
//
//-----------------------------------------------------------------------------
template <typename Src, typename Dst> struct copy_const: std::type_identity<Dst> {};
template <typename Src, typename Dst> struct copy_const<Src, Dst const>: std::type_identity<Dst> {};
template <typename Src, typename Dst> struct copy_const<Src const, Dst>: std::type_identity<Dst const> {};
template <typename Src, typename Dst> struct copy_const<Src const, Dst const>: std::type_identity<Dst const> {};

template <typename Src, typename Dst> using copy_const_t = copy_const<Src, Dst>::type;
//-----------------------------------------------------------------------------
//
// Trait template <typename T> copy_volatile
//
//-----------------------------------------------------------------------------
template <typename Src, typename Dst> struct copy_volatile: std::type_identity<Dst> {};
template <typename Src, typename Dst> struct copy_volatile<Src, Dst volatile>: std::type_identity<Dst> {};
template <typename Src, typename Dst> struct copy_volatile<Src volatile, Dst>: std::type_identity<Dst volatile> {};
template <typename Src, typename Dst> struct copy_volatile<Src volatile, Dst volatile>: std::type_identity<Dst volatile> {};

template <typename Src, typename Dst> using copy_volatile_t = copy_volatile<Src, Dst>::type;

//-----------------------------------------------------------------------------
//
// Trait template <typename T> copy_lvalue_reference
//
//-----------------------------------------------------------------------------
template <typename Src, typename Dst> struct copy_lvalue_reference: std::type_identity<Dst> {};
template <typename Src, typename Dst> struct copy_lvalue_reference<Src, Dst &>: std::type_identity<Dst> {};
template <typename Src, typename Dst> struct copy_lvalue_reference<Src &, Dst>: std::type_identity<Dst &> {};
template <typename Src, typename Dst> struct copy_lvalue_reference<Src &, Dst &>: std::type_identity<Dst &> {};

template <typename Src, typename Dst> using copy_lvalue_reference_t = copy_lvalue_reference<Src, Dst>::type;

//-----------------------------------------------------------------------------
//
// Trait template <typename T> copy_rvalue_reference
//
//-----------------------------------------------------------------------------
template <typename Src, typename Dst> struct copy_rvalue_reference: std::type_identity<Dst> {};
template <typename Src, typename Dst> struct copy_rvalue_reference<Src, Dst &&>: std::type_identity<Dst> {};
template <typename Src, typename Dst> struct copy_rvalue_reference<Src &&, Dst>: std::type_identity<Dst &&> {};
template <typename Src, typename Dst> struct copy_rvalue_reference<Src &&, Dst &&>: std::type_identity<Dst &&> {};

template <typename Src, typename Dst> using copy_rvalue_reference_t = copy_rvalue_reference<Src, Dst>::type;

//-----------------------------------------------------------------------------
//
// Trait template <typename T> copy_reference
//
// NOTE: because references are collapsed to minimize type instantiation do not use
// copy_lvalue_reference_t, copy_rvalue_reference_t, but reimplement the trait
//
//-----------------------------------------------------------------------------
template <typename Src, typename Dst> struct copy_reference: std::type_identity<Dst> {};
template <typename Src, typename Dst> struct copy_reference<Src, Dst &>: std::type_identity<Dst> {};
template <typename Src, typename Dst> struct copy_reference<Src &, Dst>: std::type_identity<Dst &> {};
template <typename Src, typename Dst> struct copy_reference<Src &, Dst &>: std::type_identity<Dst &> {};
template <typename Src, typename Dst> struct copy_reference<Src, Dst &&>: std::type_identity<Dst> {};
template <typename Src, typename Dst> struct copy_reference<Src &&, Dst>: std::type_identity<Dst &&> {};
template <typename Src, typename Dst> struct copy_reference<Src &&, Dst &&>: std::type_identity<Dst &&> {};

template <typename Src, typename Dst> using copy_reference_t = copy_reference<Src, Dst>::type;

//-----------------------------------------------------------------------------
//
// Trait template <typename T> copy_cvr
//
//-----------------------------------------------------------------------------
template <typename Src, typename Dst>
using copy_cvr = copy_reference
<
    Src,
    copy_volatile_t
    <
        std::remove_reference_t<Src>,
        copy_const_t
        <
            std::remove_reference_t<Src>,
            std::remove_reference_t<Dst>
        >
    >
>;

template <typename Src, typename Dst>
using copy_cvr_t = copy_cvr<Src, Dst>::type;

//-----------------------------------------------------------------------------
//
// Trait template <typename T> deep_add_const
//
//-----------------------------------------------------------------------------
template <typename T> struct deep_add_const : std::type_identity<T const> {};
template <typename T> struct deep_add_const<T &> : std::type_identity<T const &> {};
template <typename T> struct deep_add_const<T *> : std::type_identity<T const * const> {};

template <typename T> using deep_add_const_t = deep_add_const<T>::type;

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
using deep_remove_const_t = deep_remove_const<T>::type;

//-----------------------------------------------------------------------------
//
// Trait template <typename T> deep_copy_const
//
//-----------------------------------------------------------------------------
template <typename Src, typename Dst> struct deep_copy_const : copy_const<Src, Dst> {};
template <typename Src, typename Dst> struct deep_copy_const<Src, Dst &> : std::add_lvalue_reference<copy_const_t<Src, Dst>> {};
template <typename Src, typename Dst> struct deep_copy_const<Src, Dst *> : std::add_pointer<copy_const_t<Src, Dst>> {};

template <typename Src, typename Dst> using deep_copy_const_t = deep_copy_const<Src, Dst>::type;

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
using underlying_type_t = underlying_type<T>::type;

///////////////////////////////////////////////////////////////////////////////
//
// mask type
//

// always unsigned integer
template <c::enumerable T> using mask_type_t = std::make_unsigned_t<interop_type_t<T>>;

///////////////////////////////////////////////////////////////////////////////
template <c::enumerable auto Int_> using int_ = std::integral_constant<decltype(Int_), Int_>;

template <typename Int> constexpr auto value_v = Int::value;

///////////////////////////////////////////////////////////////////////////////
//
// Usable with 'if constexpr' expression for conditional compilation
//
template <bool Val_, typename...>
constexpr bool dependent_v = Val_;

///////////////////////////////////////////////////////////////////////////////
//
// string helpers
//
///////////////////////////////////////////////////////////////////////////////
template <c::string T> using char_t = T::value_type;
template <c::string T> using char_traits_t = T::traits_type;

///////////////////////////////////////////////////////////////////////////////
//
// type_info
//
///////////////////////////////////////////////////////////////////////////////
template <c::adapted_type T>
constexpr auto type_info_v = []
{
    // initialize through lambda to avoid linking errors during
    // constructor execution if the constructor throws exception
    return adapt_type_info(T{});
}();

//-----------------------------------------------------------------------------
template <c::formatted_type T> 
constexpr c::format_info auto default_format_info_v = get_default_format_info(T{});


}