#pragma once

#include "macro.h"

///////////////////////////////////////////////////////////////////////////////////
// 
// ENUM
// 
///////////////////////////////////////////////////////////////////////////////////

// forwarders
#define CMN_PP_DECLARE_ENUM_FORWARDER(NAME_)    \
    CMN_PP_ENUM_DECLARE_UNIT_STEPPABLE(NAME_)  \
    CMN_PP_ENUM_DECLARE_IO(NAME_)

#define CMN_PP_DECLARE_ENUM_INNER_FORWARDER(NAME_)    \
    CMN_PP_ENUM_INNER_DECLARE_UNIT_STEPPABLE(NAME_)  \
    CMN_PP_ENUM_INNER_DECLARE_IO(NAME_)

#define CMN_PP_DECLARE_ENUM_EXPORT_FORWARDER(NAME_)  \
    CMN_PP_ENUM_EXPORT_DECLARE_UNIT_STEPPABLE(NAME_) \
    CMN_PP_ENUM_EXPORT_DECLARE_IO(NAME_)

////////////////////////////////////////////////////////////////////////////////
#define CMN_PP_DECLARE_ENUM_GROUP(NAME_, GROUP_SEQ_)        \
        ::std::array                                        \
        {                                                   \
            BOOST_PP_SEQ_FOR_EACH_I(CMN_PP_DECLARE_ENUM_RECORD, NAME_, GROUP_SEQ_)  \
        }

#define CMN_PP_DECLARE_ENUM_RECORD(r, NAME_, I_, ITEM_)     \
    BOOST_PP_COMMA_IF(I_)                                   \
    utils_type::record_type                                 \
    {                                                       \
        ITEM_                                               \
        BOOST_PP_COMMA()                                    \
        BOOST_PP_CAT(BOOST_PP_STRINGIZE(ITEM_), sv)         \
        BOOST_PP_COMMA()                                    \
        BOOST_PP_CAT(BOOST_PP_WSTRINGIZE(ITEM_), sv)        \
    }

//
// CMN_PP_DECLARE_ENUM()
//
// usage:
// CMN_PP_DECLARE_ENUM
// (
//    en_t,
//    (one)
//    (four_, 4)
//    (five),
//    (mask)
//    (mask2 = 7)
// )
#define CMN_PP_DECLARE_ENUM(NAME_, GROUP_SEQ_, ...)                                 \
    CMN_PP_DECLARE_ENUM_BODY(, NAME_,,, GROUP_SEQ_, __VA_ARGS__)                    \
    CMN_PP_BEGIN_DECLARE_ENUM_META(, NAME_, enum_)                                  \
        CMN_PP_DECLARE_ENUM_GROUP(NAME_, BOOST_PP_CAT(ADD_NAME_1 GROUP_SEQ_, _END)) \
    CMN_PP_END_DECLARE_ENUM_META()                                                  \
    CMN_PP_DECLARE_ENUM_FORWARDER(NAME_)

// CMN_PP_DECLARE_ENUM_INNER()
#define CMN_PP_DECLARE_ENUM_INNER(NAME_, GROUP_SEQ_, ...)                           \
    CMN_PP_DECLARE_ENUM_BODY(, NAME_,,, GROUP_SEQ_, __VA_ARGS__)                    \
    CMN_PP_BEGIN_DECLARE_ENUM_META(friend, NAME_, enum_)                            \
        CMN_PP_DECLARE_ENUM_GROUP(NAME_, BOOST_PP_CAT(ADD_NAME_1 GROUP_SEQ_, _END)) \
    CMN_PP_END_DECLARE_ENUM_META()                                                  \
    CMN_PP_DECLARE_ENUM_INNER_FORWARDER(NAME_)

// CMN_PP_DECLARE_EXPORT_ENUM()
#define CMN_PP_DECLARE_EXPORT_ENUM(NAME_, GROUP_SEQ_, ...)                          \
    CMN_PP_DECLARE_ENUM_BODY(export, NAME_,,, GROUP_SEQ_, __VA_ARGS__)              \
    CMN_PP_BEGIN_DECLARE_ENUM_META(export, NAME_, enum_)                            \
        CMN_PP_DECLARE_ENUM_GROUP(NAME_, BOOST_PP_CAT(ADD_NAME_1 GROUP_SEQ_, _END)) \
    CMN_PP_END_DECLARE_ENUM_META()                                                  \
    CMN_PP_DECLARE_ENUM_EXPORT_FORWARDER(NAME_)

////////////////////////////////////////////////////////////////////////////////

// CMN_PP_DECLARE_ENUM_BASE()
#define CMN_PP_DECLARE_ENUM_BASE(NAME_, BASE_, GROUP_SEQ_, ...)                     \
    CMN_PP_DECLARE_ENUM_BODY(, NAME_,, : BASE_, GROUP_SEQ_, __VA_ARGS__)            \
    CMN_PP_BEGIN_DECLARE_ENUM_META(, NAME_, enum_)                                  \
        CMN_PP_DECLARE_ENUM_GROUP(NAME_, BOOST_PP_CAT(ADD_NAME_1 GROUP_SEQ_, _END)) \
    CMN_PP_END_DECLARE_ENUM_META()                                                  \
    CMN_PP_DECLARE_ENUM_FORWARDER(NAME_)

// CMN_PP_DECLARE_ENUM_INNER_BASE()
#define CMN_PP_DECLARE_ENUM_INNER_BASE(NAME_, BASE_, GROUP_SEQ_, ...)               \
    CMN_PP_DECLARE_ENUM_BODY(, NAME_,, : BASE_, GROUP_SEQ_, __VA_ARGS__)            \
    CMN_PP_BEGIN_DECLARE_ENUM_META(friend, NAME_, enum_)                            \
        CMN_PP_DECLARE_ENUM_GROUP(NAME_, BOOST_PP_CAT(ADD_NAME_1 GROUP_SEQ_, _END)) \
    CMN_PP_END_DECLARE_ENUM_META()                                                  \
    CMN_PP_DECLARE_ENUM_INNER_FORWARDER(NAME_)

// CMN_PP_DECLARE_EXPORT_ENUM_BASE()
#define CMN_PP_DECLARE_EXPORT_ENUM_BASE(NAME_, BASE_, GROUP_SEQ_, ...)              \
    CMN_PP_DECLARE_ENUM_BODY(export, NAME_,, : BASE_, GROUP_SEQ_, __VA_ARGS__)      \
    CMN_PP_BEGIN_DECLARE_ENUM_META(export, NAME_, enum_)                        \
    CMN_PP_DECLARE_ENUM_GROUP(NAME_, BOOST_PP_CAT(ADD_NAME_1 GROUP_SEQ_, _END))     \
    CMN_PP_END_DECLARE_ENUM_META()                                                  \
    CMN_PP_DECLARE_ENUM_EXPORT_FORWARDER(NAME_)

//
// CMN_PP_ADAPT_ENUM()
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
// CMN_PP_ADAPT_ENUM
// (
//     en_t,
//     (one)
//     (four)
//     (five)
//     (mask)
//     (mask2)
// )
#define CMN_PP_ADAPT_ENUM(NAME_, GROUP_SEQ_)                                        \
    CMN_PP_BEGIN_DECLARE_ENUM_META(, NAME_, enum_)                                  \
        CMN_PP_DECLARE_ENUM_GROUP(NAME_, BOOST_PP_CAT(ADD_NAME_1 GROUP_SEQ_, _END)) \
    CMN_PP_END_DECLARE_ENUM_META()                                                  \
    CMN_PP_DECLARE_ENUM_FORWARDER(NAME_)

// CMN_PP_ADAPT_ENUM_INNER()
//
// usage:
//
// class outer
// {
//     enum en_t
//     {
//         one
//         , four_ = 4
//         , five
//         , mask = one | four_
//         , mask2 = five
//     };
// };
// 
// CMN_PP_ADAPT_ENUM_INNER
// (
//     outer,
//     en_t,
//     (one)
//     (four)
//     (five)
//     (mask)
//     (mask2)
// )
#define CMN_PP_ADAPT_ENUM_INNER(CLASS_, NAME_, GROUP_SEQ_)                                  \
    CMN_PP_BEGIN_DECLARE_ENUM_META(, CMN_PP_QNAME(CLASS_, NAME_), enum_)                    \
        CMN_PP_DECLARE_ENUM_CLASS_GROUP(CLASS_, BOOST_PP_CAT(ADD_NAME_1 GROUP_SEQ_, _END))  \
    CMN_PP_END_DECLARE_ENUM_META()                                                          \
    CMN_PP_DECLARE_ENUM_FORWARDER(CMN_PP_QNAME(CLASS_, NAME_)) 

///////////////////////////////////////////////////////////////////////////////////
// 
// ENUM_CLASS
// 
///////////////////////////////////////////////////////////////////////////////////

// forwarders
#define CMN_PP_DECLARE_ENUM_CLASS_FORWARDER(NAME_)      \
    CMN_PP_ENUM_DECLARE_UNIT_STEPPABLE(NAME_)           \
    CMN_PP_ENUM_DECLARE_TOTALLY_ORDERED(NAME_)          \
    CMN_PP_ENUM_DECLARE_IO(NAME_)

#define CMN_PP_DECLARE_ENUM_CLASS_INNER_FORWARDER(NAME_)    \
    CMN_PP_ENUM_INNER_DECLARE_UNIT_STEPPABLE(NAME_)         \
    CMN_PP_ENUM_INNER_DECLARE_TOTALLY_ORDERED(NAME_)        \
    CMN_PP_ENUM_INNER_DECLARE_IO(NAME_)

#define CMN_PP_DECLARE_ENUM_CLASS_EXPORT_FORWARDER(NAME_) \
    CMN_PP_ENUM_EXPORT_DECLARE_UNIT_STEPPABLE(NAME_)      \
    CMN_PP_ENUM_EXPORT_DECLARE_TOTALLY_ORDERED(NAME_)     \
    CMN_PP_ENUM_EXPORT_DECLARE_IO(NAME_)

///////////////////////////////////////////////////////////////////////////////////
#define CMN_PP_DECLARE_ENUM_CLASS_GROUP(NAME_, GROUP_SEQ_)      \
        ::std::array                                            \
        {                                                       \
            BOOST_PP_SEQ_FOR_EACH_I(CMN_PP_DECLARE_ENUM_CLASS_RECORD, NAME_, GROUP_SEQ_)  \
        }

#define CMN_PP_DECLARE_ENUM_CLASS_RECORD(r, NAME_, I_, ITEM_)       \
    BOOST_PP_COMMA_IF(I_)                                           \
    utils_type::record_type                                         \
    {                                                               \
        BOOST_PP_CAT(BOOST_PP_CAT(NAME_, ::), ITEM_)                \
        BOOST_PP_COMMA()                                            \
        BOOST_PP_CAT(BOOST_PP_STRINGIZE(ITEM_), sv)                 \
        BOOST_PP_COMMA()                                            \
        BOOST_PP_CAT(BOOST_PP_WSTRINGIZE(ITEM_), sv)                \
    }

////////////////////////////////////////////////////////////////////////////////

// CMN_PP_DECLARE_ENUM_CLASS()
#define CMN_PP_DECLARE_ENUM_CLASS(NAME_, GROUP_SEQ_, ...)                                   \
    CMN_PP_DECLARE_ENUM_BODY(, NAME_, class,, GROUP_SEQ_, __VA_ARGS__)                      \
    CMN_PP_BEGIN_DECLARE_ENUM_META(, NAME_, enum_)                                          \
        CMN_PP_DECLARE_ENUM_CLASS_GROUP(NAME_, BOOST_PP_CAT(ADD_NAME_1 GROUP_SEQ_, _END))   \
    CMN_PP_END_DECLARE_ENUM_META()                                                          \
    CMN_PP_DECLARE_ENUM_CLASS_FORWARDER(NAME_)

// CMN_PP_DECLARE_ENUM_CLASS_INNER()
#define CMN_PP_DECLARE_ENUM_CLASS_INNER(NAME_, GROUP_SEQ_, ...)                             \
    CMN_PP_DECLARE_ENUM_BODY(, NAME_, class,, GROUP_SEQ_, __VA_ARGS__)                      \
    CMN_PP_BEGIN_DECLARE_ENUM_META(friend, NAME_, enum_)                                    \
        CMN_PP_DECLARE_ENUM_CLASS_GROUP(NAME_, BOOST_PP_CAT(ADD_NAME_1 GROUP_SEQ_, _END))   \
    CMN_PP_END_DECLARE_ENUM_META()                                                          \
    CMN_PP_DECLARE_ENUM_CLASS_INNER_FORWARDER(NAME_)

// CMN_PP_DECLARE_EXPORT_ENUM_CLASS()
#define CMN_PP_DECLARE_EXPORT_ENUM_CLASS(NAME_, GROUP_SEQ_, ...)                        \
    CMN_PP_DECLARE_ENUM_BODY(export, NAME_, class,, GROUP_SEQ_, __VA_ARGS__)            \
    CMN_PP_BEGIN_DECLARE_ENUM_META(export, NAME_, enum_)                                \
    CMN_PP_DECLARE_ENUM_CLASS_GROUP(NAME_, BOOST_PP_CAT(ADD_NAME_1 GROUP_SEQ_, _END))   \
    CMN_PP_END_DECLARE_ENUM_META()                                                      \
    CMN_PP_DECLARE_ENUM_CLASS_EXPORT_FORWARDER(NAME_)

////////////////////////////////////////////////////////////////////////////////
// CMN_PP_DECLARE_ENUM_CLASS_BASE()
#define CMN_PP_DECLARE_ENUM_CLASS_BASE(NAME_, BASE_, GROUP_SEQ_, ...)                       \
    CMN_PP_DECLARE_ENUM_BODY(, NAME_, class, : BASE_, GROUP_SEQ_, __VA_ARGS__)              \
    CMN_PP_BEGIN_DECLARE_ENUM_META(, NAME_, enum_)                                          \
        CMN_PP_DECLARE_ENUM_CLASS_GROUP(NAME_, BOOST_PP_CAT(ADD_NAME_1 GROUP_SEQ_, _END))   \
    CMN_PP_END_DECLARE_ENUM_META()                                                          \
    CMN_PP_DECLARE_ENUM_CLASS_FORWARDER(NAME_)

// CMN_PP_DECLARE_ENUM_CLASS_INNER_BASE()
#define CMN_PP_DECLARE_ENUM_CLASS_INNER_BASE(NAME_, BASE_, GROUP_SEQ_, ...)                 \
    CMN_PP_DECLARE_ENUM_BODY(, NAME_, class, : BASE_, GROUP_SEQ_, __VA_ARGS__)              \
    CMN_PP_BEGIN_DECLARE_ENUM_META(friend, NAME_, enum_)                                    \
        CMN_PP_DECLARE_ENUM_CLASS_GROUP(NAME_, BOOST_PP_CAT(ADD_NAME_1 GROUP_SEQ_, _END))   \
    CMN_PP_END_DECLARE_ENUM_META()                                                          \
    CMN_PP_DECLARE_ENUM_CLASS_INNER_FORWARDER(NAME_)

// CMN_PP_DECLARE_EXPORT_ENUM_CLASS_BASE()
#define CMN_PP_DECLARE_EXPORT_ENUM_CLASS_BASE(NAME_, BASE_, GROUP_SEQ_, ...)            \
    CMN_PP_DECLARE_ENUM_BODY(export, NAME_, class, : BASE_, GROUP_SEQ_, __VA_ARGS__)    \
    CMN_PP_BEGIN_DECLARE_ENUM_META(export, NAME_, enum_)                                \
    CMN_PP_DECLARE_ENUM_CLASS_GROUP(NAME_, BOOST_PP_CAT(ADD_NAME_1 GROUP_SEQ_, _END))   \
    CMN_PP_END_DECLARE_ENUM_META()                                                      \
    CMN_PP_DECLARE_ENUM_CLASS_EXPORT_FORWARDER(NAME_)

// CMN_PP_ADAPT_ENUM_CLASS()
#define CMN_PP_ADAPT_ENUM_CLASS(NAME_, GROUP_SEQ_)                                          \
    CMN_PP_BEGIN_DECLARE_ENUM_META(, NAME_, enum_)                                          \
        CMN_PP_DECLARE_ENUM_CLASS_GROUP(NAME_, BOOST_PP_CAT(ADD_NAME_1 GROUP_SEQ_, _END))   \
    CMN_PP_END_DECLARE_ENUM_META()                                                          \
    CMN_PP_DECLARE_ENUM_CLASS_FORWARDER(NAME_)

#ifdef UNITY_BUILD
#   include "io.h"
#endif