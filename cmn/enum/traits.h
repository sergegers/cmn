#pragma once

#include <tuple>
#include <array>

#include <cmn/fwd.h>
#include <cmn/meta/concepts.h>
#include <cmn/meta/traits.h>

#include <cmn/enum/detail/qualified_name.h>
#include <cmn/enum/detail/name_info.h>
#include <cmn/enum/detail/record_info.h>
#include <cmn/enum/detail/group_info.h>
#include <cmn/enum/detail/enum_info.h>

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

namespace record_
{

using detail::record_::make;

}

//-----------------------------------------------------------------------------
template <c::enum_ E, typename Char, typename CharTraits>
constexpr auto name(E, std::basic_ios<Char, CharTraits> const &) noexcept
{
    return basic_qualified_name<Char, CharTraits>{ E{} };
}

///////////////////////////////////////////////////////////////////////////////
//
// enum_info shortcuts
//
///////////////////////////////////////////////////////////////////////////////
template <c::adapted_enum E>
constexpr auto enum_info_v = []
{
    // initialize through lambda to avoid linking errors during
    // constructor execution if the constructor throws exception
    return adapt_enum_info(E{});
}();

template <c::adapted_enum E> constexpr kind_t kind_v = enum_info_v<E>.kind();
template <c::adapted_enum E> constexpr interop_type_t<op_t> ops_v = enum_info_v<E>.m_ops;
template <c::adapted_enum E> constexpr E begin_v = enum_info_v<E>.min_value();
template <c::adapted_enum E> constexpr E last_v = enum_info_v<E>.max_value();
template <c::adapted_enum E> constexpr E end_v = static_cast<E>(interop_cast(last_v<E>) + 1);
template <c::adapted_enum E> constexpr auto groups_v = enum_info_v<E>.m_groups;
template <c::adapted_enum E, std::size_t GroupId_> constexpr auto group_v = std::get<GroupId_>(groups_v<E>);

template 
<
      c::adapted_enum E
    , std::size_t GroupId_
    , std::size_t RecId_
> 
inline constexpr auto record_v = group_v<E, GroupId_>.m_records[RecId_];

template <c::adapted_enum E> constexpr auto masks_v = enum_info_v<E>.m_masks;
template <c::adapted_enum E> constexpr bool nullable_v = enum_info_v<E>.nullable();
template <c::adapted_enum E> constexpr bool unique_v = enum_info_v<E>.unique();

}

