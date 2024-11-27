#pragma once

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/punctuation/comma.hpp>
#include <boost/preprocessor/seq/for_each_i.hpp>
#include <boost/preprocessor/seq/cat.hpp>

// ReSharper disable once CppUnusedIncludeDirective
#include <cmn/enum/op.h>
#include <cmn/enum/util.h>

namespace cmn::enum_
{


//template <util::c::group_info... Groups>
//consteval auto adapt_combo_info_helper(groups_info<Groups...> &&groups, interop_type_t<op_t> ops = default_ops(kind_t::combo))
//{
//    return enum_info { ops, std::move(groups) };
//}

}

///////////////////////////////////////////////////////////////////////////////

#define CMN_MARKUP_GROUP_INFO(r, data, i, group_nvp_seq) \
    BOOST_PP_COMMA() ::cmn::enum_::group_::make<CMN_EXTRACT_GROUP_SEQ(group_nvp_seq)>()

#define CMN_ADAPT_COMBO_INFO(name, groups_seq) \
    consteval auto adapt_enum_info(name)    \
    {   \
        using enum name;    \
        return ::cmn::enum_::enum_info{   \
            ::cmn::enum_::default_ops( ::cmn::enum_::kind_t::combo) \
            BOOST_PP_SEQ_FOR_EACH_I(CMN_MARKUP_GROUP_INFO, data, groups_seq) };    \
    }

////////////////////////////////////////////////////////////////////////////////
//
// CMN_ADAPT_COMBO()
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
// CMN_ADAPT_COMBO
// (
//     cmb_t,
// 
//     ((one)(four))
//     ((sixteen))
// )
//
///////////////////////////////////////////////////////////////////////////////

#define CMN_ADAPT_COMBO(name, groups_seq)                                             \
    CMN_ADAPT_COMBO_INFO(name, groups_seq)                                            \
    CMN_INJECT_ENUM_OPS()

 ////////////////////////////////////////////////////////////////////////////////
//
// CMN_DEFINE_COMBO()
//
// usage:
// CMN_DEFINE_COMBO
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

#define CMN_DEFINE_COMBO(name, groups_seq, ...)                                          \
    CMN_DEFINE_ENUM_HEADER(, name, )                                                     \
        CMN_DEFINE_GROUP_BODY(BOOST_PP_SEQ_CAT(groups_seq))                              \
        CMN_DEFINE_MASK_BODY(__VA_ARGS__)                                                \
    CMN_DEFINE_ENUM_FOOTER()                                                             \
    CMN_ADAPT_COMBO_INFO(name, groups_seq)                                               \
    CMN_INJECT_ENUM_OPS()

///////////////////////////////////////////////////////////////////////////////
#define CMN_DEFINE_COMBO_BASE(name, base, groups_seq, ...)                               \
    CMN_DEFINE_ENUM_HEADER(, name, : base)                                               \
        CMN_DEFINE_GROUP_BODY(BOOST_PP_SEQ_CAT(groups_seq))                              \
        CMN_DEFINE_MASK_BODY(__VA_ARGS__)                                                \
    CMN_DEFINE_ENUM_FOOTER()                                                             \
    CMN_ADAPT_COMBO_INFO(name, groups_seq)                                               \
    CMN_INJECT_ENUM_OPS()

///////////////////////////////////////////////////////////////////////////////
#define CMN_ADAPT_COMBO_CLASS(name, groups_seq)                                          \
    CMN_ADAPT_COMBO(name, groups_seq)

///////////////////////////////////////////////////////////////////////////////
#define CMN_DEFINE_COMBO_CLASS(name, groups_seq, ...)                                    \
    CMN_DEFINE_ENUM_HEADER(class, name, )                                                \
        CMN_DEFINE_GROUP_BODY(BOOST_PP_SEQ_CAT(groups_seq))                              \
        CMN_DEFINE_MASK_BODY(__VA_ARGS__)                                                \
    CMN_DEFINE_ENUM_FOOTER()                                                             \
    CMN_ADAPT_COMBO_INFO(name, groups_seq)                                               \
    CMN_INJECT_ENUM_OPS()

///////////////////////////////////////////////////////////////////////////////
#define CMN_DEFINE_COMBO_CLASS_BASE(name, base, groups_seq, ...)                         \
    CMN_DEFINE_ENUM_HEADER(class, name, : base)                                          \
        CMN_DEFINE_GROUP_BODY(BOOST_PP_SEQ_CAT(groups_seq))                              \
        CMN_DEFINE_MASK_BODY(__VA_ARGS__)                                                \
    CMN_DEFINE_ENUM_FOOTER()                                                             \
    CMN_ADAPT_COMBO_INFO(name, groups_seq)                                               \
    CMN_INJECT_ENUM_OPS()

#ifdef UNITY_BUILD
#   include "io.h"
#endif