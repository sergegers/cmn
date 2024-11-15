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

namespace cmn::enum_
{

template <util::c::group_info... GroupInfos> using groups_info = std::tuple<GroupInfos...>;

namespace groups_
{

template <typename /*util::c::group_info*/... Groups>
consteval /*util::c::groups_info*/ auto make(Groups &&...groups)
{
    return groups_info{ std::forward<Groups>(groups)... };
}

template <typename Op, util::c::group_info Group, util::c::group_info... Groups>
auto fold
(
    groups_info<Group, Groups...> const &groups, 
    std::array<group_::mask_type_t<Group>, sizeof... (Groups) + 1> const &group_masks,
    group_::mask_type_t<Group> en, 
    Op const &op,
    group_::mask_type_t<Group> addditional_mask = no_mask<group_::record_enum_type_t<Group>>
    
) -> group_::mask_type_t<Group> // return remainder
{
    using mask_type = group_::mask_type_t<Group>;
    using sequences_type = boost::fusion::vector<decltype(groups), decltype(group_masks)>;

    return boost::fusion::fold
    (
        boost::fusion::zip_view<sequences_type>{ sequences_type{ groups, group_masks } },
        en & addditional_mask,
        [&op, addditional_mask](mask_type remainder, auto const &group_mask)
        {
            using boost::fusion::at_c;
            auto const &current_group = at_c<0>(group_mask);
            mask_type const current_mask = at_c<1>(group_mask);

            if (current_mask & addditional_mask)
                return group_::find(current_group, current_mask, remainder, op);
            else
                return remainder;
        }
    );
}

}

}
