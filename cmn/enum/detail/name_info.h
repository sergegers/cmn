#pragma once

#include <cmn/meta/concepts.h>

#include "qualified_name.h"

namespace cmn::enum_::detail
{

///////////////////////////////////////////////////////////////////////////////
template <c::enum_ Enum>
struct name_info
{
    qualified_name  m_name;
    wqualified_name m_wname;

    consteval name_info():
        m_name{ Enum{} },
        m_wname{ Enum{}}
    {}
};

}
