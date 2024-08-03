#pragma once

#include <type_traits>

// do not include
//#include "concepts.h"

namespace cmn
{

//-----------------------------------------------------------------------------
//
// https://eel.is/c++draft/conv.prom
//
template <typename T>
struct int_promotion: std::type_identity<T> {};

template <typename T> requires (std::integral<T> && (sizeof (T) < sizeof (int)))
struct int_promotion<T> : std::type_identity<int> {};

template <> struct int_promotion<char8_t> : std::type_identity<int> {};
template <> struct int_promotion<char16_t> : std::type_identity<int> {};
template <> struct int_promotion<char32_t> : std::type_identity<long int> {};
template <> struct int_promotion<wchar_t> : std::type_identity<int> {};

template <> struct int_promotion<bool> : std::type_identity<int> {};

template <typename T> requires (std::is_enum_v<T> && !std::is_scoped_enum_v<T>)
struct int_promotion<T> : std::type_identity<std::underlying_type_t<T>> {};

template <typename T>
using int_promotion_t = typename int_promotion<T>::type;

//-----------------------------------------------------------------------------
//
// https://eel.is/c++draft/conv.fpprom
//
template <typename T>
struct float_promotion : std::type_identity<T> {};

template <>
struct float_promotion<float>: std::type_identity<double> {};

template <typename T>
using float_promotion_t = typename float_promotion<T>::type;

}
