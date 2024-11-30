#pragma once

#include <string_view>

#include <cmn/meta/concepts.h>

#include <cmn/enum/kind.h>
#include <cmn/enum/info.h>

namespace cmn::enum_
{

////////////////////////////////////////////////////////////////////////////////
// naive enum
template <c::enum_ Enum, typename = void>
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
    using enum_type = Enum;
    using underlying_type = std::underlying_type_t<enum_type>;
    using mask_type = interop_type_t<enum_type>;

    static constexpr name_info<enum_type> name_info{};

    // "attempting to reference a deleted function" error here means
    // that enum definition (macros family CMN_ENUM_DECLARE_ENUM_...) is not included
    static constexpr auto enum_info = adapt_enum_info(enum_type{});

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // enum: 1 group with n records
    // bitfield: n groups with 1 record in each
    // combo: n groups with m records in each
    //
    ////////////////////////////////////////////////////////////////////////////////
    static constexpr kind_t kind = enum_info.kind();
    static constexpr int    ops = enum_info.m_ops;

    // std::array<mask_type, group_size>
    static constexpr auto masks = enum_info.m_masks;

    static constexpr enum_type begin = enum_info.min_value();
    static constexpr enum_type last = enum_info.max_value();
    static constexpr enum_type end = static_cast<enum_type>(static_cast<underlying_type>(last) + 1);

    //template <typename Char, std::size_t Size_>
    //static constexpr auto get_str(typename utils_type::template group_t<Size_> const &group, Enum en)
    //{
    //    auto const idx = utils_type::get_index(group, en);
    //    assert(idx >= 0);

    //    decltype(auto) rec = group[idx];
    //    return rec.template get_str<Char>();
    //}
};

}

