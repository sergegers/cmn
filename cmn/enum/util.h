#pragma once

#include <tuple>
#include <array>
#include <limits>
#include <string_view>
#include <tuple>
#include <bit>
#include <algorithm>
#include <iosfwd>
#include <utility>

#include <cmn/meta/concepts.h> // kind_t, op_t, print_t
#include <cmn/meta/type_traits.h>

#include <cmn/enum/util/concepts.h>
#include <cmn/enum/util/name_info.h>
#include <cmn/enum/util/group_info.h>
#include <cmn/enum/util/groups_info.h>
#include <cmn/enum/util/enum_info.h>
#include <cmn/enum/util/macro.h>

namespace cmn::enum_
{

////////////////////////////////////////////////////////////////////////////////////////////////
//
// enum: 1 group with n records
// bitfield: n groups with 1 record in each
// combo: n groups with m records in each
//
////////////////////////////////////////////////////////////////////////////////
template <util::c::group_info... Groups> consteval auto get_kind(enum_info<Groups...> const &) -> kind_t
{
    using enum kind_t;
    return sizeof... (Groups) == 1?
        enum_:
        ((std::tuple_size_v<Groups> == 1) && ...)? bitfield: combo;
}

namespace detail
{

template <util::c::group_info Group, util::c::group_info... Groups, std::size_t... Idss_>
consteval auto calc_masks_impl(groups_info<Group, Groups...> const &groups, std::index_sequence<Idss_...>)
    -> std::array<group_::mask_type_t<Group>, sizeof... (Groups) + 1>
{
    return { group_::calc_mask(std::get<Idss_>(groups))... };
}

}

template <util::c::group_info Group, util::c::group_info... Groups>
consteval auto calc_masks(enum_info<Group, Groups...> const &enum_info)
    -> std::array<group_::mask_type_t<Group>, sizeof... (Groups) + 1>
{
    return detail::calc_masks_impl(enum_info.m_groups, std::index_sequence_for<Group, Groups...>{});
}

///////////////////////////////////////////////////////////////////////////////
consteval auto default_ops(kind_t kind) -> unsigned int
{
    using enum kind_t;
    return 
        kind == enum_? (op_comparable | op_steppable | op_io):
        kind == bitfield? (op_bitwise | op_io):
        kind == combo? (op_comparable | op_steppable | op_bitwise | op_io):
            op_empty
    ;
}

// TODO: investigate
//template <c::enumerable Enum>
//consteval auto default_ops(Enum, kind_t kind) -> unsigned int
//{
//    auto const scoped_enum = std::is_scoped_enum_v<Enum>;
//
//    using enum kind_t;
//    return 
//        kind == enum_? op_comparable | op_steppable | op_io:
//        kind == bitfield? op_bitwise | op_io:
//        kind == combo? op_comparable | op_steppable | op_bitwise | op_io:
//            op_empty
//    ;
//}

///////////////////////////////////////////////////////////////////////////////
//
// get smallest enum value
//
namespace detail
{

template <typename EnumType, std::size_t... Idss_>
consteval auto get_min_enum_value_impl(util::c::groups_info auto const &groups, std::index_sequence<Idss_...>)
    -> EnumType
{
    using mask_type = interop_type_t<EnumType>;

    return static_cast<EnumType>(std::min({ static_cast<mask_type>(group_::get_min_value(std::get<Idss_>(groups)))... }));
}


}

template <util::c::group_info Group, util::c::group_info... Groups>
consteval auto min_value(enum_info<Group, Groups...> const &enum_info) -> group_::record_enum_type_t<Group>
{
    using enum_type = group_::record_enum_type_t<Group>;
    return detail::get_min_enum_value_impl<enum_type>(enum_info.m_groups, std::index_sequence_for<Group, Groups...>{});
}

///////////////////////////////////////////////////////////////////////////////
//
// get largest enum value
//
namespace detail
{

template <typename EnumType, std::size_t... Idss_>
consteval auto get_max_value_impl(util::c::groups_info auto const &groups, std::index_sequence<Idss_...>)
    -> EnumType
{
    using mask_type = interop_type_t<EnumType>;

    return static_cast<EnumType>(std::max({ static_cast<mask_type>(group_::get_max_value(std::get<Idss_>(groups)))... }));
}

}

template <util::c::group_info Group, util::c::group_info... Groups>
consteval auto max_value(enum_info<Group, Groups...> const &enum_info) -> group_::record_enum_type_t<Group>
{
    using enum_type = group_::record_enum_type_t<Group>;
    return detail::get_max_value_impl<enum_type>(enum_info.m_groups, std::index_sequence_for<Group, Groups...>{});
}

///////////////////////////////////////////////////////////////////////////////
namespace detail
{

template <std::size_t Idx_, std::integral Mask, std::size_t Size_>
static constexpr auto united_mask(std::array<Mask, Size_> const &masks)
{
    return []<std::size_t... LeftIndices_, std::size_t... RightIndices_>
    (
          std::array<Mask, sizeof... (LeftIndices_) + sizeof... (RightIndices_) + 1> const &masks
        , std::index_sequence<LeftIndices_...> 
        , std::index_sequence<RightIndices_...> 
    )
    {
        return (0ul | ... | masks.at(LeftIndices_)) | (0ul | ... | masks.at(RightIndices_));
    }
    (
          masks
        , std::make_index_sequence<Idx_>{}
        , make_index_sequence<Idx_ + 1, Size_>{}
    );
}

}

template <std::integral Mask, std::size_t Size_> requires (Size_ > 0)
static constexpr auto mask_overlap(std::array<Mask, Size_> const &masks) -> Mask
{
    return []<std::size_t... Indices_>
    (
          std::array<Mask, sizeof... (Indices_)> const &masks
        , std::index_sequence<Indices_...> 
    )
    {
        //[[maybe_unused]] auto const umask = (... | masks.at(Indices_));
        return (... | (detail::united_mask<Indices_>(masks) & masks.at(Indices_)));
    }
    (
          masks
        , std::make_index_sequence<Size_>{}
    );
}     

}   
