#pragma once

#include <type_traits>
#include <limits>

#include <boost/type_traits/promote.hpp>

#include <cmn/meta/concepts.h>

// TODO: eliminate double mask definitions, see concepts.h

namespace cmn::enum_
{

//-----------------------------------------------------------------------------
template <typename T>
struct mask_type : std::make_unsigned<T> {};

template <typename T> requires std::integral<T>
struct mask_type<T> : boost::promote<T> {};

template <typename T> requires c::enum_<T>
struct mask_type<T> : boost::promote<std::underlying_type_t<T>> {};

template <typename T> requires c::scoped_enum<T>
struct mask_type<T> : std::make_unsigned<std::underlying_type_t<T>> {};

template <c::enumerable T>
using mask_type_t = typename mask_type<T>::type;

template <c::enumerable T>
static constexpr auto no_mask = std::numeric_limits<mask_type_t<T>>::max();

}
