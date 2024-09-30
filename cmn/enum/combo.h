#pragma once

#include <boost/preprocessor/seq/cat.hpp>

#include "macro.h"

////////////////////////////////////////////////////////////////////////////////
//
// CMN_PP_ADAPT_COMBO()
//
// usage:
//     enum cmb_t
//     {
//         one      = 1,
//         four     = 4,
//         sixteen  = 16
//         mask     = one | four,
//         mask2    = sixteen
//      }
//
// CMN_PP_ADAPT_COMBO
// (
//     cmb_t,
// 
//     ((one)(four))
//     ((sixteen))
// )
//
///////////////////////////////////////////////////////////////////////////////

#define CMN_PP_ADAPT_COMBO(name, groups_seq)                                                \
    CMN_PP_ADAPT_COMBO_INFO(name, groups_seq)                                            \
    CMN_PP_INJECT_ENUM_OPS()

 ////////////////////////////////////////////////////////////////////////////////

//
// CMN_PP_DEFINE_COMBO()
//
// usage:
// CMN_PP_DEFINE_COMBO
// (
//     cmb_t,
// 
//     (
//         (one, 1)
//         (four, 4)
//     )
//     (
//         (sixteen, 16)
//     ),
//     (mask, one | four)
//     (mask2, sixteen)
// )
//
///////////////////////////////////////////////////////////////////////////////

#define CMN_PP_DEFINE_COMBO(name, groups_seq, ...)                                          \
    CMN_PP_DEFINE_ENUM_HEADER(, name, )                                                     \
        CMN_PP_DEFINE_GROUP_BODY(BOOST_PP_SEQ_CAT(groups_seq))                              \
        CMN_PP_DEFINE_MASK_BODY(__VA_ARGS__)                                                \
    CMN_PP_DEFINE_ENUM_FOOTER()                                                             \
    CMN_PP_ADAPT_COMBO_INFO(name, groups_seq)                                               \
    CMN_PP_INJECT_ENUM_OPS()

#ifdef UNITY_BUILD
#   include "io.h"
#endif