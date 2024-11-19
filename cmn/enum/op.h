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

#define CMN_ENUM_RQUIRES_CLAUSE(supported_ops)  \
    requires ((ops_v<Enum> & (supported_ops)) == (supported_ops))


#define CMN_ENUM_BINARY_OP(op, supported_ops, lhs_type, rhs_type, result_type)  \
    template <c::enum_ Enum> [[nodiscard]] constexpr auto operator op (lhs_type lhs, rhs_type rhs) noexcept -> result_type \
        CMN_ENUM_RQUIRES_CLAUSE(supported_ops)  \
    {   \
        return static_cast<result_type>(to_mask(lhs) op to_mask(rhs));  \
    } 

#define CMN_ENUM_BINARY_OP_AUTO(op, supported_ops, lhs_type, rhs_type)  \
    template <c::enum_ Enum> [[nodiscard]] constexpr auto operator op (lhs_type lhs, rhs_type rhs) noexcept    \
        CMN_ENUM_RQUIRES_CLAUSE(supported_ops)  \
    {   \
        return to_mask(lhs) op to_mask(rhs);  \
    } 

#define CMN_ENUM_UNARY_OP(op, supported_ops)  \
    template <c::enum_ Enum> [[nodiscard]] constexpr auto operator op (Enum f) noexcept -> Enum \
        CMN_ENUM_RQUIRES_CLAUSE(supported_ops)  \
    {   \
        return static_cast<Enum>(op to_mask(f));  \
    }

#define CMN_ENUM_MUTABLE_PREFIX_UNARY_OP(op, supported_ops)  \
    template <c::enum_ Enum> constexpr auto operator op (Enum f) noexcept -> Enum \
        CMN_ENUM_RQUIRES_CLAUSE(supported_ops)  \
    {   \
        return static_cast<Enum>(op to_mask(f));  \
    }

#define CMN_ENUM_MUTABLE_POSTFIX_UNARY_OP(op, supported_ops)  \
    template <c::enum_ Enum> constexpr auto operator op (Enum &f, int) noexcept -> Enum \
        CMN_ENUM_RQUIRES_CLAUSE(supported_ops)  \
    {   \
        auto const tmp = f;   \
        return f = static_cast<Enum>(op to_mask(f)), tmp;  \
    }

#define CMN_ENUM_COMPOUND_OP(op, supported_ops, lhs_type, rhs_type)  \
    template <c::enum_ Enum> constexpr auto operator BOOST_PP_CAT(op, =) (lhs_type &lhs, rhs_type rhs) noexcept -> lhs_type & \
        CMN_ENUM_RQUIRES_CLAUSE(supported_ops)  \
    {   \
        return lhs = static_cast<lhs_type>( to_mask(lhs) op to_mask(rhs));  \
    }

//-----------------------------------------------------------------------------
//
// bitwise
//
//-----------------------------------------------------------------------------
CMN_ENUM_BINARY_OP(|, op_bitwise, Enum, Enum, Enum)
CMN_ENUM_BINARY_OP(|, op_bitwise | op_interoperable, Enum, interop_type_t<Enum>, Enum)
CMN_ENUM_BINARY_OP(|, op_bitwise | op_interoperable, interop_type_t<Enum>, Enum, Enum)
CMN_ENUM_COMPOUND_OP(|, op_bitwise, Enum, Enum)
CMN_ENUM_COMPOUND_OP(|, op_bitwise | op_interoperable, Enum, interop_type_t<Enum>)

CMN_ENUM_BINARY_OP(&, op_bitwise, Enum, Enum, Enum)
CMN_ENUM_BINARY_OP(&, op_bitwise | op_interoperable, Enum, interop_type_t<Enum>, Enum)
CMN_ENUM_BINARY_OP(&, op_bitwise | op_interoperable, interop_type_t<Enum>, Enum, Enum)
CMN_ENUM_COMPOUND_OP(&, op_bitwise, Enum, Enum)
CMN_ENUM_COMPOUND_OP(&, op_bitwise | op_interoperable, Enum, interop_type_t<Enum>)

CMN_ENUM_BINARY_OP(^, op_bitwise, Enum, Enum, Enum)
CMN_ENUM_BINARY_OP(^, op_bitwise | op_interoperable, Enum, interop_type_t<Enum>, Enum)
CMN_ENUM_BINARY_OP(^, op_bitwise | op_interoperable, interop_type_t<Enum>, Enum, Enum)
CMN_ENUM_COMPOUND_OP(^, op_bitwise, Enum, Enum)
CMN_ENUM_COMPOUND_OP(^, op_bitwise | op_interoperable, Enum, interop_type_t<Enum>)

CMN_ENUM_UNARY_OP(~, op_bitwise)

//-----------------------------------------------------------------------------
//
// unit steppable
//
//-----------------------------------------------------------------------------
CMN_ENUM_MUTABLE_PREFIX_UNARY_OP(++, op_steppable)
CMN_ENUM_MUTABLE_POSTFIX_UNARY_OP(++, op_steppable)
CMN_ENUM_MUTABLE_PREFIX_UNARY_OP(--, op_steppable)
CMN_ENUM_MUTABLE_POSTFIX_UNARY_OP(--, op_steppable)

//-----------------------------------------------------------------------------
//
// arithmetic
//
//-----------------------------------------------------------------------------
CMN_ENUM_BINARY_OP(+, op_ariphmetic, Enum, Enum, Enum)
CMN_ENUM_BINARY_OP(+, op_ariphmetic | op_interoperable, Enum, interop_type_t<Enum>, Enum)
CMN_ENUM_COMPOUND_OP(+, op_ariphmetic, Enum, Enum)
CMN_ENUM_COMPOUND_OP(+, op_ariphmetic | op_interoperable, Enum, interop_type_t<Enum>)

CMN_ENUM_BINARY_OP(-, op_ariphmetic, Enum, Enum, Enum)
CMN_ENUM_BINARY_OP(-, op_ariphmetic | op_interoperable, Enum, interop_type_t<Enum>, Enum)
CMN_ENUM_COMPOUND_OP(-, op_ariphmetic, Enum, Enum)
CMN_ENUM_COMPOUND_OP(-, op_ariphmetic | op_interoperable, Enum, interop_type_t<Enum>)

//-----------------------------------------------------------------------------
//
// comparable
//
//-----------------------------------------------------------------------------
//CMN_ENUM_BINARY_OP_AUTO(<=>, op_comparable, Enum, Enum)
//CMN_ENUM_BINARY_OP_AUTO(<=>, op_comparable | op_interoperable, Enum, interop_type_t<Enum>)
//CMN_ENUM_BINARY_OP_AUTO(<=>, op_comparable | op_interoperable, interop_type_t<Enum>, Enum)
CMN_ENUM_BINARY_OP_AUTO(==, op_comparable | op_interoperable, interop_type_t<Enum>, Enum)
CMN_ENUM_BINARY_OP_AUTO(!=, op_comparable | op_interoperable, interop_type_t<Enum>, Enum)

CMN_ENUM_BINARY_OP(>, op_comparable, Enum, Enum, bool)
CMN_ENUM_BINARY_OP(>, op_comparable | op_interoperable, Enum, interop_type_t<Enum>, bool)
CMN_ENUM_BINARY_OP(>, op_comparable | op_interoperable, interop_type_t<Enum>, Enum, bool)

CMN_ENUM_BINARY_OP(>=, op_comparable, Enum, Enum, bool)
CMN_ENUM_BINARY_OP(>=, op_comparable | op_interoperable, Enum, interop_type_t<Enum>, bool)
CMN_ENUM_BINARY_OP(>=, op_comparable | op_interoperable, interop_type_t<Enum>, Enum, bool)

CMN_ENUM_BINARY_OP(<, op_comparable, Enum, Enum, bool)
CMN_ENUM_BINARY_OP(<, op_comparable | op_interoperable, Enum, interop_type_t<Enum>, bool)
CMN_ENUM_BINARY_OP(<, op_comparable | op_interoperable, interop_type_t<Enum>, Enum, bool)

CMN_ENUM_BINARY_OP(<=, op_comparable, Enum, Enum, bool)
CMN_ENUM_BINARY_OP(<=, op_comparable | op_interoperable, Enum, interop_type_t<Enum>, bool)
CMN_ENUM_BINARY_OP(<=, op_comparable | op_interoperable, interop_type_t<Enum>, Enum, bool)

//-----------------------------------------------------------------------------
//
// IO
//
//-----------------------------------------------------------------------------
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

#undef CMN_ENUM_BINARY_OP
#undef CMN_ENUM_UNARY_OP
#undef CMN_ENUM_MUTABLE_PREFIX_UNARY_OP
#undef CMN_ENUM_MUTABLE_POSTFIX_UNARY_OP
#undef CMN_ENUM_COMPOUND_OP

}

}

// inject to STL streams namespace for using ADL
namespace std
{

using cmn::enum_::op::operator <<;
using cmn::enum_::op::operator >>;

}
