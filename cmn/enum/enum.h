#pragma once

#include "macro.h"

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


#ifdef UNITY_BUILD
#   include "io.h"
#endif