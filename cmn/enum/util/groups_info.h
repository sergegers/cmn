#pragma once

#include <tuple>
#include <array>

// boost.fusion
// ReSharper disable CppUnusedIncludeDirective
#include <boost/fusion/container/vector.hpp>
#include <boost/fusion/adapted/std_array.hpp>
#include <boost/fusion/adapted/std_tuple.hpp>
#include <boost/fusion/sequence/intrinsic/at_c.hpp>
// ReSharper restore CppUnusedIncludeDirective
#include <boost/fusion/algorithm/iteration/fold.hpp>
#include <boost/fusion/view/zip_view.hpp>

#include <cmn/meta/type_traits.h>

#include "group_info.h"
#include "concepts.h"

//namespace cmn::enum_
//{
//
//template <util::c::group_info... GroupInfos> requires (sizeof... (GroupInfos) > 0)
//using groups_info = std::tuple<GroupInfos...>;
//
//namespace groups_
//{
//
//template <util::c::group_info... Groups>
//consteval util::c::groups_info auto make(Groups &&...groups)
//{
//    return groups_info<Groups...> { std::forward<Groups>(groups)... };
//}
//
//}
//
//}
