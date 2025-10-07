#pragma once

#include <cmn/meta/concepts.h>
#include <cmn/util/feature.h>
#include <cmn/enum/traits.h>

namespace cmn
{
namespace enum_
{

///////////////////////////////////////////////////////////////////////////////
namespace detail
{

template <typename Policy, c::bitfield Feature>
constexpr auto inplace_set_feature_(Policy &pol, Feature feat) noexcept -> Policy &
{
    auto old_pol = pol;
    auto const mask = mask_by_enum(feat);
    pol = static_cast<Policy>(set_feature_(pol, feat, mask));
    std::cout << "policy:" << old_pol << "->" << pol << " feature" << feat << " mask:" << mask << "\n";

    return pol;
}

template <c::strong_bitfield Policy>
constexpr bool bitfield_or_combo_ = in(cmn::enum_::kind_v<Policy>, kind_t::bitfield, kind_t::combo);

}


template <c::strong_bitfield Policy, c::bitfield... Features>
    requires (... && std::same_as<Policy, Features>) && detail::bitfield_or_combo_<Policy>
[[nodiscard]] constexpr auto set_features(Policy pol, Policy feat, Features... feats) noexcept -> Policy
{
    using detail::inplace_set_feature_;
    return (inplace_set_feature_(pol, feat), ..., inplace_set_feature_(pol, feats));
}

}

}
