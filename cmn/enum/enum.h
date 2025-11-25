#pragma once

#include <cmn/enum/detail/macro.h>

#include <cmn/enum/util.h>
// ReSharper disable once CppUnusedIncludeDirective
#include <cmn/enum/op.h>

namespace cmn::enum_
{

template <c::enum_ auto En_, decltype(En_) ... Ens_>
consteval auto adapt_enum_info_helper(interop_type_t<op_t> ops = default_ops(En_, kind_t::enum_))
{
    return enum_info { ops, group_::make<En_, Ens_...>() };
}

}

///////////////////////////////////////////////////////////////////////////////
#define CMN_ENUM_ADAPT_ENUM_INFO(name, group_nvp_seq) \
    consteval auto adapt_type_info(name)    \
    {   \
        using enum name;    \
        return ::cmn::enum_::adapt_enum_info_helper<CMN_ENUM_EXTRACT_GROUP_SEQ(group_nvp_seq)>();  \
    }

///////////////////////////////////////////////////////////////////////////////
//
// CMN_ENUM_ADAPT_ENUM()
//
// usage:
// enum en_t
// {
//       one
//     , four_ = 4
//     , five
//     , mask = one | four_
//     , mask2 = five
// };
// 
// CMN_ENUM_ADAPT_ENUM
// (
//     en_t,
//     (one)
//     (four)
//     (five)
// )
//
///////////////////////////////////////////////////////////////////////////////
#define CMN_ENUM_ADAPT_ENUM(name, group_nvp_seq)                                        \
    CMN_ENUM_ADAPT_ENUM_INFO(name, group_nvp_seq)                                       \
    CMN_ENUM_INJECT_OPS()

///////////////////////////////////////////////////////////////////////////////
//
// CMN_ENUM_DEFINE_ENUM()
//
// usage:
// CMN_ENUM_DEFINE_ENUM
// (
//    en_t,
//    (one)
//    (four_, 4)
//    (five)
//    ,
//    (mask)
//    (mask2, 7)
// )
//
///////////////////////////////////////////////////////////////////////////////

#define CMN_ENUM_DEFINE_ENUM(name, group_nvp_seq, ...)                                  \
    CMN_ENUM_DEFINE_HEADER(, name, )                                                    \
        CMN_ENUM_DEFINE_GROUP_BODY(group_nvp_seq)                                       \
        CMN_ENUM_DEFINE_MASK_BODY(__VA_ARGS__)                                          \
    CMN_ENUM_DEFINE_FOOTER()                                                            \
    CMN_ENUM_ADAPT_ENUM_INFO(name, group_nvp_seq)                                       \
    CMN_ENUM_INJECT_OPS()

///////////////////////////////////////////////////////////////////////////////
#define CMN_ENUM_DEFINE_ENUM_BASE(name, base, group_nvp_seq, ...)                       \
    CMN_ENUM_DEFINE_HEADER(, name, : base)                                              \
        CMN_ENUM_DEFINE_GROUP_BODY(group_nvp_seq)                                       \
        CMN_ENUM_DEFINE_MASK_BODY(__VA_ARGS__)                                          \
    CMN_ENUM_DEFINE_FOOTER()                                                            \
    CMN_ENUM_ADAPT_ENUM_INFO(name, group_nvp_seq)                                       \
    CMN_ENUM_INJECT_OPS()

///////////////////////////////////////////////////////////////////////////////
#define CMN_ENUM_ADAPT_ENUM_CLASS(name, group_nvp_seq)                                  \
    CMN_ENUM_ADAPT_ENUM(name, group_nvp_seq)

///////////////////////////////////////////////////////////////////////////////
#define CMN_ENUM_DEFINE_ENUM_CLASS(name, group_nvp_seq, ...)                            \
    CMN_ENUM_DEFINE_HEADER(class, name, )                                               \
        CMN_ENUM_DEFINE_GROUP_BODY(group_nvp_seq)                                       \
        CMN_ENUM_DEFINE_MASK_BODY(__VA_ARGS__)                                          \
    CMN_ENUM_DEFINE_FOOTER()                                                            \
    CMN_ENUM_ADAPT_ENUM_INFO(name, group_nvp_seq)                                       \
    CMN_ENUM_INJECT_OPS()


///////////////////////////////////////////////////////////////////////////////
#define CMN_ENUM_DEFINE_ENUM_CLASS_BASE(name, base, group_nvp_seq, ...)                 \
    CMN_ENUM_DEFINE_HEADER(class, name, : base)                                         \
        CMN_ENUM_DEFINE_GROUP_BODY(group_nvp_seq)                                       \
        CMN_ENUM_DEFINE_MASK_BODY(__VA_ARGS__)                                          \
    CMN_ENUM_DEFINE_FOOTER()                                                            \
    CMN_ENUM_ADAPT_ENUM_INFO(name, group_nvp_seq)                                       \
    CMN_ENUM_INJECT_OPS()

#ifdef UNITY_BUILD
#   include "io.h"
#endif