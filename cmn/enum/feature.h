#pragma once

#include <cmn/meta/concepts.h>
#include <cmn/util/feature.h>
#include <cmn/enum/util.h>

namespace cmn
{

namespace enum_
{

namespace detail
{

template <typename Policy, c::bitfield Feature>
    requires (c::adapted_enum<Feature> && !nullable_v<Feature>)
[[nodiscard]] constexpr auto set_feature_adapted(Policy pol, Feature feat) noexcept -> Policy
{
    std::ignore = fold
    (
          groups_v<Feature>
        , masks_v<Feature>
        , feat
        , [&pol](record_info<Feature> const& rec, mask_type_t<Feature> mask)
        {
            pol = static_cast<Policy>(set_feature_(pol, rec.m_value, mask));
            return false;
        }
    );

    return pol;
}

template <typename Policy, c::bitfield Feature> requires c::adapted_enum<Feature>
constexpr auto inplace_set_feature_adapted(Policy &pol, Feature feat) noexcept -> Policy &
{
    auto const mask = mask_by_enum(feat);
    return pol = static_cast<Policy>(set_feature_(pol, feat, mask));
}

}

///////////////////////////////////////////////////////////////////////////////

template <c::bitfield Policy>
    requires (c::adapted_enum<Policy> && !nullable_v<Policy>)
[[nodiscard]] constexpr auto set_feature(interop_type_t<Policy> pol, Policy feat) noexcept -> interop_type_t<Policy>
{
    return detail::set_feature_adapted(pol, feat);
}

template <c::strong_bitfield Policy>
    requires (c::adapted_enum<Policy> && !nullable_v<Policy>)
[[nodiscard]] constexpr auto set_feature(Policy pol, Policy feat) noexcept -> Policy
{
    return detail::set_feature_adapted(pol, feat);
}

///////////////////////////////////////////////////////////////////////////////
template <c::bitfield Policy, c::bitfield... Features>
    requires (... && std::same_as<Policy, Features>) && c::adapted_enum<Policy>
[[nodiscard]] constexpr auto set_features(interop_type_t<Policy> pol, Policy feat, Features... feats) noexcept
    -> interop_type_t<Policy>
{
    using detail::inplace_set_feature_adapted;
    return (inplace_set_feature_adapted(pol, feat), ..., inplace_set_feature_adapted(pol, feats));
}

template <c::strong_bitfield Policy, c::strong_bitfield... Features>
    requires (... && std::same_as<Policy, Features>) && c::adapted_enum<Policy>
[[nodiscard]] constexpr auto set_features(Policy pol, Policy feat, Features... feats) noexcept -> Policy
{
    using detail::inplace_set_feature_adapted;
    return (inplace_set_feature_adapted(pol, feat), ..., inplace_set_feature_adapted(pol, feats));
}

}

using enum_::set_feature;
using enum_::set_features;

}
