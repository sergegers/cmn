// ReSharper disable CppClangTidyCppcoreguidelinesMacroUsage
#pragma once

#include <iosfwd>

#include <boost/preprocessor/cat.hpp>

#include <cmn/meta/concepts.h>
#include <cmn/enum/traits.h>

namespace cmn::enum_
{

namespace io
{

template<c::enum_ Enum, kind_t Kind_>
struct reader
{
    Enum                                    &m_val;
    [[no_unique_address]] kkind_t<Kind_>    m_kind;

    template <typename Char, typename CharTraits>
    auto read(std::basic_istream<Char, CharTraits> &istr) -> decltype(istr)
    {
        static_assert
        (
            sizeof(Char) == 0,
            "If you want to output enum, include header <cmn/enum/io.h>"
        );
        return istr;
    }
};

template<c::enum_ Enum, kind_t Kind_>
struct printer
{
    Enum                                    m_val;
    [[no_unique_address]] kkind_t<Kind_>    m_kind;

    template <typename Char, typename CharTraits>
    auto print(std::basic_ostream<Char, CharTraits> &ostr) -> decltype(ostr)
    {
        static_assert
        (
            sizeof(Char) == 0,
            "If you want to output enum, include header <cmn/enum/io.h>"
        );
        return ostr;
    }
};

}

namespace op
{

//-----------------------------------------------------------------------------
//
// NOTES:
// 1. no need to full qualify names because the macros below are used locally only
// 2. at least one of the operand types is named Enum
//
//-----------------------------------------------------------------------------

#define CMN_PP_ENUM_RQUIRES_CLAUSE(OPS_)  \
    requires ((ops_v<Enum> & (OPS_)) == (OPS_))


#define CMN_PP_ENUM_CONSTEXPR_BINARY_OP(OP_, OPS_, LHS_, RHS_, RES_)  \
    template <c::enum_ Enum> [[nodiscard]] constexpr auto operator OP_ (LHS_ lhs, RHS_ rhs) noexcept -> RES_ \
        CMN_PP_ENUM_RQUIRES_CLAUSE(OPS_)  \
    {   \
        return static_cast<RES_>(to_mask(lhs) OP_ to_mask(rhs));  \
    } 

#define CMN_PP_ENUM_CONSTEXPR_BINARY_OP_AUTO(OP_, OPS_, LHS_, RHS_)  \
    template <c::enum_ Enum> [[nodiscard]] constexpr auto operator OP_ (LHS_ lhs, RHS_ rhs) noexcept    \
        CMN_PP_ENUM_RQUIRES_CLAUSE(OPS_)  \
    {   \
        return to_mask(lhs) OP_ to_mask(rhs);  \
    } 

#define CMN_PP_ENUM_CONSTEXPR_UNARY_OP(OP_, OPS_)  \
    template <c::enum_ Enum> [[nodiscard]] constexpr auto operator OP_ (Enum f) noexcept -> Enum \
        CMN_PP_ENUM_RQUIRES_CLAUSE(OPS_)  \
    {   \
        return static_cast<Enum>(OP_ to_mask(f));  \
    }

#define CMN_PP_ENUM_MUTABLE_PREFIX_UNARY_OP(OP_, OPS_)  \
    template <c::enum_ Enum> constexpr auto operator OP_ (Enum f) noexcept -> Enum \
        CMN_PP_ENUM_RQUIRES_CLAUSE(OPS_)  \
    {   \
        return static_cast<Enum>(OP_ to_mask(f));  \
    }

#define CMN_PP_ENUM_MUTABLE_POSTFIX_UNARY_OP(OP_, OPS_)  \
    template <c::enum_ Enum> constexpr auto operator OP_ (Enum &f, int) noexcept -> Enum \
        CMN_PP_ENUM_RQUIRES_CLAUSE(OPS_)  \
    {   \
        auto const tmp = f;   \
        return f = static_cast<Enum>(OP_ to_mask(f)), tmp;  \
    }

#define CMN_PP_ENUM_COMPOUND_OP(OP_, OPS_, LHS_, RHS_)  \
    template <c::enum_ Enum> constexpr auto operator BOOST_PP_CAT(OP_, =) (LHS_ &lhs, RHS_ rhs) noexcept -> LHS_ & \
        CMN_PP_ENUM_RQUIRES_CLAUSE(OPS_)  \
    {   \
        return lhs = static_cast<LHS_>(to_mask(lhs) OP_ to_mask(rhs));  \
    }

// bitwise
CMN_PP_ENUM_CONSTEXPR_BINARY_OP(|, op_bitwise, Enum, Enum, Enum)
CMN_PP_ENUM_CONSTEXPR_BINARY_OP(|, op_bitwise | op_interoperable, Enum, interop_type_t<Enum>, Enum)
CMN_PP_ENUM_CONSTEXPR_BINARY_OP(|, op_bitwise | op_interoperable, interop_type_t<Enum>, Enum, Enum)
CMN_PP_ENUM_CONSTEXPR_BINARY_OP(&, op_bitwise, Enum, Enum, Enum)
CMN_PP_ENUM_CONSTEXPR_BINARY_OP(&, op_bitwise | op_interoperable, Enum, interop_type_t<Enum>, Enum)
CMN_PP_ENUM_CONSTEXPR_BINARY_OP(&, op_bitwise | op_interoperable, interop_type_t<Enum>, Enum, Enum)
CMN_PP_ENUM_CONSTEXPR_BINARY_OP(^, op_bitwise, Enum, Enum, Enum)
CMN_PP_ENUM_CONSTEXPR_BINARY_OP(^, op_bitwise | op_interoperable, Enum, interop_type_t<Enum>, Enum)
CMN_PP_ENUM_CONSTEXPR_BINARY_OP(^, op_bitwise | op_interoperable, interop_type_t<Enum>, Enum, Enum)
CMN_PP_ENUM_CONSTEXPR_UNARY_OP(~, op_bitwise)
CMN_PP_ENUM_COMPOUND_OP(|, op_bitwise, Enum, Enum)
CMN_PP_ENUM_COMPOUND_OP(&, op_bitwise, Enum, Enum)
CMN_PP_ENUM_COMPOUND_OP(^, op_bitwise, Enum, Enum)

// unit steppable
CMN_PP_ENUM_MUTABLE_PREFIX_UNARY_OP(++, op_steppable)
CMN_PP_ENUM_MUTABLE_POSTFIX_UNARY_OP(++, op_steppable)
CMN_PP_ENUM_MUTABLE_PREFIX_UNARY_OP(--, op_steppable)
CMN_PP_ENUM_MUTABLE_POSTFIX_UNARY_OP(--, op_steppable)

// arithmetic
CMN_PP_ENUM_CONSTEXPR_BINARY_OP(+, op_ariphmetic, Enum, Enum, Enum)
CMN_PP_ENUM_CONSTEXPR_BINARY_OP(-, op_ariphmetic, Enum, Enum, Enum)

// comparable
//CMN_PP_ENUM_CONSTEXPR_BINARY_OP_AUTO(<=>, op_comparable, Enum, Enum)
//CMN_PP_ENUM_CONSTEXPR_BINARY_OP_AUTO(<=>, op_comparable | op_interoperable, Enum, interop_type_t<Enum>)
//CMN_PP_ENUM_CONSTEXPR_BINARY_OP_AUTO(<=>, op_comparable | op_interoperable, interop_type_t<Enum>, Enum)
CMN_PP_ENUM_CONSTEXPR_BINARY_OP_AUTO(==, op_comparable | op_interoperable, interop_type_t<Enum>, Enum)
CMN_PP_ENUM_CONSTEXPR_BINARY_OP_AUTO(!=, op_comparable | op_interoperable, interop_type_t<Enum>, Enum)
//CMN_PP_ENUM_CONSTEXPR_BINARY_OP(>, op_comparable, Enum, Enum, bool)
//CMN_PP_ENUM_CONSTEXPR_BINARY_OP(>=, op_comparable, Enum, Enum, bool)
//CMN_PP_ENUM_CONSTEXPR_BINARY_OP(<, op_comparable, Enum, Enum, bool)
//CMN_PP_ENUM_CONSTEXPR_BINARY_OP(<=, op_comparable, Enum, Enum, bool)

template <c::enum_ Enum, typename Char, typename CharTaits>
auto operator << (std::basic_ostream<Char, CharTaits> &ostr, Enum en) -> std::basic_ostream<Char, CharTaits> &
    requires ((ops_v<Enum> & op_io) == op_io)
{
    return io::printer{ en, kkind_t<kind_v<Enum>>{} }.print(ostr);                                                                                     \
} 

template <c::enum_ Enum, typename Char, typename CharTaits>
auto operator >> (std::basic_istream<Char, CharTaits> &istr, Enum &en) -> std::basic_istream<Char, CharTaits> &
    requires ((ops_v<Enum> & op_io) == op_io)
{
    return io::reader{ en, kkind_t<kind_v<Enum>>{} }.read(istr);
} 

#undef CMN_PP_ENUM_CONSTEXPR_BINARY_OP
#undef CMN_PP_ENUM_CONSTEXPR_UNARY_OP
#undef CMN_PP_ENUM_MUTABLE_PREFIX_UNARY_OP
#undef CMN_PP_ENUM_MUTABLE_POSTFIX_UNARY_OP
#undef CMN_PP_ENUM_COMPOUND_OP

}

}

// FIX: boost::lexical_cast<>
namespace std
{

using cmn::enum_::op::operator <<;
using cmn::enum_::op::operator >>;

}
