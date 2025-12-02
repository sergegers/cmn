#pragma once

#include <boost/exception/all.hpp>

#include <boost/mp11/map.hpp>
#include <boost/mpl/pair.hpp>

#include "traits.h"

namespace cmn::c
{

///////////////////////////////////////////////////////////////////////////////
//
// exception concepts
//
template <typename E>
concept exception = std::derived_from<E, std::exception> && std::derived_from<E, boost::exception>;

template <typename E>
concept std_only_exception = std::derived_from<E, std::exception> && !std::derived_from<E, boost::exception>;

template <typename E>
concept boost_only_exception = !std::derived_from<E, std::exception> && std::derived_from<E, boost::exception>;

//-----------------------------------------------------------------------------
template <typename T>
concept error_info = instance_of<T, boost::error_info>;

template <typename T>
concept error_info_pair =
    instance_of<T, boost::mpl::pair> && error_info<typename T::first> && error_info<typename T::second>;

namespace detail
{

using namespace boost::mp11;

template <typename T>
struct is_error_info_map_ : std::false_type
{
};

template <error_info_pair... ErrorInfoPairs>
struct is_error_info_map_<mp_list<ErrorInfoPairs...>> : std::true_type
{
};

//-----------------------------------------------------------------------------
template <typename T>
struct is_error_info_list_ : std::false_type
{
};

template <error_info... ErrorInfos>
struct is_error_info_list_<mp_list<ErrorInfos...>> : std::true_type
{
};

//-----------------------------------------------------------------------------
template <boost::c::mp11_list L>
constexpr bool mp_is_unique_v = std::is_same_v<L, boost::mp11::mp_unique<L>>;

} // namespace detail

///////////////////////////////////////////////////////////////////////////////
template <typename T>
concept error_info_map = detail::is_error_info_map_<T>::value && detail::mp_is_unique_v<boost::mp11::mp_map_keys<T>>;

template <typename T>
concept error_info_list = detail::is_error_info_list_<T>::value && detail::mp_is_unique_v<T>;

//
///////////////////////////////////////////////////////////////////////////////

}
