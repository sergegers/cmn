
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/facilities/overload.hpp>
#include <boost/preprocessor/facilities/empty.hpp>
#include <boost/preprocessor/punctuation/comma.hpp>
#include <boost/preprocessor/punctuation/comma_if.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/seq/for_each_i.hpp>
#include <boost/preprocessor/seq/enum.hpp>
#include <boost/preprocessor/seq/variadic_seq_to_seq.hpp>
#include <boost/preprocessor/tuple/rem.hpp>

#pragma warning(disable:4003)

#define CMN_PP_DEFINE_ENUM_HEADER(class_postfix, name, base)  \
    enum class_postfix name base\
    {

#define CMN_PP_DEFINE_ENUM_FOOTER() \
    };
    

//-----------------------------------------------------------------------------
//
// CMN_PP_DEFINE_NVP(...)
//
// input (enum_member_name) or (enum_member_name, value)
// output enum_member_name or enum_member_name = value
//
//-----------------------------------------------------------------------------
#define CMN_PP_DEFINE_NVP_1(enum_member_name)              enum_member_name
#define CMN_PP_DEFINE_NVP_2(enum_member_name, value)       enum_member_name = value

#define CMN_PP_DEFINE_NVP(...) \
     BOOST_PP_CAT(BOOST_PP_OVERLOAD(CMN_PP_DEFINE_NVP_,__VA_ARGS__)(__VA_ARGS__),BOOST_PP_EMPTY())


///////////////////////////////////////////////////////////////////////////////
#define CMN_PP_MARKUP_ENUM_ITEM(r, data, i, nvp)   \
    BOOST_PP_COMMA_IF(i)    CMN_PP_DEFINE_NVP(BOOST_PP_TUPLE_REM() nvp)

#define CMN_PP_DEFINE_GROUP_BODY(group_nvp_seq)  \
    BOOST_PP_SEQ_FOR_EACH_I(CMN_PP_MARKUP_ENUM_ITEM, data, BOOST_PP_VARIADIC_SEQ_TO_SEQ(group_nvp_seq))

///////////////////////////////////////////////////////////////////////////////
#define CMN_PP_MARKUP_MASK_ITEM(r, data, nvp)   \
    BOOST_PP_COMMA() CMN_PP_DEFINE_NVP(BOOST_PP_TUPLE_REM() nvp)

#define CMN_PP_DEFINE_MASK_BODY(mask_nvp_seq)  \
    BOOST_PP_SEQ_FOR_EACH(CMN_PP_MARKUP_MASK_ITEM, data, BOOST_PP_VARIADIC_SEQ_TO_SEQ(mask_nvp_seq))

///////////////////////////////////////////////////////////////////////////////

// simply eats value arguments
#define CMN_PP_EXTRACT_GROUP_SEQ(group_nvp_seq) \
    BOOST_PP_SEQ_ENUM(group_nvp_seq)

///////////////////////////////////////////////////////////////////////////////
#define CMN_PP_ADAPT_ENUM_INFO(name, group_nvp_seq) \
    consteval auto adapt_enum_info(name)    \
    {   \
        using enum name;    \
        return ::cmn::enum_::adapt_enum_info_helper<CMN_PP_EXTRACT_GROUP_SEQ(group_nvp_seq)>();  \
    }

///////////////////////////////////////////////////////////////////////////////
#define CMN_PP_ADAPT_BITFIELD_INFO(name, group_nvp_seq) \
    consteval auto adapt_enum_info(name)    \
    {   \
        using enum name;    \
        return ::cmn::enum_::adapt_bitfield_info_helper<CMN_PP_EXTRACT_GROUP_SEQ(group_nvp_seq)>();  \
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

///////////////////////////////////////////////////////////////////////////////
#define CMN_PP_INJECT_ENUM_OPS()   \
    using ::cmn::enum_::op::operator ==; \
    using ::cmn::enum_::op::operator !=; \
    using ::cmn::enum_::op::operator &; \
    using ::cmn::enum_::op::operator &=; \
    using ::cmn::enum_::op::operator |; \
    using ::cmn::enum_::op::operator |=; \
    using ::cmn::enum_::op::operator ^; \
    using ::cmn::enum_::op::operator ^=; \
    using ::cmn::enum_::op::operator ~; \
    using ::cmn::enum_::op::operator ++; \
    using ::cmn::enum_::op::operator --; \
    using ::cmn::enum_::op::operator +; \
    using ::cmn::enum_::op::operator -; \
    using ::cmn::enum_::op::operator <<; \
    using ::cmn::enum_::op::operator >>;

