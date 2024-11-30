#pragma once

#include <cmn/meta/concepts.h>
#include <cmn/meta/type_traits.h>
#include <cmn/util/util.h>

namespace cmn
{

 // feature utils
template <c::enumerable Policy>
[[nodiscard]] constexpr auto empty(Policy pol) noexcept -> bool { return static_cast<std::intptr_t>(pol) == 0; }

template <c::strong_bitfield Policy>
[[nodiscard]] constexpr auto has_feature(Policy pol, Policy feat) noexcept -> bool
{
    return feat == (pol & feat);
}

template <c::strong_bitfield Policy, c::strong_bitfield... Features>
    requires (... && std::same_as<Policy, Features>)
[[nodiscard]] constexpr auto has_all_features(Policy pol, Features ...feats) noexcept -> bool
{
    return (... && has_feature(pol, feats));
}

template <c::strong_bitfield Policy, c::strong_bitfield... Features>
    requires (... && std::same_as<Policy, Features>)
[[nodiscard]] constexpr auto has_any_feature(Policy pol, Features ...feats) noexcept -> bool
{
    return (... || has_feature(pol, feats));
}

template <c::strong_bitfield Policy>
[[nodiscard]] constexpr auto is_feature_added(Policy old_pol, Policy new_pol, Policy feat) noexcept -> bool
{
    return !has_feature(old_pol, feat) && has_feature(new_pol, feat);
}

template <c::strong_bitfield Policy>
[[nodiscard]] constexpr auto is_feature_removed(Policy old_pol, Policy new_pol, Policy feat) noexcept -> bool
{
    return has_feature(old_pol, feat) && !has_feature(new_pol, feat);
}

template <c::strong_bitfield Policy>
[[nodiscard]] constexpr auto is_feature_changed(Policy old_pol, Policy new_pol, Policy feat) noexcept -> bool
{
    return is_feature_added(old_pol, new_pol, feat) || is_feature_removed(old_pol, new_pol, feat);
}

// mask is useful for combo, not required for bitfields
template <c::bitfield Policy>
[[nodiscard]] constexpr auto set_value(Policy pol, Policy val, interop_type_t<Policy> mask = no_mask<Policy>)
{
    return pol & ~mask | val;
}

template <c::strong_bitfield Policy>
[[nodiscard]] constexpr auto set_value(Policy pol, Policy val, Policy mask) noexcept
{
    return pol & ~mask | val;
}

template <c::bitfield Policy>
[[nodiscard]] constexpr auto get_value(Policy pol, interop_type_t<Policy> mask = no_mask<Policy>
) noexcept
{
    return pol & mask;
}

template <c::strong_bitfield Policy>
[[nodiscard]] constexpr auto get_value(Policy pol, Policy mask) noexcept -> Policy { return pol & mask; }

template <c::strong_bitfield Policy>
[[nodiscard]] constexpr auto set_feature(Policy pol, Policy feat) noexcept -> Policy{ return feat | pol; }

template <c::strong_bitfield Policy>
[[nodiscard]] constexpr auto reset_feature(Policy pol, Policy feat) noexcept -> Policy { return ~feat & pol; }

template <c::bitfield Policy>
[[nodiscard]] constexpr auto reset_feature(interop_type_t<Policy> pol, Policy feat) noexcept -> interop_type_t<Policy> 
{ 
    return ~feat & pol; 
}

template <c::bitfield Policy>
[[nodiscard]] constexpr auto enable_feature(Policy pol, Policy feat, bool enable) noexcept
{
    return enable? set_feature(pol, feat): reset_feature(pol, feat);
}

template <c::bitfield Policy>
[[nodiscard]] constexpr auto toggle_feature(Policy pol, Policy feat) noexcept { return (feat ^ pol) & feat | pol & ~feat; }

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
