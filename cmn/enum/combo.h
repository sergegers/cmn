#pragma once

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/punctuation/comma.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/seq/cat.hpp>

// ReSharper disable once CppUnusedIncludeDirective
#include <cmn/enum/detail/macro.h>
#include <cmn/enum/util.h>
// ReSharper disable once CppUnusedIncludeDirective
#include <cmn/enum/op.h>
#include <cmn/enum/io/format_info.h>

///////////////////////////////////////////////////////////////////////////////

#define CMN_ENUM_MARKUP_COMBO_GROUP_INFO(r, data, group_nvp_seq) \
    BOOST_PP_COMMA() ::cmn::enum_::group_::make<CMN_ENUM_EXTRACT_GROUP_SEQ(group_nvp_seq)>()

#define CMN_ENUM_ADAPT_COMBO_INFO(name, groups_seq) \
    consteval auto adapt_type_info(name)    \
    {   \
        using enum name;    \
        return ::cmn::enum_::enum_info{   \
            ::cmn::enum_::default_ops(name{}, ::cmn::enum_::kind_t::combo) \
            BOOST_PP_SEQ_FOR_EACH(CMN_ENUM_MARKUP_COMBO_GROUP_INFO, data, groups_seq) };    \
    }

////////////////////////////////////////////////////////////////////////////////
//
// CMN_ENUM_ADAPT_COMBO()
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
// CMN_ENUM_ADAPT_COMBO
// (
//     cmb_t,
// 
//     ((one)(four))
//     ((sixteen))
// )
//
///////////////////////////////////////////////////////////////////////////////

#define CMN_ENUM_ADAPT_COMBO(name, groups_seq)                                          \
    CMN_ENUM_ADAPT_COMBO_INFO(name, groups_seq)                                         \
    CMN_ENUM_INJECT_OPS()

 ////////////////////////////////////////////////////////////////////////////////
//
// CMN_ENUM_DEFINE_COMBO()
//
// usage:
// CMN_ENUM_DEFINE_COMBO
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

#define CMN_ENUM_DEFINE_COMBO(name, groups_seq, ...)                                    \
    CMN_ENUM_DEFINE_HEADER(, name, )                                                    \
        CMN_ENUM_DEFINE_GROUP_BODY(BOOST_PP_SEQ_CAT(groups_seq))                        \
        CMN_ENUM_DEFINE_MASK_BODY(__VA_ARGS__)                                          \
    CMN_ENUM_DEFINE_FOOTER()                                                            \
    CMN_ENUM_ADAPT_COMBO_INFO(name, groups_seq)                                         \
    CMN_ENUM_INJECT_OPS()

///////////////////////////////////////////////////////////////////////////////
#define CMN_ENUM_DEFINE_COMBO_BASE(name, base, groups_seq, ...)                         \
    CMN_ENUM_DEFINE_HEADER(, name, : base)                                              \
        CMN_ENUM_DEFINE_GROUP_BODY(BOOST_PP_SEQ_CAT(groups_seq))                        \
        CMN_ENUM_DEFINE_MASK_BODY(__VA_ARGS__)                                          \
    CMN_ENUM_DEFINE_FOOTER()                                                            \
    CMN_ENUM_ADAPT_COMBO_INFO(name, groups_seq)                                         \
    CMN_ENUM_INJECT_OPS()

///////////////////////////////////////////////////////////////////////////////
#define CMN_ENUM_ADAPT_COMBO_CLASS(name, groups_seq)                                    \
    CMN_ENUM_ADAPT_COMBO(name, groups_seq)

///////////////////////////////////////////////////////////////////////////////
#define CMN_ENUM_DEFINE_COMBO_CLASS(name, groups_seq, ...)                              \
    CMN_ENUM_DEFINE_HEADER(class, name, )                                               \
        CMN_ENUM_DEFINE_GROUP_BODY(BOOST_PP_SEQ_CAT(groups_seq))                        \
        CMN_ENUM_DEFINE_MASK_BODY(__VA_ARGS__)                                          \
    CMN_ENUM_DEFINE_FOOTER()                                                            \
    CMN_ENUM_ADAPT_COMBO_INFO(name, groups_seq)                                         \
    CMN_ENUM_INJECT_OPS()

///////////////////////////////////////////////////////////////////////////////
#define CMN_ENUM_DEFINE_COMBO_CLASS_BASE(name, base, groups_seq, ...)                   \
    CMN_ENUM_DEFINE_HEADER(class, name, : base)                                         \
        CMN_ENUM_DEFINE_GROUP_BODY(BOOST_PP_SEQ_CAT(groups_seq))                        \
        CMN_ENUM_DEFINE_MASK_BODY(__VA_ARGS__)                                          \
    CMN_ENUM_DEFINE_FOOTER()                                                            \
    CMN_ENUM_ADAPT_COMBO_INFO(name, groups_seq)                                         \
    CMN_ENUM_INJECT_OPS()

#ifdef UNITY_BUILD
#   include "io.h"
#endif