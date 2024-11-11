#pragma once

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/punctuation/comma_if.hpp>
#include <boost/preprocessor/seq/for_each_i.hpp>
#include <boost/preprocessor/seq/cat.hpp>

#include <cmn/enum/util.h>
// ReSharper disable once CppUnusedIncludeDirective
#include <cmn/enum/op.h>
#include <cmn/enum/macro.h>

namespace cmn::enum_
{


template <typename... Groups>
consteval auto adapt_combo_info_helper(groups_info<Groups...> &&groups, unsigned int ops = default_ops(kind_t::combo))
{
    return enum_info
    {
        .m_ops = ops,
        .m_groups = std::move(groups)
    };
}

}

///////////////////////////////////////////////////////////////////////////////

#define CMN_PP_MARKUP_GROUP_INFO(r, data, i, group_nvp_seq) \
    BOOST_PP_COMMA_IF(i) ::cmn::enum_::group_::make<CMN_PP_EXTRACT_GROUP_SEQ(group_nvp_seq)>()

#define CMN_PP_ADAPT_COMBO_INFO(name, groups_seq) \
    consteval auto adapt_enum_info(name)    \
    {   \
        using enum name;    \
        return ::cmn::enum_::adapt_combo_info_helper(   \
            ::cmn::enum_::groups_info{ BOOST_PP_SEQ_FOR_EACH_I(CMN_PP_MARKUP_GROUP_INFO, data, groups_seq) });    \
    }

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

#define CMN_PP_ADAPT_COMBO(name, groups_seq)                                             \
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

///////////////////////////////////////////////////////////////////////////////
#define CMN_PP_ADAPT_COMBO_CLASS(name, groups_seq)                                          \
    CMN_PP_ADAPT_COMBO(name, groups_seq)

///////////////////////////////////////////////////////////////////////////////
#define CMN_PP_DEFINE_COMBO_CLASS(name, groups_seq, ...)                                    \
    CMN_PP_DEFINE_ENUM_HEADER(class, name, )                                                \
        CMN_PP_DEFINE_GROUP_BODY(BOOST_PP_SEQ_CAT(groups_seq))                              \
        CMN_PP_DEFINE_MASK_BODY(__VA_ARGS__)                                                \
    CMN_PP_DEFINE_ENUM_FOOTER()                                                             \
    CMN_PP_ADAPT_COMBO_INFO(name, groups_seq)                                               \
    CMN_PP_INJECT_ENUM_OPS()

///////////////////////////////////////////////////////////////////////////////
#define CMN_PP_DEFINE_COMBO_CLASS_BASE(name, base, groups_seq, ...)                         \
    CMN_PP_DEFINE_ENUM_HEADER(class, name, : base)                                          \
        CMN_PP_DEFINE_GROUP_BODY(BOOST_PP_SEQ_CAT(groups_seq))                              \
        CMN_PP_DEFINE_MASK_BODY(__VA_ARGS__)                                                \
    CMN_PP_DEFINE_ENUM_FOOTER()                                                             \
    CMN_PP_ADAPT_COMBO_INFO(name, groups_seq)                                               \
    CMN_PP_INJECT_ENUM_OPS()

#ifdef UNITY_BUILD
#   include "io.h"
#endif