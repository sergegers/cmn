#pragma once

#include <tuple>
#include <array>
#include <limits>
#include <string_view>
#include <string>
#include <tuple>
#include <bit>
#include <algorithm>
#include <iosfwd>
#include <utility>
#include <sstream>

#include <cmn/meta/concepts.h>
#include <cmn/meta/type_traits.h>
#include <cmn/meta/symbols.h>

#include <cmn/util/util.h>  // make_index_sequence<>

#include "traits.h"
#include "concepts.h"

#include <cmn/enum/detail/name_info.h>

namespace cmn::enum_
{

///////////////////////////////////////////////////////////////////////////////
//
// Trait shortcuts
//
///////////////////////////////////////////////////////////////////////////////
template <c::enum_ En> constexpr kind_t kind_v = traits<En>::kind;
template <c::enum_ En> constexpr int ops_v = traits<En>::ops;
template <c::enum_ Enum> constexpr c::instance_of<name_info> auto name_info_v = traits<Enum>::name_info;

template <c::e_any_enum Enum> constexpr Enum begin_v = traits<Enum>::begin;
template <c::e_any_enum Enum> constexpr Enum last_v = traits<Enum>::last;
template <c::e_any_enum Enum> constexpr Enum end_v = traits<Enum>::end;
template <c::e_any_enum Enum> constexpr /*c::instance_of<enum_info>*/ auto enum_info_v = traits<Enum>::enum_info;
template <c::e_any_enum Enum> constexpr /*util::c::groups_info*/ auto groups_v = enum_info_v<Enum>.m_groups;
template <c::e_any_enum Enum, std::size_t GroupId_> constexpr auto records_v = std::get<GroupId_>(groups_v<Enum>);
template <c::e_any_enum Enum> constexpr auto masks_v = traits<Enum>::masks;

template <c::enum_ Enum, typename Char, typename CharTraits>
constexpr auto name(Enum, std::basic_ios<Char, CharTraits> const &) noexcept
{
    if constexpr (std::is_same_v<Char, char>) return name_info_v<Enum>.m_name;
    else if constexpr (std::is_same_v<Char, wchar_t>) return name_info_v<Enum>.m_wname;
    else static_assert(!std::is_same_v<Char, Char>, "Not implemented");
}

namespace detail
{

template <c::e_any_enum auto Mask_>
struct group_by_mask_
{
    using enum_type = decltype(Mask_);

    template <std::size_t Idx_>
    consteval auto const &operator ()() const
    {
        if constexpr (masks_v<enum_type>[Idx_] == to_interop(Mask_))
            return std::get<Idx_>(groups_v<enum_type>);
        else
            return group_by_mask_<Mask_>{}.template operator()<Idx_ + 1>();
    }

    template <std::size_t Idx_>
    consteval auto const &operator ()() const
        requires (Idx_ == std::size(masks_v<enum_type>))
    {
        if constexpr (masks_v<enum_type>[Idx_] == to_interop(Mask_))
            return std::get<Idx_>(groups_v<enum_type>);
        else
            return group_by_mask_<Mask_>{}.template operator()<Idx_ + 1>();
    }
};

}

template <c::e_any_enum auto Mask_, std::size_t Idx_ = 0>
constexpr auto const &group_by_mask()
{
    using enum_type = decltype(Mask_);

    if constexpr (masks_v<enum_type>[Idx_] == to_interop(Mask_))
        return std::get<Idx_>(groups_v<enum_type>);
    else
        return group_by_mask<Mask_, Idx_ + 1>();
}

///////////////////////////////////////////////////////////////////////////////
//
// first enum constant value in next group based on previous mask
//
template <c::enumerable Int> 
constexpr auto next_on_mask(Int prev_mask) -> Int
{
    using mask_type = std::make_unsigned_t<underlying_type_t<Int>>;
    return Int{ 1 } << (bsr(static_cast<mask_type>(prev_mask)) + 1);
}

template 
<
      c::enumerable Enum
    , c::enumerable... Enums
>
    requires (std::same_as<Enum, Enums> && ...)
constexpr auto in(Enum en, Enums ...ens) -> bool
{
    return ((en == ens) || ...);
}

//-----------------------------------------------------------------------------
template <c::enumerable Enum>
constexpr auto to_mask(Enum en) -> interop_type_t<Enum>
{
    return static_cast<interop_type_t<Enum>>(en);
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

// check enum constants consistency
template <c::e_any_enum Enum>
constexpr bool is_masks_overlapped_v = 0 != mask_overlap(traits<Enum>::masks);

}   
