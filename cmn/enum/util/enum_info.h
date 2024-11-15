#pragma once

#include <concepts>

#include <cmn/meta/concepts.h>
#include <cmn/enum/feature.h>
#include "group_info.h"
#include "groups_info.h"
#include "concepts.h"

namespace cmn::enum_
{

template <util::c::group_info Group, util::c::group_info... Groups>
    requires (std::same_as<group_::record_enum_type_t<Group>, group_::record_enum_type_t<Groups>> && ...)
struct enum_info
{
    using enum_type = group_::record_enum_type_t<Group>;
    using op_type = interop_type_t<op_t>;
    using groups_info_type = groups_info<Group, Groups...>;

    op_type                 m_ops;
    groups_info_type        m_groups;

    consteval enum_info(op_type ops, groups_info_type &&groups)
        requires c::scoped_enum<enum_type>
    :
        m_ops{ ops }, m_groups{ std::move(groups) }
    {}

    consteval enum_info(op_type ops, groups_info_type &&groups)
        requires c::c_enum<enum_type>
    :
        // tune operations, drop interoperable flag
        m_ops{ reset_feature(ops, op_interoperable) },
        m_groups{ std::move(groups) }
    {
    }
};

}
