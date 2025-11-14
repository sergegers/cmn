#pragma once

#include <cstddef>
#include <tuple>
#include <utility>

#include <boost/fusion/algorithm/iteration/fold.hpp>
// ReSharper disable CppUnusedIncludeDirective
#include <boost/fusion/adapted/std_tuple.hpp>
// ReSharper restore CppUnusedIncludeDirective

#if __has_include(<boost/fusion/concepts.hpp>)
#   include <boost/fusion/concepts.hpp>
#else
#   include <cmn/meta/boost/fusion/concepts.hpp>
#endif

#include <cmn/meta/concepts.h>
#include <cmn/util/util.h>
#include <cmn/util/feature.h>

#include "traits.h"

namespace cmn::enum_
{

namespace group_
{

///////////////////////////////////////////////////////////////////////////////
//
// execute operation Op if enum value chunk belongs to group and return unprocessed
// enum value remainder
//
///////////////////////////////////////////////////////////////////////////////
template <c::adapted_enum E, std::size_t Sz_>
constexpr auto find_if
(
      group_info<E, Sz_> const &group
    , E en
    , std::invocable<record_info<E> const &, mask_type_t<E>> auto const &op
    , mask_type_t<E> additional_mask = no_mask<E>
) -> E
{
    using record_type = record_info<E>;

    auto const mask = group.m_mask & additional_mask;
    if (empty(mask)) return en;
        
    // extract enum part belongs to the current group
    auto const mval = get_feature(en, mask);

    std::ignore = std::ranges::find_if
    (
        group.m_records,
        [&en, mval, &op, mask](record_type const &rec) constexpr -> bool
        {
            bool const found = (rec.m_value == mval);
            if (found)
            {
                op(rec, mask);
                en = reset_feature(en, mval);
            }
            return found;
        }
    );

    return en;
}

}

///////////////////////////////////////////////////////////////////////////////
template
<
      boost::c::fus_sequence Groups
    , c::adapted_enum E
>
constexpr auto fold
(
      Groups const &groups
    , E en
    , std::invocable<record_info<E> const&, mask_type_t<E>> auto const &op
    , mask_type_t<E> addditional_mask = no_mask<E>
) noexcept
-> E // return remainder
{
    return boost::fusion::fold
    (
        groups,
        get_feature(en, addditional_mask),
        [&op, addditional_mask]
        (E remainder, auto const &group) constexpr
        {
            // check it in group_::find()
            //if (!empty(group.m_mask & addditional_mask))

            return group_::find_if(group, remainder, op, addditional_mask);
        }
    );
}

///////////////////////////////////////////////////////////////////////////////
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
template <c::adapted_enum E> requires unique_v<E>
[[nodiscard]] constexpr auto mask_by_enum(E en) noexcept -> mask_type_t<E>
{
    auto const &groups = groups_v<E>;

    mask_type_t<E> res = no_mask<E>;
    std::ignore = fold
    (
        groups,
        en,
        [en, &res](record_info<E> const &rec, mask_type_t<E> mask) constexpr noexcept -> bool
        {
            return rec.m_value == en? res = mask, true: false;
        }
    );


    return res;
}

///////////////////////////////////////////////////////////////////////////////
template
<
    c::enumerable Enum
    , c::enumerable... Enums
>
    requires (std::same_as<Enum, Enums> && ...)
constexpr auto contains(Enum en, Enums ...ens) noexcept -> bool
{
    return ((en == ens) || ...);
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
        return contains(kind, enum_, bitfield, combo)? op_io: op_empty;
}

}
