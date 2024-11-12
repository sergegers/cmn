#pragma once

#include <cmn/enum/util.h>
// ReSharper disable once CppUnusedIncludeDirective
#include <cmn/enum/op.h>
#include <cmn/enum/util/macro.h>

namespace cmn::enum_
{


template <c::enum_ auto ... Ens_>
consteval auto adapt_bitfield_info_helper(unsigned int ops = default_ops(kind_t::bitfield))
{
    return enum_info
    {
        .m_ops = ops,
        .m_groups = std::make_tuple(group_::make<Ens_>()...)
    };
}

}

///////////////////////////////////////////////////////////////////////////////
#define CMN_PP_ADAPT_BITFIELD_INFO(name, group_nvp_seq) \
    consteval auto adapt_enum_info(name)    \
    {   \
        using enum name;    \
        return ::cmn::enum_::adapt_bitfield_info_helper<CMN_PP_EXTRACT_GROUP_SEQ(group_nvp_seq)>();  \
    }

///////////////////////////////////////////////////////////////////////////////
//
// CHT_PP_ADAPT_BITFIELD()
//
// usage:
// enum bf_t
// {
//    one  = 0x1,
//    four = 0x4,
//    five = 0x6
//    mask = one | four,
//    mask2 = five
// };
//
// CHT_PP_ADAPT_BITFIELD
// (
//     bf_t,
//     (one)
//     (four)
//     (five)
// )
//
///////////////////////////////////////////////////////////////////////////////
#define CMN_PP_ADAPT_BITFIELD(name, group_nvp_seq)                                        \
    CMN_PP_ADAPT_BITFIELD_INFO(name, group_nvp_seq)                                       \
    CMN_PP_INJECT_ENUM_OPS()

///////////////////////////////////////////////////////////////////////////////
//
// CMN_PP_DEFINE_BITFIELD()
//
// usage:
// CMN_PP_DEFINE_BITFIELD
// (
//     bf_t,
//     (one, 1)
//     (four, 4)
//     (five, 5)
//      ,
//     (mask, one | four)
//     (mask2, five)
// )
//
///////////////////////////////////////////////////////////////////////////////
#define CMN_PP_DEFINE_BITFIELD(name, group_nvp_seq, ...)                                    \
    CMN_PP_DEFINE_ENUM_HEADER(, name, )                                                     \
        CMN_PP_DEFINE_GROUP_BODY(group_nvp_seq)                                             \
        CMN_PP_DEFINE_MASK_BODY(__VA_ARGS__)                                                \
    CMN_PP_DEFINE_ENUM_FOOTER()                                                             \
    CMN_PP_ADAPT_BITFIELD_INFO(name, group_nvp_seq)                                         \
    CMN_PP_INJECT_ENUM_OPS()

#ifdef UNITY_BUILD
#   include "io.h"
#endif