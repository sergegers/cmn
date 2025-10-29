#pragma once

#include <cstddef>
#include <utility>
#include <array>

#include <cmn/fwd.h>
#include <cmn/meta/concepts.h>

#include <cmn/algorithm/find.h>

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
using detail::group_::find;
using detail::group_::size_v;

}

namespace record_
{

using detail::record_::make;

}

using detail::fold;

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
template <c::adapted_enum E> constexpr c::enum_info auto enum_info_v = adapt_enum_info(E{});
template <c::adapted_enum E> constexpr kind_t kind_v = enum_info_v<E>.kind();
template <c::adapted_enum E> constexpr interop_type_t<op_t> ops_v = enum_info_v<E>.m_ops;
template <c::adapted_enum E> constexpr E begin_v = enum_info_v<E>.min_value();
template <c::adapted_enum E> constexpr E last_v = enum_info_v<E>.max_value();
template <c::adapted_enum E> constexpr E end_v = static_cast<E>(interop_cast(last_v<E>) + 1);
template <c::adapted_enum E> constexpr auto groups_v = enum_info_v<E>.m_groups;
template <c::adapted_enum E, std::size_t GroupId_> constexpr auto records_v = std::get<GroupId_>(groups_v<E>);
template <c::adapted_enum E> constexpr auto masks_v = enum_info_v<E>.m_masks;

namespace detail
{

template <c::adapted_enum auto Mask_>
struct group_by_mask_
{
    using enum_type = decltype(Mask_);

    template <std::size_t Idx_>
    consteval auto const &operator ()() const
    {
        if constexpr (lazy_to_interop(masks_v<enum_type>[Idx_]) == lazy_to_interop(Mask_))
            return std::get<Idx_>(groups_v<enum_type>);
        else
            return group_by_mask_{}.operator()<Idx_ + 1>();
    }

    template <std::size_t Idx_>
    consteval auto const &operator ()() const
        requires (Idx_ == std::size(masks_v<enum_type>))
    {
        if constexpr (lazy_to_interop(masks_v<enum_type>[Idx_]) == lazy_to_interop(Mask_))
            return std::get<Idx_>(groups_v<enum_type>);
        else
            return group_by_mask_{}.operator()<Idx_ + 1>();
    }
};

}

template <c::adapted_enum auto Mask_, std::size_t Idx_ = 0>
constexpr auto const &group_by_mask()
{
    using enum_type = decltype(Mask_);

    if constexpr (lazy_to_interop(masks_v<enum_type>[Idx_]) == lazy_to_interop(Mask_))
        return std::get<Idx_>(groups_v<enum_type>);
    else
        return group_by_mask<Mask_, Idx_ + 1>();
}

///////////////////////////////////////////////////////////////////////////////
//
// get first left empty bit
//
template <c::enumerable E>
constexpr auto next_step(E prev_mask)
{
    using mask_type = mask_type_t<E>;
    return 1 << (bsr(static_cast<mask_type>(prev_mask)) + 1);
}

///////////////////////////////////////////////////////////////////////////////
template <c::enum_ E>
consteval auto default_ops(E, kind_t kind) -> interop_type_t<kind_t>
{
    using enum kind_t;
    if constexpr (c::scoped_enum<E>)
        return 
            kind == enum_? (op_comparable | op_steppable | op_io):
            kind == bitfield? (op_bitwise | op_io):
            kind == combo? (op_comparable | op_steppable | op_bitwise | op_io):
                op_empty
        ;
    else
        // by default use builtin operators for C enums
        return in(kind, enum_, bitfield, combo)? op_io: op_empty;
}

///////////////////////////////////////////////////////////////////////////////
namespace detail
{

template <std::size_t Idx_, c::enumerable Mask, std::size_t Size_>
static constexpr auto united_mask(std::array<Mask, Size_> const &masks)
{
    return []<std::size_t... LeftIndices_, std::size_t... RightIndices_>
    (
          std::array<Mask, sizeof... (LeftIndices_) + sizeof... (RightIndices_) + 1> const &masks
        , std::index_sequence<LeftIndices_...> 
        , std::index_sequence<RightIndices_...> 
    )
    {
        return (0ul | ... | lazy_to_interop(masks.at(LeftIndices_))) | (0ul | ... | lazy_to_interop(masks.at(RightIndices_)));
    }
    (
          masks
        , std::make_index_sequence<Idx_>{}
        , make_index_sequence<Idx_ + 1, Size_>{}
    );
}

}

template <c::enumerable Mask, std::size_t Size_> requires (Size_ > 0)
static constexpr auto mask_overlap(std::array<Mask, Size_> const &masks)
{
    return []<std::size_t... Indices_>
    (
          std::array<Mask, sizeof... (Indices_)> const &masks
        , std::index_sequence<Indices_...> 
    )
    {
        //[[maybe_unused]] auto const umask = (... | masks.at(Indices_));
        return (... | (detail::united_mask<Indices_>(masks) & lazy_to_interop(masks.at(Indices_))));
    }
    (
          masks
        , std::make_index_sequence<Size_>{}
    );
}     

// check enum constants consistency
template <c::adapted_enum E> constexpr bool is_masks_overlapped_v = 0 != mask_overlap(masks_v<E>);

///////////////////////////////////////////////////////////////////////////////
template <c::adapted_enum E>
[[nodiscard]] constexpr auto mask_by_enum(E en) noexcept -> mask_type_t<E>
{
    auto const &groups = groups_v<E>;
    auto const &masks = masks_v<E>;

    mask_type_t<E> res = no_mask<E>;
    std::ignore = fold
    (
        groups,
        masks,
        en,
        [en, &res](record_info<E> const &rec, mask_type_t<E> mask) constexpr
        {
            if (rec.m_value == en)
            {
                res = mask;
                return true;
            }
            else
            {
                return false;
            }
        }
    );


    return res;
}

///////////////////////////////////////////////////////////////////////////////
template <c::adapted_enum E>
[[nodiscard]] constexpr bool contains_zero_v = [] constexpr -> bool
    {
        return find_if_fus
        (
            groups_v<E>,
            [](auto const& group) -> bool
            {
                return std::ranges::binary_search(group.m_records, 0, {}, &record_info<E>::as_interop);
            }
        )
            != -1;
    }
();

}

