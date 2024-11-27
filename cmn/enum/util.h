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

}   
