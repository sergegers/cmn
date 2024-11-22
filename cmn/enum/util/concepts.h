#pragma once

#include <cstddef>
#include <type_traits>
#include <tuple>
#include <array>

#include <cmn/meta/concepts.h>

namespace cmn::enum_
{

template <c::enum_ En> struct record_info;
// template <typename RecordInfo, std::size_t N_> using group_info = std::array<RecordInfo, N_>;
// template <typename... GroupInfos> using groups_info = std::tuple<GroupInfos...>;
//template <typename... Groups> struct enum_info;


namespace util::c
{

namespace detail
{

template <typename T>                               struct is_record_info_: std::false_type {};
template <cmn::c::enum_ En>                         struct is_record_info_<record_info<En>>: std::true_type {};

}

template <typename T> concept record_info = detail::is_record_info_<T>::value;

//-----------------------------------------------------------------------------
namespace detail
{

template <typename T>                               struct is_group_info_: std::false_type {};
template <record_info RecordInfo, std::size_t N_>   struct is_group_info_<std::array<RecordInfo, N_>>: std::true_type {};

}

template <typename T> concept group_info = detail::is_group_info_<T>::value;

//-----------------------------------------------------------------------------
namespace detail
{

template <typename T>                   struct is_groups_info_: std::false_type {};
template <group_info... GroupInfos>     struct is_groups_info_<std::tuple<GroupInfos...>>: std::true_type {};


}

template <typename T> concept groups_info = detail::is_groups_info_<T>::value;

}

}
