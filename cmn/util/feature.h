#pragma once

#include <concepts>

#include <cmn/meta/concepts.h>
#include <cmn/meta/type_traits.h>

#include <cmn/util/util.h>

namespace cmn
{

template <c::bitfield Policy>
[[nodiscard]] constexpr auto empty(Policy pol) noexcept -> bool { return static_cast<interop_type_t<Policy>>(pol) == 0; }

//-----------------------------------------------------------------------------
namespace detail
{

template <c::bitfield Policy>
[[nodiscard]] constexpr auto feature_(Policy pol, auto mask) noexcept -> interop_type_t<Policy>
{
    return to_interop(pol) & lazy_to_interop(mask);
}

template <c::bitfield Policy>
[[nodiscard]] constexpr auto has_feature_(auto pol, Policy feat, auto mask) noexcept -> bool
{
    return to_interop(feat) == (feature_(static_cast<Policy>(pol), mask) & to_interop(feat));
}

template <c::bitfield Policy>
[[nodiscard]] constexpr auto set_feature_(Policy pol, Policy feat, auto mask) noexcept -> interop_type_t<Policy>
{
    return feature_(pol, mask) | to_interop(feat);
}

}

//-----------------------------------------------------------------------------
template <c::bitfield Policy>
[[nodiscard]] constexpr auto feature(Policy pol, interop_type_t<Policy> mask = no_mask<Policy>) noexcept -> Policy
{
    return static_cast<Policy>(detail::feature_(pol, mask));
}

template <c::strong_bitfield Policy>
[[nodiscard]] constexpr auto feature(Policy pol, Policy mask) noexcept -> Policy
{
    return static_cast<Policy>(detail::feature_(pol, mask));
}
//-----------------------------------------------------------------------------
template <c::enumerable To, c::bitfield Policy>
[[nodiscard]] constexpr auto feature_to(Policy pol, interop_type_t<Policy> mask = no_mask<Policy>) noexcept -> To
{
    return static_cast<To>(detail::feature_(pol, mask));
}

template <c::enumerable To, c::strong_bitfield Policy>
[[nodiscard]] constexpr auto feature_to(Policy pol, Policy mask) noexcept -> To
{
    return static_cast<To>(detail::feature_(pol, mask));
}

//-----------------------------------------------------------------------------
template <c::bitfield Policy>
[[nodiscard]] constexpr auto has_feature(Policy pol, Policy feat, interop_type_t<Policy> mask = no_mask<Policy>) noexcept
    -> bool
{
    return detail::has_feature_(pol, feat, mask);
}

template <c::bitfield Policy>
[[nodiscard]] constexpr auto has_feature(interop_type_t<Policy> pol, Policy feat, interop_type_t<Policy> mask = no_mask<Policy>) noexcept
    -> bool
{
    return detail::has_feature_(pol, feat, mask);
}

template <c::strong_bitfield Policy>
[[nodiscard]] constexpr auto has_feature(Policy pol, Policy feat, Policy mask) noexcept
    -> bool
{
    return detail::has_feature_(pol, feat, mask);
}

//-----------------------------------------------------------------------------
template <c::bitfield Policy, c::bitfield... Features>
    requires (... && std::same_as<Policy, Features>)
[[nodiscard]] constexpr auto has_all_features(Policy pol, Features ...feats) noexcept -> bool
{
    return (... && has_feature(pol, feats));
}

template <c::bitfield Policy, c::bitfield... Features>
    requires (... && std::same_as<Policy, Features>)
[[nodiscard]] constexpr auto has_any_feature(Policy pol, Features ...feats) noexcept -> bool
{
    return (... || has_feature(pol, feats));
}

template <c::bitfield Policy>
[[nodiscard]] constexpr auto is_feature_added(Policy old_pol, Policy new_pol, Policy feat) noexcept -> bool
{
    return !has_feature(old_pol, feat) && has_feature(new_pol, feat);
}

template <c::bitfield Policy>
[[nodiscard]] constexpr auto is_feature_removed(Policy old_pol, Policy new_pol, Policy feat) noexcept -> bool
{
    return has_feature(old_pol, feat) && !has_feature(new_pol, feat);
}

template <c::bitfield Policy>
[[nodiscard]] constexpr auto is_feature_changed(Policy old_pol, Policy new_pol, Policy feat) noexcept -> bool
{
    return is_feature_added(old_pol, new_pol, feat) || is_feature_removed(old_pol, new_pol, feat);
}

//-----------------------------------------------------------------------------
template <c::bitfield Policy>
[[nodiscard]] constexpr auto set_feature(Policy pol, Policy feat, mask_type_t<Policy> mask = no_mask<Policy>) noexcept
    -> Policy
{
    return static_cast<Policy>(detail::set_feature_(pol, feat, mask));
}

template <c::strong_bitfield Policy>
[[nodiscard]] constexpr auto set_feature(Policy pol, Policy feat, Policy mask) noexcept -> Policy
{
    return static_cast<Policy>(detail::set_feature_(pol, feat, mask));
}

//-----------------------------------------------------------------------------
template <c::bitfield Policy>
[[nodiscard]] constexpr auto reset_feature(Policy pol, Policy feat, mask_type_t<Policy> mask = no_mask<Policy>) noexcept
    -> Policy
{
    return static_cast<Policy>(detail::feature_(pol, mask) & ~to_interop(feat));
}


//-----------------------------------------------------------------------------
template <c::bitfield PolicyMask, c::bitfield... Features>
    requires (... && std::same_as<PolicyMask, Features>)
[[nodiscard]] constexpr auto reset_features(PolicyMask pol, Features... feats) noexcept -> PolicyMask
{
    return ~(... | feats) & pol;
}


//-----------------------------------------------------------------------------
template <c::bitfield Policy>
[[nodiscard]] constexpr auto enable_feature(Policy pol, Policy feat, bool enable) noexcept-> Policy
{
    return enable? set_feature(pol, feat): reset_feature(pol, feat);
}

template <c::bitfield Policy>
[[nodiscard]] constexpr auto toggle_feature(Policy pol, Policy feat) noexcept -> Policy
{
    return (feat ^ pol) & feat | pol & ~feat;
}

///////////////////////////////////////////////////////////////////////////////
//
// mask utils
//
namespace detail
{

template <c::enumerable Policy>
[[nodiscard]] constexpr auto value_(Policy pol, mask_type_t<Policy> mask = no_mask<Policy>) noexcept
    -> mask_type_t<Policy>
{
    return to_interop(pol) & mask;
}

}

template <c::enumerable Policy>
[[nodiscard]] constexpr auto value(Policy pol, mask_type_t<Policy> mask = no_mask<Policy>) noexcept -> Policy
{
    return static_cast<Policy>(detail::value_(pol, mask));
}

template <c::enumerable Policy>
[[nodiscard]] constexpr auto set_value(Policy pol, Policy val, mask_type_t<Policy> mask = no_mask<Policy>) noexcept
    -> Policy
{
    return static_cast<Policy>(detail::value_(pol, ~mask) | detail::value_(val, mask));
}

template <c::enumerable Policy>
[[nodiscard]] constexpr auto has_value(Policy pol, mask_type_t<Policy> mask = no_mask<Policy>) noexcept -> bool
{
    return 0 != detail::value_(pol, mask);
}

template <c::enumerable Policy>
[[nodiscard]] constexpr auto reset_value(Policy pol, Policy val, mask_type_t<Policy> mask = no_mask<Policy>) noexcept
    -> Policy
{
    return static_cast<Policy>(to_interop(pol) & ~to_interop(val) & mask);
}

///////////////////////////////////////////////////////////////////////////////
template <c::enumerable I>
[[nodiscard]] constexpr auto next(I i) noexcept { return static_cast<I>(to_underlying(i) + 1); }

// useful for combo
template <c::bitfield I>
[[nodiscard]] constexpr auto next_with_mask(I i, I mask) noexcept
{
    auto const value = to_underlying(i);
    auto const mask_value = to_underlying(mask);
    auto masked = value & mask_value;
    auto const other =  value & ~mask_value;
    return static_cast<I>(++masked | other);
}

//-----------------------------------------------------------------------------
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

}
