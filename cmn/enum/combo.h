#pragma once

#include "macro.h"

#define MAKE_COMBO_MASK_DEF(r, data, ITEM_)    \
    ITEM_   BOOST_PP_COMMA()

#define DECLARE_COMBO_GROUP(GROUP_SEQ_)  \
    BOOST_PP_SEQ_FOR_EACH(MAKE_COMBO_MASK_DEF, data, GROUP_SEQ_)

//-----------------------------------------------------------------------------
#define CMN_PP_DECLARE_COMBO_ITEM(r, data, _I, GROUP_SEQ_)      \
    DECLARE_COMBO_GROUP(BOOST_PP_CAT(ADD_NAME_VALUE_PAIR_1 GROUP_SEQ_, _END))

//-----------------------------------------------------------------------------
#define CMN_PP_DECLARE_COMBO_GROUPS(r, NAME_, I_, GROUP_SEQ_)                       \
    BOOST_PP_COMMA_IF(I_)                                                           \
    CMN_PP_DECLARE_COMBO_GROUP(NAME_, BOOST_PP_CAT(ADD_NAME_1 GROUP_SEQ_, _END))

#define CMN_PP_DECLARE_COMBO_GROUP(NAME_, GROUP_SEQ_)                               \
    ::std::array                                                                    \
    {                                                                               \
        BOOST_PP_SEQ_FOR_EACH(CMN_PP_DECLARE_COMBO_RECORD, NAME_, GROUP_SEQ_)       \
    }

#define CMN_PP_DECLARE_COMBO_RECORD(r, NAME_, ITEM_)            \
    utils_type::record_type                                     \
    {                                                           \
        ITEM_                                                   \
        BOOST_PP_COMMA()                                        \
        BOOST_PP_CAT(BOOST_PP_STRINGIZE(ITEM_), sv)             \
        BOOST_PP_COMMA()                                        \
        BOOST_PP_CAT(BOOST_PP_WSTRINGIZE(ITEM_), sv)            \
    }                                                           \
    BOOST_PP_COMMA()

#define CMN_PP_DECLARE_COMBO_BODY(EXPORT_, NAME_, CLASS_, BASE_, GROUP_SEQ_, ...)   \
    CMN_PP_DECLARE_ENUM_HEADER(EXPORT_, NAME_, CLASS_, BASE_)                       \
        BOOST_PP_SEQ_FOR_EACH_I(CMN_PP_DECLARE_COMBO_ITEM, data,                    \
            BOOST_PP_CAT(ADD_NAME_VALUE_PAIR_1 GROUP_SEQ_, _END))                   \
        DECLARE_ENUM_GROUP(BOOST_PP_CAT(ADD_NAME_VALUE_PAIR_1 __VA_ARGS__, _END))   \
    CMN_PP_DECLARE_ENUM_FOOTER()


////////////////////////////////////////////////////////////////////////////////
// 
// COMBO
// 
////////////////////////////////////////////////////////////////////////////////

// forwarders
#define CMN_PP_DECLARE_COMBO_FORWARDER(NAME_)   \
    CMN_PP_ENUM_DECLARE_BITWISE(NAME_)          \
    CMN_PP_ENUM_DECLARE_IO(NAME_)

#define CMN_PP_DECLARE_COMBO_INNER_FORWARDER(NAME_)     \
    CMN_PP_ENUM_INNER_DECLARE_BITWISE(NAME_)            \
    CMN_PP_ENUM_INNER_DECLARE_IO(NAME_)

#define CMN_PP_DECLARE_COMBO_EXPORT_FORWARDER(NAME_) \
    CMN_PP_ENUM_EXPORT_DECLARE_BITWISE(NAME_)        \
    CMN_PP_ENUM_EXPORT_DECLARE_IO(NAME_)

////////////////////////////////////////////////////////////////////////////////

//
// CMN_PP_DECLARE_COMBO()
//
// usage:
// CMN_PP_DECLARE_COMBO
// (
//     en_t,
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

#define CMN_PP_DECLARE_COMBO(NAME_, GROUP_SEQ_, ...)                            \
    CMN_PP_DECLARE_COMBO_BODY(, NAME_,,, GROUP_SEQ_, __VA_ARGS__)               \
    CMN_PP_BEGIN_DECLARE_ENUM_META(, NAME_, combo)                              \
        BOOST_PP_SEQ_FOR_EACH_I(CMN_PP_DECLARE_COMBO_GROUPS, NAME_, GROUP_SEQ_) \
    CMN_PP_END_DECLARE_ENUM_META()                                              \
    CMN_PP_DECLARE_COMBO_FORWARDER(NAME_)

// CMN_PP_DECLARE_COMBO_INNER()
#define CMN_PP_DECLARE_COMBO_INNER(NAME_, GROUP_SEQ_, ...)                      \
    CMN_PP_DECLARE_COMBO_BODY( , NAME_,,, GROUP_SEQ_, __VA_ARGS__)              \
    CMN_PP_BEGIN_DECLARE_ENUM_META(friend, NAME_, combo)                        \
        BOOST_PP_SEQ_FOR_EACH_I(CMN_PP_DECLARE_COMBO_GROUPS, NAME_, GROUP_SEQ_) \
    CMN_PP_END_DECLARE_ENUM_META()                                              \
    CMN_PP_DECLARE_COMBO_INNER_FORWARDER(NAME_)

// CMN_PP_DECLARE_EXPORT_COMBO()
#define CMN_PP_DECLARE_EXPORT_COMBO(NAME_, GROUP_SEQ_, ...)                     \
    CMN_PP_DECLARE_COMBO_BODY(export, NAME_,,, GROUP_SEQ_, __VA_ARGS__)         \
    CMN_PP_BEGIN_DECLARE_ENUM_META(export, NAME_, combo)                        \
    BOOST_PP_SEQ_FOR_EACH_I(CMN_PP_DECLARE_COMBO_GROUPS, NAME_, GROUP_SEQ_)     \
    CMN_PP_END_DECLARE_ENUM_META()                                              \
    CMN_PP_DECLARE_COMBO_EXPORT_FORWARDER(NAME_)

///////////////////////////////////////////////////////////////////////////////
//
//  CMN_PP_DECLARE_COMBO_BASE
//  (
//        enum_name
//      , base_type
//      ...
//      ((enum_item_0 [, enaum_value_0]))
//      ((enum_item_1 [, enaum_value_1]))
//      ...
//      (
//          (bitfield_item_0[, bitfield_value_0])
//          (bitfield_item_1[, bitfield_value_1])
//          ...
//      )
//      [,
//          (const_0, const_value_0)
//          ...
//      ]
//  )
//
///////////////////////////////////////////////////////////////////////////////

// CMN_PP_DECLARE_COMBO_BASE()
#define CMN_PP_DECLARE_COMBO_BASE(NAME_, BASE_, GROUP_SEQ_, ...)                \
    CMN_PP_DECLARE_COMBO_BODY(, NAME_,, : BASE_, GROUP_SEQ_, __VA_ARGS__)       \
    CMN_PP_BEGIN_DECLARE_ENUM_META(, NAME_, combo)                              \
        BOOST_PP_SEQ_FOR_EACH_I(CMN_PP_DECLARE_COMBO_GROUPS, NAME_, GROUP_SEQ_) \
    CMN_PP_END_DECLARE_ENUM_META()                                              \
    CMN_PP_DECLARE_COMBO_FORWARDER(NAME_)

// CMN_PP_DECLARE_COMBO_INNER_BASE()
#define CMN_PP_DECLARE_COMBO_INNER_BASE(NAME_, BASE_, GROUP_SEQ_, ...)          \
    CMN_PP_DECLARE_COMBO_BODY(, NAME_,, : BASE_, GROUP_SEQ_, __VA_ARGS__)       \
    CMN_PP_BEGIN_DECLARE_ENUM_META(friend, NAME_, combo)                        \
        BOOST_PP_SEQ_FOR_EACH_I(CMN_PP_DECLARE_COMBO_GROUPS, NAME_, GROUP_SEQ_) \
    CMN_PP_END_DECLARE_ENUM_META()                                              \
    CMN_PP_DECLARE_COMBO_INNER_FORWARDER(NAME_)

// CMN_PP_DECLARE_EXPORT_COMBO_BASE()
#define CMN_PP_DECLARE_EXPORT_COMBO_BASE(NAME_, BASE_, GROUP_SEQ_, ...)         \
    CMN_PP_DECLARE_COMBO_BODY(export, NAME_,, :BASE_, GROUP_SEQ_, __VA_ARGS__)  \
    CMN_PP_BEGIN_DECLARE_ENUM_META(, NAME_, combo)                              \
    BOOST_PP_SEQ_FOR_EACH_I(CMN_PP_DECLARE_COMBO_GROUPS, NAME_, GROUP_SEQ_)     \
    CMN_PP_END_DECLARE_ENUM_META()                                              \
    CMN_PP_DECLARE_COMBO_EXPORT_FORWARDER(NAME_)

// CMN_PP_ADAPT_COMBO()
#define CMN_PP_ADAPT_COMBO(NAME_, GROUP_SEQ_)                                   \
    CMN_PP_BEGIN_DECLARE_ENUM_META(, NAME_, combo)                              \
        BOOST_PP_SEQ_FOR_EACH_I(CMN_PP_DECLARE_COMBO_GROUPS, NAME_, GROUP_SEQ_) \
    CMN_PP_END_DECLARE_ENUM_META()                                              \
    CMN_PP_DECLARE_COMBO_FORWARDER(NAME_)

////////////////////////////////////////////////////////////////////////////////
// 
// COMBO_CLASS
// 
////////////////////////////////////////////////////////////////////////////////

// forwarders
#define CMN_PP_DECLARE_COMBO_CLASS_FORWARDER(NAME_)     \
    CMN_PP_ENUM_CLASS_DECLARE_BITWISE(NAME_)            \
    CMN_PP_ENUM_DECLARE_EQUALITY_COMPARABLE(NAME_)      \
    CMN_PP_ENUM_DECLARE_IO(NAME_)

#define CMN_PP_DECLARE_COMBO_CLASS_INNER_FORWARDER(NAME_)   \
    CMN_PP_ENUM_INNER_DECLARE_BITWISE(NAME_)                \
    CMN_PP_ENUM_INNER_DECLARE_TOTALLY_ORDERED(NAME_)        \
    CMN_PP_ENUM_INNER_DECLARE_IO(NAME_)

#define CMN_PP_DECLARE_COMBO_CLASS_EXPORT_FORWARDER(NAME_)  \
    CMN_PP_ENUM_CLASS_EXPORT_DECLARE_BITWISE(NAME_)         \
    CMN_PP_ENUM_EXPORT_DECLARE_EQUALITY_COMPARABLE(NAME_)   \
    CMN_PP_ENUM_EXPORT_DECLARE_IO(NAME_)

////////////////////////////////////////////////////////////////////////////////
#define CMN_PP_DECLARE_COMBO_CLASS_GROUPS(r, NAME_, I_, GROUP_SEQ_)     \
    BOOST_PP_COMMA_IF(I_)                                               \
    CMN_PP_DECLARE_COMBO_CLASS_GROUP(NAME_,BOOST_PP_CAT(ADD_NAME_1 GROUP_SEQ_, _END))

#define CMN_PP_DECLARE_COMBO_CLASS_GROUP(NAME_, GROUP_SEQ_)                             \
        ::std::array                                                                    \
        {                                                                               \
            BOOST_PP_SEQ_FOR_EACH(CMN_PP_DECLARE_COMBO_CLASS_RECORD, NAME_, GROUP_SEQ_) \
        }

#define CMN_PP_DECLARE_COMBO_CLASS_RECORD(r, NAME_, ITEM_)  \
    utils_type::record_type                                 \
    {                                                       \
        BOOST_PP_CAT(BOOST_PP_CAT(NAME_, ::), ITEM_)        \
        BOOST_PP_COMMA()                                    \
        BOOST_PP_CAT(BOOST_PP_STRINGIZE(ITEM_), sv)         \
        BOOST_PP_COMMA()                                    \
        BOOST_PP_CAT(BOOST_PP_WSTRINGIZE(ITEM_), sv)        \
    }                                                       \
    BOOST_PP_COMMA()

// CMN_PP_DECLARE_COMBO_CLASS()
// usage:
//
// CMN_PP_DECLARE_COMBO_CLASS
// (
//     en_t,
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
#define CMN_PP_DECLARE_COMBO_CLASS(NAME_, GROUP_SEQ_, ...)                              \
    CMN_PP_DECLARE_COMBO_BODY(, NAME_, class,, GROUP_SEQ_, __VA_ARGS__)                 \
    CMN_PP_BEGIN_DECLARE_ENUM_META(, NAME_, combo)                                      \
        BOOST_PP_SEQ_FOR_EACH_I(CMN_PP_DECLARE_COMBO_CLASS_GROUPS, NAME_, GROUP_SEQ_)   \
    CMN_PP_END_DECLARE_ENUM_META()                                                      \
    CMN_PP_DECLARE_COMBO_CLASS_FORWARDER(NAME_)

// CMN_PP_DECLARE_COMBO_CLASS_INNER()
#define CMN_PP_DECLARE_COMBO_CLASS_INNER(NAME_, GROUP_SEQ_, ...)                        \
    CMN_PP_DECLARE_COMBO_BODY(, NAME_, class,, GROUP_SEQ_, __VA_ARGS__)                 \
    CMN_PP_BEGIN_DECLARE_ENUM_META(friend, NAME_, combo)                                \
        BOOST_PP_SEQ_FOR_EACH_I(CMN_PP_DECLARE_COMBO_CLASS_GROUPS, NAME_, GROUP_SEQ_)   \
    CMN_PP_END_DECLARE_ENUM_META()                                                      \
    CMN_PP_DECLARE_COMBO_CLASS_INNER_FORWARDER(NAME_)

// CMN_PP_DECLARE_EXPORT_COMBO_CLASS()
#define CMN_PP_DECLARE_EXPORT_COMBO_CLASS(NAME_, GROUP_SEQ_, ...)                   \
    CMN_PP_DECLARE_COMBO_BODY(export, NAME_, class,, GROUP_SEQ_, __VA_ARGS__)       \
    CMN_PP_BEGIN_DECLARE_ENUM_META(export, NAME_, combo)                            \
    BOOST_PP_SEQ_FOR_EACH_I(CMN_PP_DECLARE_COMBO_CLASS_GROUPS, NAME_, GROUP_SEQ_)   \
    CMN_PP_END_DECLARE_ENUM_META()                                                  \
    CMN_PP_DECLARE_COMBO_CLASS_EXPORT_FORWARDER(NAME_)

///////////////////////////////////////////////////////////////////////////////
//
//  CMN_PP_DECLARE_COMBO_CLASS_BASE
//  (
//        enum_class_name
//      , base_type
//      ...
//      ((enum_item_0 [, enaum_value_0]))
//      ((enum_item_1 [, enaum_value_1]))
//      ...
//      (
//          (bitfield_item_0[, bitfield_value_0])
//          (bitfield_item_1[, bitfield_value_1])
//          ...
//      )
//      [,
//          (const_0, const_value_0)
//          ...
//      ]
//  )
//
///////////////////////////////////////////////////////////////////////////////

// CMN_PP_DECLARE_COMBO_CLASS_BASE()
#define CMN_PP_DECLARE_COMBO_CLASS_BASE(NAME_, BASE_, GROUP_SEQ_, ...)                  \
    CMN_PP_DECLARE_COMBO_BODY(, NAME_, class, : BASE_, GROUP_SEQ_, __VA_ARGS__)         \
    CMN_PP_BEGIN_DECLARE_ENUM_META(, NAME_, combo)                                      \
        BOOST_PP_SEQ_FOR_EACH_I(CMN_PP_DECLARE_COMBO_CLASS_GROUPS, NAME_, GROUP_SEQ_)   \
    CMN_PP_END_DECLARE_ENUM_META()                                                      \
    CMN_PP_DECLARE_COMBO_CLASS_FORWARDER(NAME_)

// CMN_PP_DECLARE_COMBO_CLASS_INNER_BASE()
#define CMN_PP_DECLARE_COMBO_CLASS_INNER_BASE(NAME_, BASE_, GROUP_SEQ_, ...)            \
    CMN_PP_DECLARE_COMBO_BODY(, NAME_, class, : BASE_, GROUP_SEQ_, __VA_ARGS__)         \
    CMN_PP_BEGIN_DECLARE_ENUM_META(friend, NAME_, combo)                                \
        BOOST_PP_SEQ_FOR_EACH_I(CMN_PP_DECLARE_COMBO_CLASS_GROUPS, NAME_, GROUP_SEQ_)   \
    CMN_PP_END_DECLARE_ENUM_META()                                                      \
    CMN_PP_DECLARE_COMBO_CLASS_INNER_FORWARDER(NAME_)

// CMN_PP_DECLARE_EXPORT_COMBO_CLASS_BASE()
#define CMN_PP_DECLARE_EXPORT_COMBO_CLASS_BASE(NAME_, BASE_, GROUP_SEQ_, ...)         \
    CMN_PP_DECLARE_COMBO_BODY(export, NAME_, class, : BASE_, GROUP_SEQ_, __VA_ARGS__) \
    CMN_PP_BEGIN_DECLARE_ENUM_META(export, NAME_, combo)                              \
    BOOST_PP_SEQ_FOR_EACH_I(CMN_PP_DECLARE_COMBO_CLASS_GROUPS, NAME_, GROUP_SEQ_)     \
    CMN_PP_END_DECLARE_ENUM_META()                                                    \
    CMN_PP_DECLARE_COMBO_CLASS_EXPORT_FORWARDER(NAME_)

// CMN_PP_ADAPT_COMBO_CLASS()
#define CMN_PP_ADAPT_COMBO_CLASS(NAME_, GROUP_SEQ_)                                     \
    CMN_PP_BEGIN_DECLARE_ENUM_META(, NAME_, combo)                                      \
        BOOST_PP_SEQ_FOR_EACH_I(CMN_PP_DECLARE_COMBO_CLASS_GROUPS, NAME_, GROUP_SEQ_)   \
    CMN_PP_END_DECLARE_ENUM_META()                                                      \
    CMN_PP_DECLARE_COMBO_CLASS_FORWARDER(NAME_)

#ifdef UNITY_BUILD
#   include "io.h"
#endif