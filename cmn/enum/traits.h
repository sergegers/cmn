#pragma once

#include <string_view>
#include <tuple>
#include <array>
#include <iosfwd>
#include <concepts>

#include <cmn/meta/concepts.h>
#include <cmn/util/util.h>        // for bsr(), to_mask_type()
#include <cmn/enum/util.h>

namespace cmn::enum_
{

////////////////////////////////////////////////////////////////////////////////

// see <cmn/meta/concepts.h>
// template <c::enum_ Enum, typename = void>
// struct traits;

// naive enum
template <c::enum_ Enum, typename /*= void*/>
struct traits
{
    using underlying_type = std::underlying_type_t<Enum>;
    using mask_type = interop_type_t<Enum>;

    static constexpr name_info<Enum> name_info{};

    static constexpr kind_t kind = kind_t::naive;
    static constexpr int   ops = op_empty;
};

//-----------------------------------------------------------------------------
template <c::enum_ Enum>
struct traits<Enum, std::void_t<decltype(adapt_enum_info(Enum{}))>>
{
    using underlying_type = std::underlying_type_t<Enum>;
    using mask_type = interop_type_t<Enum>;

    static constexpr name_info<Enum> name_info{};

    // "attempting to reference a deleted function" error here means
    // that enum definition (macros family CMN_DECLARE_ENUM_...) is not included
    static constexpr auto enum_info = adapt_enum_info(Enum{});

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // enum: 1 group with n records
    // bitfield: n groups with 1 record in each
    // combo: n groups with m records in each
    //
    ////////////////////////////////////////////////////////////////////////////////
    static constexpr kind_t kind = get_kind(enum_info);
    static constexpr int    ops = enum_info.m_ops;

    // std::array<underlying_type, group_size>
    static constexpr auto masks = calc_masks(enum_info);

    static constexpr Enum begin = min_value(enum_info);
    static constexpr Enum last = max_value(enum_info);
    static constexpr Enum end = static_cast<Enum>(static_cast<underlying_type>(last) + 1);

    //template <typename Char, std::size_t Size_>
    //static constexpr auto get_str(typename utils_type::template group_t<Size_> const &group, Enum en)
    //{
    //    auto const idx = utils_type::get_index(group, en);
    //    assert(idx >= 0);

    //    decltype(auto) rec = group[idx];
    //    return rec.template get_str<Char>();
    //}
};

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

// check enum constants consistency
template <c::e_any_enum Enum>
constexpr bool is_masks_overlapped_v = 0 != mask_overlap(traits<Enum>::masks);

template <c::e_enum Enum>
constexpr auto to_string_view(Enum en) -> std::string_view
{
    using enum_traits_type = traits<Enum>;
    return enum_traits_type::template get_str<char>(std::get<0>(enum_traits_type::groups), en);
}

template <c::e_enum Enum>
constexpr auto to_wstring_view(Enum en) -> std::wstring_view
{
    using enum_traits_type = traits<Enum>;
    return enum_traits_type::template get_str<wchar_t>(std::get<0>(enum_traits_type::groups), en);
}

template <typename Char, c::e_enum Enum>
constexpr auto to_basic_string_view(Enum en) -> std::string_view
{
    using enum_traits_type = traits<Enum>;
    return enum_traits_type::template get_str<Char>(std::get<0>(enum_traits_type::groups), en);
}

template <std::integral Enum>
constexpr auto to_mask(Enum en) -> interop_type_t<Enum>
{
    return static_cast<interop_type_t<Enum>>(en);
}

template <c::enum_ Enum>
constexpr auto to_mask(Enum en) -> interop_type_t<Enum>
{
    return static_cast<interop_type_t<Enum>>(en);
}

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
template <c::e_any_enum Enum> constexpr c::instance_of<enum_info> auto enum_info_v = traits<Enum>::enum_info;
template <c::e_any_enum Enum> constexpr util::c::groups_info auto groups_v = enum_info_v<Enum>.m_groups;
template <c::e_any_enum Enum, std::size_t GroupId_> constexpr auto records_v = std::get<GroupId_>(groups_v<Enum>);
template <c::e_any_enum Enum> constexpr auto masks_v = traits<Enum>::masks;

template <c::enum_ Enum, typename Char, typename CharTraits>
constexpr auto name(Enum, std::basic_ios<Char, CharTraits> const &) noexcept
{
    if constexpr (std::is_same_v<Char, char>) 
        return name_info_v<Enum>.m_name;
    else 
        return name_info_v<Enum>.m_wname;
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
consteval auto const &group_by_mask()
{
    using enum_type = decltype(Mask_);

    if constexpr (masks_v<enum_type>[Idx_] == to_interop(Mask_))
        return std::get<Idx_>(groups_v<enum_type>);
    else
        return group_by_mask<Mask_, Idx_ + 1>();
}

}

