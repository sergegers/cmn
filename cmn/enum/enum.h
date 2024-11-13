#pragma once

// ReSharper disable once CppUnusedIncludeDirective
#include <cmn/enum/op.h>
#include <cmn/enum/util.h>

namespace cmn::enum_
{

template <c::enum_ auto ... Ens_>
consteval auto adapt_enum_info_helper(unsigned int ops = default_ops(kind_t::enum_))
{
    return enum_info
    {
        .m_ops = ops,
        .m_groups = groups_::make(group_::make<Ens_...>())
    };
}

}

///////////////////////////////////////////////////////////////////////////////
#define CMN_PP_ADAPT_ENUM_INFO(name, group_nvp_seq) \
    consteval auto adapt_enum_info(name)    \
    {   \
        using enum name;    \
        return ::cmn::enum_::adapt_enum_info_helper<CMN_PP_EXTRACT_GROUP_SEQ(group_nvp_seq)>();  \
    }

///////////////////////////////////////////////////////////////////////////////
//
// CHT_PP_ADAPT_ENUM()
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
// CHT_PP_ADAPT_ENUM
// (
//     en_t,
//     (one)
//     (four)
//     (five)
// )
//
///////////////////////////////////////////////////////////////////////////////
#define CMN_PP_ADAPT_ENUM(name, group_nvp_seq)                                        \
    CMN_PP_ADAPT_ENUM_INFO(name, group_nvp_seq)                                       \
    CMN_PP_INJECT_ENUM_OPS()


///////////////////////////////////////////////////////////////////////////////
//
// CMN_PP_DEFINE_ENUM()
//
// usage:
// CMN_PP_DEFINE_ENUM
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

#define CMN_PP_DEFINE_ENUM(name, group_nvp_seq, ...)                                        \
    CMN_PP_DEFINE_ENUM_HEADER(, name, )                                                     \
        CMN_PP_DEFINE_GROUP_BODY(group_nvp_seq)                                             \
        CMN_PP_DEFINE_MASK_BODY(__VA_ARGS__)                                                \
    CMN_PP_DEFINE_ENUM_FOOTER()                                                             \
    CMN_PP_ADAPT_ENUM_INFO(name, group_nvp_seq)                                             \
    CMN_PP_INJECT_ENUM_OPS()

///////////////////////////////////////////////////////////////////////////////
#define CMN_PP_DEFINE_ENUM_BASE(name, base, group_nvp_seq, ...)                             \
    CMN_PP_DEFINE_ENUM_HEADER(, name, : base)                                               \
        CMN_PP_DEFINE_GROUP_BODY(group_nvp_seq)                                             \
        CMN_PP_DEFINE_MASK_BODY(__VA_ARGS__)                                                \
    CMN_PP_DEFINE_ENUM_FOOTER()                                                             \
    CMN_PP_ADAPT_ENUM_INFO(name, group_nvp_seq)                                             \
    CMN_PP_INJECT_ENUM_OPS()

///////////////////////////////////////////////////////////////////////////////
#define CMN_PP_ADAPT_ENUM_CLASS(name, group_nvp_seq)                                        \
    CMN_PP_ADAPT_ENUM(name, group_nvp_seq)

///////////////////////////////////////////////////////////////////////////////
#define CMN_PP_DEFINE_ENUM_CLASS(name, group_nvp_seq, ...)                                  \
    CMN_PP_DEFINE_ENUM_HEADER(class, name, )                                                \
        CMN_PP_DEFINE_GROUP_BODY(group_nvp_seq)                                             \
        CMN_PP_DEFINE_MASK_BODY(__VA_ARGS__)                                                \
    CMN_PP_DEFINE_ENUM_FOOTER()                                                             \
    CMN_PP_ADAPT_ENUM_INFO(name, group_nvp_seq)                                             \
    CMN_PP_INJECT_ENUM_OPS()


///////////////////////////////////////////////////////////////////////////////
#define CMN_PP_DEFINE_ENUM_CLASS_BASE(name, base, group_nvp_seq, ...)                       \
    CMN_PP_DEFINE_ENUM_HEADER(class, name, : base)                                          \
        CMN_PP_DEFINE_GROUP_BODY(group_nvp_seq)                                             \
        CMN_PP_DEFINE_MASK_BODY(__VA_ARGS__)                                                \
    CMN_PP_DEFINE_ENUM_FOOTER()                                                             \
    CMN_PP_ADAPT_ENUM_INFO(name, group_nvp_seq)                                             \
    CMN_PP_INJECT_ENUM_OPS()

#ifdef UNITY_BUILD
#   include "io.h"
#endif