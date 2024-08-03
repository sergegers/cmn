#pragma once

#include "macro.h"

///////////////////////////////////////////////////////////////////////////////////
// 
// BITFIELD
// 
///////////////////////////////////////////////////////////////////////////////////

// forwarders
#define CMN_PP_DECLARE_BITFIELD_FORWARDER(NAME_)    \
    CMN_PP_ENUM_DECLARE_BITWISE(NAME_)              \
    CMN_PP_ENUM_DECLARE_IO(NAME_)

#define CMN_PP_DECLARE_BITFIELD_INNER_FORWARDER(NAME_)  \
    CMN_PP_ENUM_INNER_DECLARE_BITWISE(NAME_)            \
    CMN_PP_ENUM_INNER_DECLARE_IO(NAME_)

#define CMN_PP_DECLARE_BITFIELD_EXPORT_FORWARDER(NAME_) \
    CMN_PP_ENUM_EXPORT_DECLARE_BITWISE(NAME_)           \
    CMN_PP_ENUM_EXPORT_DECLARE_IO(NAME_)

///////////////////////////////////////////////////////////////////////////////////
#define CMN_PP_DECLARE_BITFIELD_GROUP(NAME_, GROUP_SEQ_)    \
            BOOST_PP_SEQ_FOR_EACH_I(CMN_PP_DECLARE_BITFIELD_RECORD, NAME_, GROUP_SEQ_)

#define CMN_PP_DECLARE_BITFIELD_RECORD(r, NAME_, I_, ITEM_)     \
    BOOST_PP_COMMA_IF(I_)                                       \
    ::std::array                                                \
    {                                                           \
        utils_type::record_type                                 \
        {                                                       \
            ITEM_                                               \
            BOOST_PP_COMMA()                                    \
            BOOST_PP_CAT(BOOST_PP_STRINGIZE(ITEM_), sv)         \
            BOOST_PP_COMMA()                                    \
            BOOST_PP_CAT(BOOST_PP_WSTRINGIZE(ITEM_), sv)        \
        }                                                       \
    }

//
// CMN_PP_DECLARE_BITFIELD()
//
// usage:
// CMN_PP_DECLARE_BITFIELD
// (
//     en_t,
//     (one, 1)
//     (four, 4)
//     (five, 5)
//     (mask, one | four)
//     (mask2, five)
// )
#define CMN_PP_DECLARE_BITFIELD(NAME_, GROUP_SEQ_, ...)                                     \
    CMN_PP_DECLARE_ENUM_BODY(, NAME_,,, GROUP_SEQ_, __VA_ARGS__)                            \
    CMN_PP_BEGIN_DECLARE_ENUM_META(, NAME_, bitfield)                                       \
        CMN_PP_DECLARE_BITFIELD_GROUP(NAME_, BOOST_PP_CAT(ADD_NAME_1 GROUP_SEQ_, _END))     \
    CMN_PP_END_DECLARE_ENUM_META()                                                          \
    CMN_PP_DECLARE_BITFIELD_FORWARDER(NAME_)

// CMN_PP_DECLARE_BITFIELD_INNER()
#define CMN_PP_DECLARE_BITFIELD_INNER(NAME_, GROUP_SEQ_, ...)                               \
    CMN_PP_DECLARE_ENUM_BODY(, NAME_,,, GROUP_SEQ_, __VA_ARGS__)                            \
    CMN_PP_BEGIN_DECLARE_ENUM_META(, NAME_, bitfield)                                       \
        CMN_PP_DECLARE_BITFIELD_GROUP(NAME_, BOOST_PP_CAT(ADD_NAME_1 GROUP_SEQ_, _END))     \
    CMN_PP_END_DECLARE_ENUM_META()                                                          \
    CMN_PP_DECLARE_BITFIELD_INNER_FORWARDER(NAME_)

// CMN_PP_DECLARE_EXPORT_BITFIELD()
#define CMN_PP_DECLARE_EXPORT_BITFIELD(NAME_, GROUP_SEQ_, ...)                              \
    CMN_PP_DECLARE_ENUM_BODY(, NAME_,,, GROUP_SEQ_, __VA_ARGS__)                            \
    CMN_PP_BEGIN_DECLARE_ENUM_META(export, NAME_, bitfield)                                 \
        CMN_PP_DECLARE_BITFIELD_GROUP(NAME_, BOOST_PP_CAT(ADD_NAME_1 GROUP_SEQ_, _END))     \
    CMN_PP_END_DECLARE_ENUM_META()                                                          \
    CMN_PP_DECLARE_BITFIELD_EXPORT_FORWARDER(NAME_)

// CMN_PP_DECLARE_BITFIELD_BASE()
#define CMN_PP_DECLARE_BITFIELD_BASE(NAME_, BASE_, GROUP_SEQ_, ...)                         \
    CMN_PP_DECLARE_ENUM_BODY(, NAME_,, : BASE_, GROUP_SEQ_, __VA_ARGS__)                    \
    CMN_PP_BEGIN_DECLARE_ENUM_META(, NAME_, bitfield)                                       \
        CMN_PP_DECLARE_BITFIELD_GROUP(NAME_, BOOST_PP_CAT(ADD_NAME_1 GROUP_SEQ_, _END))     \
    CMN_PP_END_DECLARE_ENUM_META()                                                          \
    CMN_PP_DECLARE_BITFIELD_FORWARDER(NAME_)

// CMN_PP_DECLARE_BITFIELD_INNER_BASE()
#define CMN_PP_DECLARE_BITFIELD_INNER_BASE(NAME_, BASE_, GROUP_SEQ_, ...)                   \
    CMN_PP_DECLARE_ENUM_BODY(, NAME_,, : BASE_, GROUP_SEQ_, __VA_ARGS__)                    \
    CMN_PP_BEGIN_DECLARE_ENUM_META(friend, NAME_, bitfield)                                 \
        CMN_PP_DECLARE_BITFIELD_GROUP(NAME_, BOOST_PP_CAT(ADD_NAME_1 GROUP_SEQ_, _END))     \
    CMN_PP_END_DECLARE_ENUM_META()                                                          \
    CMN_PP_DECLARE_BITFIELD_INNER_FORWARDER(NAME_)

// CMN_PP_ADAPT_BITFILED()
#define CMN_PP_ADAPT_BITFILED(NAME_, GROUP_SEQ_)                                        \
    CMN_PP_BEGIN_DECLARE_ENUM_META(, NAME_, bitfield)                                   \
        CMN_PP_DECLARE_BITFIELD_GROUP(NAME_, BOOST_PP_CAT(ADD_NAME_1 GROUP_SEQ_, _END)) \
    CMN_PP_END_DECLARE_ENUM_META()                                                      \
    CMN_PP_DECLARE_BITFIELD_FORWARDER(NAME_)

///////////////////////////////////////////////////////////////////////////////////
// 
// BITFIELD_CLASS
// 
///////////////////////////////////////////////////////////////////////////////////

// forwarders
#define CMN_PP_DECLARE_BITFIELD_CLASS_FORWARDER(NAME_)      \
    CMN_PP_ENUM_CLASS_DECLARE_BITWISE(NAME_)                \
    CMN_PP_ENUM_DECLARE_EQUALITY_COMPARABLE(NAME_)          \
    CMN_PP_ENUM_DECLARE_IO(NAME_)

#define CMN_PP_DECLARE_BITFIELD_CLASS_INNER_FORWARDER(NAME_)    \
    CMN_PP_ENUM_INNER_DECLARE_BITWISE(NAME_)                    \
    CMN_PP_ENUM_INNER_DECLARE_TOTALLY_ORDERED(NAME_)            \
    CMN_PP_ENUM_INNER_DECLARE_IO(NAME_)

#define CMN_PP_DECLARE_BITFIELD_CLASS_EXPORT_FORWARDER(NAME_) \
    CMN_PP_ENUM_CLASS_EXPORT_DECLARE_BITWISE(NAME_)           \
    CMN_PP_ENUM_EXPORT_DECLARE_EQUALITY_COMPARABLE(NAME_)     \
    CMN_PP_ENUM_EXPORT_DECLARE_IO(NAME_)

///////////////////////////////////////////////////////////////////////////////////
#define CMN_PP_DECLARE_BITFIELD_CLASS_GROUP(NAME_, GROUP_SEQ_)    \
            BOOST_PP_SEQ_FOR_EACH_I(CMN_PP_DECLARE_BITFIELD_CLASS_RECORD, NAME_, GROUP_SEQ_)

#define CMN_PP_DECLARE_BITFIELD_CLASS_RECORD(r, NAME_, I_, ITEM_)   \
    BOOST_PP_COMMA_IF(I_)                                           \
    ::std::array                                                    \
    {                                                               \
        utils_type::record_type                                     \
        {                                                           \
            CMN_PP_QNAME(NAME_, ITEM_)                              \
            BOOST_PP_COMMA()                                        \
            BOOST_PP_CAT(BOOST_PP_STRINGIZE(ITEM_), sv)             \
            BOOST_PP_COMMA()                                        \
            BOOST_PP_CAT(BOOST_PP_WSTRINGIZE(ITEM_), sv)            \
        }                                                           \
    }

// CMN_PP_DECLARE_BITFIELD_CLASS()
//
// usage:
// CMN_PP_DECLARE_BITFIELD_CLASS
// (
//     en_t,
//     (one, 1)
//     (four, 4)
//     (five, 5)
//     (mask, one | four)
//     (mask2, five)
// )
#define CMN_PP_DECLARE_BITFIELD_CLASS(NAME_, GROUP_SEQ_, ...)                                   \
    CMN_PP_DECLARE_ENUM_BODY(, NAME_, class,, GROUP_SEQ_, __VA_ARGS__)                          \
    CMN_PP_BEGIN_DECLARE_ENUM_META(, NAME_, bitfield)                                           \
        CMN_PP_DECLARE_BITFIELD_CLASS_GROUP(NAME_, BOOST_PP_CAT(ADD_NAME_1 GROUP_SEQ_, _END))   \
    CMN_PP_END_DECLARE_ENUM_META()                                                              \
    CMN_PP_DECLARE_BITFIELD_CLASS_FORWARDER(NAME_)

// CMN_PP_DECLARE_BITFIELD_CLASS_INNER()
#define CMN_PP_DECLARE_BITFIELD_CLASS_INNER(NAME_, GROUP_SEQ_, ...)                             \
    CMN_PP_DECLARE_ENUM_BODY(, NAME_, class,, GROUP_SEQ_, __VA_ARGS__)                          \
    CMN_PP_BEGIN_DECLARE_ENUM_META(, NAME_, bitfield)                                           \
        CMN_PP_DECLARE_BITFIELD_CLASS_GROUP(NAME_, BOOST_PP_CAT(ADD_NAME_1 GROUP_SEQ_, _END))   \
    CMN_PP_END_DECLARE_ENUM_META()                                                              \
    CMN_PP_DECLARE_BITFIELD_CLASS_INNER_FORWARDER(NAME_)

// CMN_PP_DECLARE_EXPORT_BITFIELD_CLASS()
#define CMN_PP_DECLARE_EXPORT_BITFIELD_CLASS(NAME_, GROUP_SEQ_, ...)                            \
    CMN_PP_DECLARE_ENUM_BODY(export, NAME_, class,, GROUP_SEQ_, __VA_ARGS__)                    \
    CMN_PP_BEGIN_DECLARE_ENUM_META(, NAME_, bitfield)                                           \
        CMN_PP_DECLARE_BITFIELD_CLASS_GROUP(NAME_, BOOST_PP_CAT(ADD_NAME_1 GROUP_SEQ_, _END))   \
    CMN_PP_END_DECLARE_ENUM_META()                                                              \
    CMN_PP_DECLARE_BITFIELD_CLASS_EXPORT_FORWARDER(NAME_)

// CMN_PP_DECLARE_BITFIELD_CLASS_BASE()
#define CMN_PP_DECLARE_BITFIELD_CLASS_BASE(NAME_, BASE_, GROUP_SEQ_, ...)                       \
    CMN_PP_DECLARE_ENUM_BODY(, NAME_, class, : BASE_, GROUP_SEQ_, __VA_ARGS__)                  \
    CMN_PP_BEGIN_DECLARE_ENUM_META(, NAME_, bitfield)                                           \
        CMN_PP_DECLARE_BITFIELD_CLASS_GROUP(NAME_, BOOST_PP_CAT(ADD_NAME_1 GROUP_SEQ_, _END))   \
    CMN_PP_END_DECLARE_ENUM_META()                                                              \
    CMN_PP_DECLARE_BITFIELD_CLASS_FORWARDER(NAME_)

// CMN_PP_DECLARE_BITFIELD_CLASS_INNER_BASE()
#define CMN_PP_DECLARE_BITFIELD_CLASS_INNER_BASE(NAME_, BASE_, GROUP_SEQ_, ...)                 \
    CMN_PP_DECLARE_ENUM_BODY(, NAME_, class, : BASE, GROUP_SEQ_, __VA_ARGS__)                   \
    CMN_PP_BEGIN_DECLARE_ENUM_META(, NAME_, bitfield)                                           \
        CMN_PP_DECLARE_BITFIELD_CLASS_GROUP(NAME_, BOOST_PP_CAT(ADD_NAME_1 GROUP_SEQ_, _END))   \
    CMN_PP_END_DECLARE_ENUM_META()                                                              \
    CMN_PP_DECLARE_BITFIELD_CLASS_INNER_FORWARDER(NAME_)

// CMN_PP_DECLARE_EXPORT_BITFIELD_CLASS_BASE()
#define CMN_PP_DECLARE_EXPORT_BITFIELD_CLASS_BASE(NAME_, BASE_, GROUP_SEQ_, ...)                \
    CMN_PP_DECLARE_ENUM_BODY(export, NAME_, class, : BASE, GROUP_SEQ_, __VA_ARGS__)             \
    CMN_PP_BEGIN_DECLARE_ENUM_META(, NAME_, bitfield)                                           \
        CMN_PP_DECLARE_BITFIELD_CLASS_GROUP(NAME_, BOOST_PP_CAT(ADD_NAME_1 GROUP_SEQ_, _END))   \
    CMN_PP_END_DECLARE_ENUM_META()                                                              \
    CMN_PP_DECLARE_BITFIELD_CLASS_EXPORT_FORWARDER(NAME_)

// CMN_PP_ADAPT_BITFILED_CLASS()
#define CMN_PP_ADAPT_BITFILED_CLASS(NAME_, GROUP_SEQ_)                                          \
    CMN_PP_BEGIN_DECLARE_ENUM_META(, NAME_, bitfield)                                           \
        CMN_PP_DECLARE_BITFIELD_CLASS_GROUP(NAME_, BOOST_PP_CAT(ADD_NAME_1 GROUP_SEQ_, _END))   \
    CMN_PP_END_DECLARE_ENUM_META()                                                              \
    CMN_PP_DECLARE_BITFIELD_CLASS_FORWARDER(NAME_)

#ifdef UNITY_BUILD
#   include "io.h"
#endif