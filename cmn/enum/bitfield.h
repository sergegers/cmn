#pragma once

// ReSharper disable once CppUnusedIncludeDirective

#include <cmn/enum/detail/macro.h>

#include <cmn/enum/util.h>
// ReSharper disable once CppUnusedIncludeDirective
#include <cmn/enum/op.h>
#include <cmn/enum/io/format_info.h>

namespace cmn::enum_
{

template <c::enum_ auto En_, decltype(En_) ... Ens_>
consteval auto adapt_bitfield_info_helper(interop_type_t<op_t> ops = default_ops(En_, kind_t::bitfield))
{
    return enum_info { ops, group_::make<En_>(), group_::make<Ens_>()... };
}

}

///////////////////////////////////////////////////////////////////////////////
#define CMN_ENUM_ADAPT_BITFIELD_INFO(name, group_nvp_seq) \
    consteval auto adapt_enum_info(name)    \
    {   \
        using enum name;    \
        return ::cmn::enum_::adapt_bitfield_info_helper<CMN_ENUM_EXTRACT_GROUP_SEQ(group_nvp_seq)>();  \
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
#define CMN_ENUM_ADAPT_BITFIELD(name, group_nvp_seq)                                    \
    CMN_ENUM_ADAPT_BITFIELD_INFO(name, group_nvp_seq)                                   \
    CMN_ENUM_INJECT_OPS()

///////////////////////////////////////////////////////////////////////////////
//
// CMN_ENUM_DEFINE_BITFIELD()
//
// usage:
// CMN_ENUM_DEFINE_BITFIELD
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
#define CMN_ENUM_DEFINE_BITFIELD(name, group_nvp_seq, ...)                              \
    CMN_ENUM_DEFINE_HEADER(, name, )                                                    \
        CMN_ENUM_DEFINE_GROUP_BODY(group_nvp_seq)                                       \
        CMN_ENUM_DEFINE_MASK_BODY(__VA_ARGS__)                                          \
    CMN_ENUM_DEFINE_FOOTER()                                                            \
    CMN_ENUM_ADAPT_BITFIELD_INFO(name, group_nvp_seq)                                   \
    CMN_ENUM_INJECT_OPS()

///////////////////////////////////////////////////////////////////////////////
#define CMN_ENUM_DEFINE_BITFIELD_BASE(name, base, group_nvp_seq, ...)                   \
    CMN_ENUM_DEFINE_HEADER(, name, : base)                                              \
        CMN_ENUM_DEFINE_GROUP_BODY(group_nvp_seq)                                       \
        CMN_ENUM_DEFINE_MASK_BODY(__VA_ARGS__)                                          \
    CMN_ENUM_DEFINE_FOOTER()                                                            \
    CMN_ENUM_ADAPT_BITFIELD_INFO(name, group_nvp_seq)                                   \
    CMN_ENUM_INJECT_OPS()

///////////////////////////////////////////////////////////////////////////////
#define CMN_ENUM_DEFINE_BITFIELD_CLASS(name, group_nvp_seq, ...)                        \
    CMN_ENUM_DEFINE_HEADER(class , name, )                                              \
        CMN_ENUM_DEFINE_GROUP_BODY(group_nvp_seq)                                       \
        CMN_ENUM_DEFINE_MASK_BODY(__VA_ARGS__)                                          \
    CMN_ENUM_DEFINE_FOOTER()                                                            \
    CMN_ENUM_ADAPT_BITFIELD_INFO(name, group_nvp_seq)                                   \
    CMN_ENUM_INJECT_OPS()

///////////////////////////////////////////////////////////////////////////////
#define CMN_ENUM_DEFINE_BITFIELD_CLASS_BASE(name, base, group_nvp_seq, ...)             \
    CMN_ENUM_DEFINE_HEADER(class , name, : base)                                        \
        CMN_ENUM_DEFINE_GROUP_BODY(group_nvp_seq)                                       \
        CMN_ENUM_DEFINE_MASK_BODY(__VA_ARGS__)                                          \
    CMN_ENUM_DEFINE_FOOTER()                                                            \
    CMN_ENUM_ADAPT_BITFIELD_INFO(name, group_nvp_seq)                                   \
    CMN_ENUM_INJECT_OPS()

#ifdef UNITY_BUILD
#   include "io.h"
#endif