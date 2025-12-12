#pragma once

#include <concepts>
#include <type_traits>

#include <cmn/fwd.h>

namespace cmn::c
{

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
struct instance_of_enumerable_ : std::false_type {};

template <enumerable I, I I_, template <typename, auto> typename IntConstntT>
struct instance_of_enumerable_<I, IntConstntT<I, I_>> : std::true_type {};

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
