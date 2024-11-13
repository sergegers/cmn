#pragma once

#include <type_traits>
#include <tuple>

#include <cmn/meta/concepts.h>

namespace cmn::enum_
{

template <c::enum_ auto En_> struct record_info;
// template <typename... RecordInfos> using group_info = std::tuple<RecordInfos...>;
// template <typename... GroupInfos> using groups_info = std::tuple<GroupInfos...>;
// template <typename... Groups> struct enum_info;


namespace util::c
{

namespace detail
{

template <typename T>                   struct is_record_info_: std::false_type {};
template <cmn::c::enum_ auto En_>       struct is_record_info_<record_info<En_>>: std::true_type {};

}

template <typename T> concept record_info = detail::is_record_info_<T>::value;

//-----------------------------------------------------------------------------
namespace detail
{

template <typename T>                   struct is_group_info_: std::false_type {};
template <record_info... RecordInfos>   struct is_group_info_<std::tuple<RecordInfos...>>: std::true_type {};

}

template <typename T> concept group_info = detail::is_group_info_<T>::value;

//-----------------------------------------------------------------------------
namespace detail
{

template <typename T>                   struct is_groups_info_: std::false_type {};
template <group_info... GroupInfos>   struct is_groups_info_<std::tuple<GroupInfos...>>: std::true_type {};


}

template <typename T> concept groups_info = detail::is_groups_info_<T>::value;

}

}
