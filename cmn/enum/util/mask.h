#pragma once

#include <type_traits>
#include <limits>

#include <cmn/meta/concepts.h>
#include <cmn/meta/promote.h>

namespace cmn::enum_
{

//-----------------------------------------------------------------------------
template <typename T>
struct mask_type : std::make_unsigned<T> {};

template <typename T> requires std::integral<T>
struct mask_type<T> : int_promotion<T> {};

template <typename T> requires c::enum_<T>
struct mask_type<T> : int_promotion<std::underlying_type_t<T>> {};

template <typename T> requires c::scoped_enum<T>
struct mask_type<T> : std::make_unsigned<std::underlying_type_t<T>> {};

template <c::enumerable T>
using mask_type_t = typename mask_type<T>::type;

template <c::enumerable T>
static constexpr auto no_mask = std::numeric_limits<mask_type_t<T>>::max();

}
