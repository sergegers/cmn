#pragma once

#include "groups_info.h"
#include "concepts.h"

namespace cmn::enum_
{

template <util::c::group_info... Groups>
struct enum_info
{
    unsigned int             m_ops;
    groups_info<Groups...>   m_groups;
};

}
