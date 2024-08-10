#pragma once

#include <cmn/meta/concepts.h>

#include "magic_get.h"
#include "qualified_name.h"

namespace cmn::enum_
{

///////////////////////////////////////////////////////////////////////////////
template <c::enum_ Enum>
struct name_info
{
    qualified_name  m_name;
    wqualified_name m_wname;

    consteval name_info():
        m_name{ get_enum_name(Enum{}) },
        m_wname{ get_enum_wname(Enum{})}
    {}
};

}
