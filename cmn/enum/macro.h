// ReSharper disable CppClangTidyCppcoreguidelinesMacroUsage
#pragma once

#include <utility>
#include <tuple>
#include <array>
#include <string_view>
#include <type_traits>

// ReSharper disable CppUnusedIncludeDirective
// boost.preprocessor
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/facilities/overload.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <boost/preprocessor/wstringize.hpp>
#include <boost/preprocessor/punctuation/comma.hpp>
#include <boost/preprocessor/punctuation/comma_if.hpp>
#include <boost/preprocessor/seq/size.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/seq/for_each_i.hpp>
// ReSharper restore CppUnusedIncludeDirective

#include "op.h"

////////////////////////////////////////////////////////////////////////////////
// 
// common enum macros
// 
////////////////////////////////////////////////////////////////////////////////

// header, footer
#define CMN_PP_DECLARE_ENUM_HEADER(EXPORT_, NAME_, CLASS_, BASE_)               \
    EXPORT_ enum CLASS_ NAME_ BASE_                                             \
    {

#define CMN_PP_DECLARE_ENUM_FOOTER()                                            \
    };

//-----------------------------------------------------------------------------
// body
#define CMN_PP_DECLARE_ENUM_BODY(EXPORT_, NAME_, CLASS_, BASE_, GROUP_SEQ_, ...)    \
    CMN_PP_DECLARE_ENUM_HEADER(EXPORT_, NAME_, CLASS_, BASE_)                       \
        DECLARE_ENUM_GROUP(BOOST_PP_CAT(ADD_NAME_VALUE_PAIR_1 GROUP_SEQ_, _END))    \
        DECLARE_MASK_GROUP(BOOST_PP_CAT(ADD_NAME_VALUE_PAIR_1 __VA_ARGS__, _END))   \
    CMN_PP_DECLARE_ENUM_FOOTER()

//-----------------------------------------------------------------------------
// input (NAME_) or (NAME_, VAL_)
// output NAME_ or NAME_ = VAL_
#define ADD_NAME_VALUE_PAIR_1(...) (DECLARE_NAME_VALUE_PAIR(__VA_ARGS__)) ADD_NAME_VALUE_PAIR_2 
#define ADD_NAME_VALUE_PAIR_2(...) (DECLARE_NAME_VALUE_PAIR(__VA_ARGS__)) ADD_NAME_VALUE_PAIR_1 
#define ADD_NAME_VALUE_PAIR_1_END 
#define ADD_NAME_VALUE_PAIR_2_END 

#define DECLARE_NAME_VALUE_PAIR_1(NAME_)            NAME_
#define DECLARE_NAME_VALUE_PAIR_2(NAME_, VALUE_)    NAME_ = VALUE_

#define DECLARE_NAME_VALUE_PAIR(...) \
    BOOST_PP_CAT(BOOST_PP_OVERLOAD(DECLARE_NAME_VALUE_PAIR_, __VA_ARGS__)(__VA_ARGS__), BOOST_PP_EMPTY())
//-----------------------------------------------------------------------------

#define MAKE_ENUM_ELEM_DEF(r, data, I_, ITEM_)    \
    BOOST_PP_COMMA_IF(I_) ITEM_

#define DECLARE_ENUM_GROUP(GROUP_SEQ_)  \
    BOOST_PP_SEQ_FOR_EACH_I(MAKE_ENUM_ELEM_DEF, data, GROUP_SEQ_)

//-----------------------------------------------------------------------------

#define MAKE_ENUM_MASK_DEF(r, data, ITEM_)    \
    BOOST_PP_COMMA() ITEM_

#define DECLARE_MASK_GROUP(GROUP_SEQ_)  \
    BOOST_PP_SEQ_FOR_EACH(MAKE_ENUM_MASK_DEF, data, GROUP_SEQ_)

////////////////////////////////////////////////////////////////////////////////
// input (NAME_) or (NAME_, VAL_)
// output NAME_

#define ADD_NAME_1(...) (DECLARE_NAME(__VA_ARGS__)) ADD_NAME_2 
#define ADD_NAME_2(...) (DECLARE_NAME(__VA_ARGS__)) ADD_NAME_1 
#define ADD_NAME_1_END 
#define ADD_NAME_2_END 

#define DECLARE_NAME_1(NAME_)               NAME_
#define DECLARE_NAME_2(NAME_, VALUE_)       NAME_

#define DECLARE_NAME(...) \
    BOOST_PP_OVERLOAD(DECLARE_NAME_, __VA_ARGS__)(__VA_ARGS__)

////////////////////////////////////////////////////////////////////////////////
#define CMN_PP_BEGIN_DECLARE_ENUM_META(PFX_, NAME_, kind)                                                   \
    PFX_ constexpr auto get_kind(NAME_) -> ::cmn::enum_::kkind_t< ::cmn::enum_::kind_t::kind>               \
    { return ::cmn::enum_::kkind_t< ::cmn::enum_::kind_t::kind>{}; }                                        \
                                                                                                            \
    PFX_ constexpr auto get_name(NAME_) -> ::std::string_view                                               \
    { using namespace ::std::string_view_literals; return BOOST_PP_CAT(BOOST_PP_STRINGIZE(NAME_), sv); }    \
                                                                                                            \
    PFX_ constexpr auto get_wname(NAME_) -> ::std::wstring_view                                             \
    { using namespace ::std::string_view_literals; return BOOST_PP_CAT(BOOST_PP_WSTRINGIZE(NAME_), sv); }   \
                                                                                                            \
    PFX_ constexpr auto get_groups(NAME_)                                                                   \
    {                                                                                                       \
        using namespace ::std::string_view_literals;                                                        \
        using utils_type = ::cmn::enum_::utils<NAME_>;                                                      \
        auto groups = ::std::tuple                                                                          \
        {

#define CMN_PP_END_DECLARE_ENUM_META()                                                                      \
        };                                                                                                  \
        utils_type::sort_groups(groups);                                                                    \
        return groups;                                                                                      \
    }

////////////////////////////////////////////////////////////////////////////////

#define TO_MASK(NAME_)             ::cmn::enum_::op::to_mask_t<NAME_>
#define ARG_CAST(NAME_, e)          static_cast<NAME_>(e)
#define FWD_ARG_1(NAME_, e)         ( ::cmn::enum_::op::fwd<NAME_>(e))
#define FWD_ARG_2(LNAME_, lhs, RNAME_, rhs)  \
    ( ::cmn::enum_::op::fwd<LNAME_>(lhs) BOOST_PP_COMMA() ::cmn::enum_::op::fwd<RNAME_>(rhs))

#define CMN_PP_ENUM_CONSTEXPR_BINARY_INTEROPERABLE_COMMUTATIVE_OP_PFX_FWD(PFX_, NAME_, op_)                 \
    PFX_ constexpr auto operator op_ (NAME_ __lhs, TO_MASK(NAME_) __rhs) noexcept ->                        \
        decltype(::cmn::enum_::op::operator op_ FWD_ARG_2(NAME_, __lhs, NAME_, ARG_CAST(NAME_, __rhs)))     \
    { return ::cmn::enum_::op::operator op_ FWD_ARG_2(NAME_, __lhs, NAME_, ARG_CAST(NAME_, __rhs)); }       \
                                                                                                            \
    PFX_ constexpr auto operator op_ (TO_MASK(NAME_) __lhs, NAME_ __rhs) noexcept ->                        \
        decltype( ::cmn::enum_::op::operator op_ FWD_ARG_2(NAME_, ARG_CAST(NAME_, __lhs), NAME_, __rhs))    \
    { return ::cmn::enum_::op::operator op_ FWD_ARG_2(NAME_, ARG_CAST(NAME_, __lhs), NAME_, __rhs); }

#define CMN_PP_ENUM_CONSTEXPR_BINARY_COMMUTATIVE_OP_PFX_FWD(PFX_, NAME_, op_)                               \
    PFX_ constexpr auto operator op_ (NAME_ __lhs, NAME_ __rhs) noexcept ->                                 \
        decltype(::cmn::enum_::op::operator op_ FWD_ARG_2(NAME_, __lhs, NAME_, __rhs))                      \
    { return ::cmn::enum_::op::operator op_ FWD_ARG_2(NAME_, __lhs, NAME_, __rhs); }

#define CMN_PP_ENUM_CLASS_CONSTEXPR_BINARY_COMMUTATIVE_OP_PFX_FWD(PFX_, NAME_, op_)                         \
    CMN_PP_ENUM_CONSTEXPR_BINARY_COMMUTATIVE_OP_PFX_FWD(PFX_, NAME_, op_)                                   \
    CMN_PP_ENUM_CONSTEXPR_BINARY_INTEROPERABLE_COMMUTATIVE_OP_PFX_FWD(PFX_, NAME_, op_)

#define CMN_PP_ENUM_CONSTEXPR_UNARY_OP_PFX_FWD(PFX_, NAME_, op_)                                            \
    PFX_ constexpr auto operator op_ (NAME_ __e) noexcept ->                                                \
        decltype(::cmn::enum_::op::operator op_ FWD_ARG_1(NAME_, __e))                                      \
    { return ::cmn::enum_::op::operator op_ FWD_ARG_1(NAME_, __e); }

#define CMN_PP_ENUM_MUTABLE_PREFIX_UNARY_OP_PFX_FWD(PFX_, NAME_, op_)                                       \
    PFX_ inline auto operator op_ (NAME_ &__e) noexcept ->                                                  \
        decltype(::cmn::enum_::op::operator op_ FWD_ARG_1(NAME_ &, __e))                                    \
    { return ::cmn::enum_::op::operator op_ FWD_ARG_1(NAME_ &, __e); }

#define CMN_PP_ENUM_MUTABLE_POSTFIX_UNARY_OP_PFX_FWD(PFX_, NAME_, op_)                                      \
    PFX_ inline auto operator op_ (NAME_ &__e, int __i) noexcept ->                                         \
        decltype(::cmn::enum_::op::operator op_( ::cmn::enum_::op::fwd<NAME_ &>(__e) BOOST_PP_COMMA() __i)) \
    {                                                                                                       \
        return ::cmn::enum_::op::operator op_ (                                                             \
            ::cmn::enum_::op::fwd<NAME_ &>(__e) BOOST_PP_COMMA() __i);                                      \
    }

#define CMN_PP_ENUM_OUTPUT_OP_PFX_FWD(PFX_, NAME_)                                                          \
    template <typename Char, typename CharTraits>                                                           \
    PFX_ auto operator << ( ::std::basic_ostream<Char, CharTraits> &__ostr, NAME_ __e) ->                   \
        ::std::basic_ostream<Char, CharTraits> &                                                            \
    {                                                                                                       \
        return ::cmn::enum_::io::printer                                                                    \
        {                                                                                                   \
            __e BOOST_PP_COMMA()                                                                            \
            get_kind(__e)                                                                                   \
        }                                                                                                   \
        .print(__ostr);                                                                                     \
    }

#define CMN_PP_ENUM_INPUT_OP_PFX_FWD(PFX_, NAME_)                                                           \
    template <typename Char, typename CharTraits>                                                           \
    PFX_ auto operator >> ( ::std::basic_istream<Char, CharTraits> &__istr, NAME_ &__e) ->                  \
        ::std::basic_istream<Char, CharTraits> &                                                            \
    {                                                                                                       \
        return ::cmn::enum_::io::reader                                                                     \
        {                                                                                                   \
            __e BOOST_PP_COMMA()                                                                            \
            get_kind(__e)                                                                                   \
        }                                                                                                   \
        .read(__istr);                                                                                      \
    }

#define CMN_PP_ENUM_OUTPUT_OP_PPFX_FWD(PFX_, NAME_)                                                         \
    PFX_ template <typename Char, typename CharTraits>                                                      \
    auto operator << ( ::std::basic_ostream<Char, CharTraits> &__ostr, NAME_ __e)                           \
        ->::std::basic_ostream<Char, CharTraits> &                                                          \
    {                                                                                                       \
        return ::cmn::enum_::io::printer                                                                    \
        {                                                                                                   \
            __e BOOST_PP_COMMA()                                                                            \
            get_kind(__e)                                                                                   \
        }                                                                                                   \
        .print(__ostr);                                                                                     \
    }

#define CMN_PP_ENUM_INPUT_OP_PPFX_FWD(PFX_, NAME_)                                                          \
    PFX_ template <typename Char, typename CharTraits>                                                      \
    auto operator >> (::std::basic_istream<Char, CharTraits> &__istr, NAME_ &__e)                           \
        ->::std::basic_istream<Char, CharTraits> &                                                          \
    {                                                                                                       \
        return ::cmn::enum_::io::reader                                                                     \
        {                                                                                                   \
            __e BOOST_PP_COMMA()                                                                            \
            get_kind(__e)                                                                                   \
        }                                                                                                   \
        .read(__istr);                                                                                      \
    }

#define CMN_PP_ENUM_COMPOUND_OP_PFX_FWD(PFX_, NAME_, op_)  \
    PFX_ inline auto operator op_ (NAME_ &__lhs, NAME_ __rhs) noexcept -> \
        decltype(::cmn::enum_::op::operator op_ FWD_ARG_2(NAME_ &, __lhs, NAME_, __rhs))    \
    { return ::cmn::enum_::op::operator op_ FWD_ARG_2(NAME_ &, __lhs, NAME_, __rhs); }

#define CMN_PP_ENUM_CLASS_COMPOUND_OP_PFX_FWD(PFX_, NAME_, op_)  \
    CMN_PP_ENUM_COMPOUND_OP_PFX_FWD(PFX_, NAME_, op_)   \
    PFX_ inline auto operator op_ (NAME_ &__lhs, TO_MASK(NAME_) __rhs) noexcept -> \
        decltype(::cmn::enum_::op::operator op_ FWD_ARG_2(NAME_ &, __lhs, NAME_, ARG_CAST(NAME_, __rhs)))   \
    { return ::cmn::enum_::op::operator op_ FWD_ARG_2(NAME_ &, __lhs, NAME_, ARG_CAST(NAME_, __rhs)); }

#define CMN_PP_ENUM_DECLARE_UNIT_STEPPABLE_PFX_FWD(PFX_, NAME_)  \
    CMN_PP_ENUM_MUTABLE_PREFIX_UNARY_OP_PFX_FWD(PFX_, NAME_, ++) \
    CMN_PP_ENUM_MUTABLE_POSTFIX_UNARY_OP_PFX_FWD(PFX_, NAME_, ++)   \
    CMN_PP_ENUM_MUTABLE_PREFIX_UNARY_OP_PFX_FWD(PFX_, NAME_, --) \
    CMN_PP_ENUM_MUTABLE_POSTFIX_UNARY_OP_PFX_FWD(PFX_, NAME_, --)

#define CMN_PP_ENUM_DECLARE_BITWISE_PFX_FWD(PFX_, NAME_)  \
    CMN_PP_ENUM_CONSTEXPR_BINARY_COMMUTATIVE_OP_PFX_FWD(PFX_, NAME_, |)      \
    CMN_PP_ENUM_CONSTEXPR_BINARY_COMMUTATIVE_OP_PFX_FWD(PFX_, NAME_, &)      \
    CMN_PP_ENUM_CONSTEXPR_BINARY_COMMUTATIVE_OP_PFX_FWD(PFX_, NAME_, ^)      \
    CMN_PP_ENUM_CONSTEXPR_UNARY_OP_PFX_FWD(PFX_, NAME_, ~)  \
    CMN_PP_ENUM_COMPOUND_OP_PFX_FWD(PFX_, NAME_, |=)    \
    CMN_PP_ENUM_COMPOUND_OP_PFX_FWD(PFX_, NAME_, &=)    \
    CMN_PP_ENUM_COMPOUND_OP_PFX_FWD(PFX_, NAME_, ^=)

#define CMN_PP_ENUM_CLASS_DECLARE_BITWISE_PFX_FWD(PFX_, NAME_)  \
    CMN_PP_ENUM_CLASS_CONSTEXPR_BINARY_COMMUTATIVE_OP_PFX_FWD(PFX_, NAME_, |)      \
    CMN_PP_ENUM_CLASS_CONSTEXPR_BINARY_COMMUTATIVE_OP_PFX_FWD(PFX_, NAME_, &)      \
    CMN_PP_ENUM_CLASS_CONSTEXPR_BINARY_COMMUTATIVE_OP_PFX_FWD(PFX_, NAME_, ^)      \
    CMN_PP_ENUM_CONSTEXPR_UNARY_OP_PFX_FWD(PFX_, NAME_, ~)  \
    CMN_PP_ENUM_CLASS_COMPOUND_OP_PFX_FWD(PFX_, NAME_, |=)    \
    CMN_PP_ENUM_CLASS_COMPOUND_OP_PFX_FWD(PFX_, NAME_, &=)    \
    CMN_PP_ENUM_CLASS_COMPOUND_OP_PFX_FWD(PFX_, NAME_, ^=)

#define CMN_PP_ENUM_DECLARE_EQUALITY_COMPARABLE_PFX_FWD(PFX_, NAME_)  \
    CMN_PP_ENUM_CONSTEXPR_BINARY_INTEROPERABLE_COMMUTATIVE_OP_PFX_FWD(PFX_, NAME_, ==)     \
    CMN_PP_ENUM_CONSTEXPR_BINARY_INTEROPERABLE_COMMUTATIVE_OP_PFX_FWD(PFX_, NAME_, !=)

#define CMN_PP_ENUM_DECLARE_COMPARABLE_PFX_FWD(PFX_, NAME_)  \
    CMN_PP_ENUM_CONSTEXPR_BINARY_INTEROPERABLE_COMMUTATIVE_OP_PFX_FWD(PFX_, NAME_, >)      \
    CMN_PP_ENUM_CONSTEXPR_BINARY_INTEROPERABLE_COMMUTATIVE_OP_PFX_FWD(PFX_, NAME_, >=)     \
    CMN_PP_ENUM_CONSTEXPR_BINARY_INTEROPERABLE_COMMUTATIVE_OP_PFX_FWD(PFX_, NAME_, <)      \
    CMN_PP_ENUM_CONSTEXPR_BINARY_INTEROPERABLE_COMMUTATIVE_OP_PFX_FWD(PFX_, NAME_, <=)      

#define CMN_PP_ENUM_DECLARE_TOTALLY_ORDERED_PFX_FWD(PFX_, NAME_)  \
    CMN_PP_ENUM_DECLARE_EQUALITY_COMPARABLE_PFX_FWD(PFX_, NAME_)  \
    CMN_PP_ENUM_DECLARE_COMPARABLE_PFX_FWD(PFX_, NAME_)

#define CMN_PP_ENUM_DECLARE_ARIPHMETIC_PFX_FWD(PFX_, NAME_)  \
    CMN_PP_ENUM_CONSTEXPR_BINARY_COMMUTATIVE_OP_PFX_FWD(PFX_, NAME_, +)      \
    CMN_PP_ENUM_CONSTEXPR_BINARY_COMMUTATIVE_OP_PFX_FWD(PFX_, NAME_, -)      \

#define CMN_PP_ENUM_CLASS_DECLARE_ARIPHMETIC_PFX_FWD(PFX_, NAME_)  \
    CMN_PP_ENUM_CLASS_CONSTEXPR_BINARY_COMMUTATIVE_OP_PFX_FWD(PFX_, NAME_, +)      \
    CMN_PP_ENUM_CLASS_CONSTEXPR_BINARY_COMMUTATIVE_OP_PFX_FWD(PFX_, NAME_, -)      \

#define CMN_PP_ENUM_DECLARE_IO_PFX_FWD(PFX_, NAME_)    \
    CMN_PP_ENUM_OUTPUT_OP_PFX_FWD(PFX_, NAME_)  \
    CMN_PP_ENUM_INPUT_OP_PFX_FWD(PFX_, NAME_)

#define CMN_PP_ENUM_DECLARE_IO_PPFX_FWD(PFX_, NAME_) \
    CMN_PP_ENUM_OUTPUT_OP_PPFX_FWD(PFX_, NAME_)      \
    CMN_PP_ENUM_INPUT_OP_PPFX_FWD(PFX_, NAME_)

////////////////////////////////////////////////////////////////////////////////
#define CMN_PP_ENUM_DECLARE_UNIT_STEPPABLE(NAME_)  \
    CMN_PP_ENUM_DECLARE_UNIT_STEPPABLE_PFX_FWD(, NAME_)

#define CMN_PP_ENUM_DECLARE_BITWISE(NAME_)  \
    CMN_PP_ENUM_DECLARE_BITWISE_PFX_FWD(, NAME_)

#define CMN_PP_ENUM_CLASS_DECLARE_BITWISE(NAME_)  \
    CMN_PP_ENUM_CLASS_DECLARE_BITWISE_PFX_FWD(, NAME_)

#define CMN_PP_ENUM_DECLARE_EQUALITY_COMPARABLE(NAME_)  \
    CMN_PP_ENUM_DECLARE_EQUALITY_COMPARABLE_PFX_FWD(, NAME_)

#define CMN_PP_ENUM_DECLARE_COMPARABLE(NAME_)  \
    CMN_PP_ENUM_DECLARE_COMPARABLE_PFX_FWD(, NAME_)

#define CMN_PP_ENUM_DECLARE_TOTALLY_ORDERED(NAME_)  \
    CMN_PP_ENUM_DECLARE_TOTALLY_ORDERED_PFX_FWD(, NAME_)

#define CMN_PP_ENUM_DECLARE_ARIPHMETIC(NAME_)  \
    CMN_PP_ENUM_DECLARE_ARIPHMETIC_PFX_FWD(, NAME_)

#define CMN_PP_ENUM_CLASS_DECLARE_ARIPHMETIC(NAME_)  \
    CMN_PP_ENUM_CLASS_DECLARE_ARIPHMETIC_PFX_FWD(, NAME_)

#define CMN_PP_ENUM_DECLARE_IO(NAME_)    \
    CMN_PP_ENUM_DECLARE_IO_PFX_FWD(, NAME_)    \

////////////////////////////////////////////////////////////////////////////////
#define CMN_PP_ENUM_INNER_DECLARE_UNIT_STEPPABLE(NAME_)  \
    CMN_PP_ENUM_DECLARE_UNIT_STEPPABLE_PFX_FWD(friend, NAME_)

#define CMN_PP_ENUM_INNER_DECLARE_BITWISE(NAME_)  \
    CMN_PP_ENUM_DECLARE_BITWISE_PFX_FWD(friend, NAME_)

#define CMN_PP_ENUM_CLASS_INNER_DECLARE_BITWISE(NAME_)  \
    CMN_PP_ENUM_CLASS_DECLARE_BITWISE_PFX_FWD(friend, NAME_)

#define CMN_PP_ENUM_INNER_DECLARE_EQUALITY_COMPARABLE(NAME_)  \
    CMN_PP_ENUM_DECLARE_EQUALITY_COMPARABLE_PFX_FWD(friend, NAME_)

#define CMN_PP_ENUM_INNER_DECLARE_COMPARABLE(NAME_)  \
    CMN_PP_ENUM_DECLARE_COMPARABLE_PFX_FWD(friend, NAME_)

#define CMN_PP_ENUM_INNER_DECLARE_TOTALLY_ORDERED(NAME_)  \
    CMN_PP_ENUM_DECLARE_TOTALLY_ORDERED_PFX_FWD(friend, NAME_)

#define CMN_PP_ENUM_INNER_DECLARE_ARIPHMETIC(NAME_)  \
    CMN_PP_ENUM_DECLARE_ARIPHMETIC_PFX_FWD(friend, NAME_)

#define CMN_PP_ENUM_CLASS_INNER_DECLARE_ARIPHMETIC(NAME_)  \
    CMN_PP_ENUM_CLASS_DECLARE_ARIPHMETIC_PFX_FWD(friend, NAME_)

#define CMN_PP_ENUM_INNER_DECLARE_IO(NAME_)    \
    CMN_PP_ENUM_DECLARE_IO_PFX_FWD(friend, NAME_)    \

////////////////////////////////////////////////////////////////////////////////
#define CMN_PP_ENUM_EXPORT_DECLARE_UNIT_STEPPABLE(NAME_)    \
    CMN_PP_ENUM_DECLARE_UNIT_STEPPABLE_PFX_FWD(export, NAME_)

#define CMN_PP_ENUM_EXPORT_DECLARE_BITWISE(NAME_)   \
    CMN_PP_ENUM_DECLARE_BITWISE_PFX_FWD(export, NAME_)

#define CMN_PP_ENUM_CLASS_EXPORT_DECLARE_BITWISE(NAME_)     \
    CMN_PP_ENUM_CLASS_DECLARE_BITWISE_PFX_FWD(export, NAME_)

#define CMN_PP_ENUM_EXPORT_DECLARE_EQUALITY_COMPARABLE(NAME_)   \
    CMN_PP_ENUM_DECLARE_EQUALITY_COMPARABLE_PFX_FWD(export, NAME_)

#define CMN_PP_ENUM_EXPORT_DECLARE_COMPARABLE(NAME_)    \
    CMN_PP_ENUM_DECLARE_COMPARABLE_PFX_FWD(export, NAME_)

#define CMN_PP_ENUM_EXPORT_DECLARE_TOTALLY_ORDERED(NAME_)   \
    CMN_PP_ENUM_DECLARE_TOTALLY_ORDERED_PFX_FWD(export, NAME_)

#define CMN_PP_ENUM_EXPORT_DECLARE_ARIPHMETIC(NAME_)    \
    CMN_PP_ENUM_DECLARE_ARIPHMETIC_PFX_FWD(export, NAME_)

#define CMN_PP_ENUM_CLASS_EXPORT_DECLARE_ARIPHMETIC(NAME_)  \
    CMN_PP_ENUM_CLASS_DECLARE_ARIPHMETIC_PFX_FWD(export, NAME_)

#define CMN_PP_ENUM_EXPORT_DECLARE_IO(NAME_)    \
    CMN_PP_ENUM_DECLARE_IO_PPFX_FWD(export, NAME_)

////////////////////////////////////////////////////////////////////////////////
#define CMN_PP_QNAME(CLASS_, NAME_) BOOST_PP_CAT(CLASS_, BOOST_PP_CAT(::, NAME_))

#define CMN_PP_DEFINE_ENUM_CONST(ITEM_)                 (ITEM_##_ BOOST_PP_COMMA() ITEM_)
#define CMN_PP_DEFINE_ENUM_CONST_BASE(ITEM_, BASE_)     (ITEM_##_ BOOST_PP_COMMA() static_cast<BASE_>(ITEM_))
#define CMN_PP_ASSIGN_ENUM_CONST(ITEM_)                 ITEM_##_ = ITEM_