#pragma once

#include <cmn/enum/detail/qualified_name.h>
#include <cmn/enum/detail/name_info.h>
#include <cmn/enum/detail/enum_info.h>
#include <cmn/enum/detail/group_info.h>
#include <cmn/enum/detail/record_info.h>

namespace cmn::enum_
{

using detail::basic_qualified_name;
using detail::qualified_name;
using detail::wqualified_name;

using detail::basic_magic_enum_name_v;
using detail::magic_enum_name_v;
using detail::magic_enum_wname_v;
using detail::basic_magic_enum_member_name_v;
using detail::magic_enum_member_name_v;
using detail::magic_enum_member_wname_v;

using detail::basic_qualified_member_name;
using detail::qualified_member_name;
using detail::wqualified_member_name;

using detail::name_info;
using detail::enum_info;
using detail::record_info;

using detail::group_info;

namespace group_
{

using detail::group_::make;
using detail::group_::size_v;

}

}
