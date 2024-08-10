#pragma once

namespace cmn::enum_
{

template <typename... Groups>
struct enum_info
{
    unsigned int             m_ops;
    groups_info<Groups...>   m_groups;
};

}
