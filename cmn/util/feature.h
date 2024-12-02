#pragma once

#include <concepts>

#include <cmn/meta/concepts.h>
#include <cmn/util/util.h>

namespace cmn
{

 // feature utils
template <c::enumerable Policy>
[[nodiscard]] constexpr auto empty(Policy pol) noexcept -> bool { return static_cast<interop_type_t<Policy>>(pol) == 0; }

template <c::bitfield Policy>
[[nodiscard]] constexpr auto has_feature(Policy pol, Policy feat) noexcept -> bool
{
    return feat == (pol & feat);
}

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

// mask is useful for combo, not required for bitfields
template <c::bitfield Policy>
[[nodiscard]] constexpr auto set_value(mask_type_t<Policy> pol, Policy val, mask_type_t<Policy> mask = no_mask<Policy>)
    -> mask_type_t<Policy>
{
    return pol & ~mask | val;
}

template <c::bitfield Policy>
[[nodiscard]] constexpr auto get_value(Policy pol, mask_type_t<Policy> mask = no_mask<Policy>) noexcept -> Policy
{
    return static_cast<Policy>(pol & mask);
}

template <c::bitfield Policy>
[[nodiscard]] constexpr auto set_feature(mask_type_t<Policy> pol, Policy feat) noexcept -> mask_type_t<Policy>
{
    return feat | pol;
}

template <c::bitfield Policy>
[[nodiscard]] constexpr auto reset_feature(mask_type_t<Policy> pol, Policy feat) noexcept -> mask_type_t<Policy>
{
    return ~feat & pol;
}

template <c::bitfield PolicyMask, c::bitfield... Features>
    requires (... && std::same_as<PolicyMask, mask_type_t<Features>>)
[[nodiscard]] constexpr auto reset_features(PolicyMask pol, Features... feats) noexcept -> PolicyMask
{
    return ~(... | feats) & pol;
}

template <c::bitfield Policy>
[[nodiscard]] constexpr auto enable_feature(mask_type_t<Policy> pol, Policy feat, bool enable) noexcept-> mask_type_t<Policy>
{
    return enable? set_feature(pol, feat): reset_feature(pol, feat);
}

template <c::bitfield Policy>
[[nodiscard]] constexpr auto toggle_feature(mask_type_t<Policy> pol, Policy feat) noexcept -> mask_type_t<Policy>
{
    return (feat ^ pol) & feat | pol & ~feat;
}

//-----------------------------------------------------------------------------
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

}
