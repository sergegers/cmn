// ReSharper disable CppClangTidyCppcoreguidelinesMacroUsage
#pragma once

#include <iosfwd>

#include <boost/preprocessor/cat.hpp>

#include <cmn/meta/concepts.h>
#include <cmn/meta/type_traits.h>   // int_<>
#include <cmn/util/util.h>          // lazy_to_interop()

#include <cmn/enum/traits.h>        // ops_v<>

namespace cmn::enum_
{

namespace io
{

template<c::adapted_enum E, kind_t Kind_>
struct reader
{
    E                                   &m_val;
    [[no_unique_address]] int_<Kind_>   m_kind;

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

template<c::adapted_enum E, kind_t Kind_>
struct printer
{
    E                                    m_val;
    [[no_unique_address]] int_<Kind_>    m_kind;

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
// 2. at least one of the operand types is named E
//
//-----------------------------------------------------------------------------

#define CMN_ENUM_RQUIRES_CLAUSE(supported_ops)  \
    requires ((ops_v<E> & (supported_ops)) == (supported_ops))

//-----------------------------------------------------------------------------
#define CMN_ENUM_BINARY_OP(op, supported_ops, lhs_type, rhs_type, result_type)  \
    template <c::adapted_enum E> [[nodiscard]] constexpr auto operator op (lhs_type lhs, rhs_type rhs) noexcept -> result_type \
        CMN_ENUM_RQUIRES_CLAUSE(supported_ops)  \
    {   \
        return static_cast<result_type>(lazy_to_interop(lhs) op lazy_to_interop(rhs));  \
    } 

#define CMN_ENUM_BINARY_OP_AUTO(op, supported_ops, lhs_type, rhs_type)  \
    template <c::adapted_enum E> [[nodiscard]] constexpr auto operator op (lhs_type lhs, rhs_type rhs) noexcept    \
        CMN_ENUM_RQUIRES_CLAUSE(supported_ops)  \
    {   \
        return lazy_to_interop(lhs) op lazy_to_interop(rhs);  \
    } 

#define CMN_ENUM_UNARY_OP(op, supported_ops)  \
    template <c::adapted_enum E> [[nodiscard]] constexpr auto operator op (E f) noexcept -> E \
        CMN_ENUM_RQUIRES_CLAUSE(supported_ops)  \
    {   \
        return static_cast<E>(op lazy_to_interop(f));  \
    }

#define CMN_ENUM_MUTABLE_PREFIX_UNARY_OP(op, supported_ops)  \
    template <c::adapted_enum E> constexpr auto operator op (E f) noexcept -> E \
        CMN_ENUM_RQUIRES_CLAUSE(supported_ops)  \
    {   \
        return static_cast<E>(op lazy_to_interop(f));  \
    }

#define CMN_ENUM_MUTABLE_POSTFIX_UNARY_OP(op, supported_ops)  \
    template <c::adapted_enum E> constexpr auto operator op (E &f, int) noexcept -> E \
        CMN_ENUM_RQUIRES_CLAUSE(supported_ops)  \
    {   \
        auto const tmp = f;   \
        return f = static_cast<E>(op lazy_to_interop(f)), tmp;  \
    }

#define CMN_ENUM_COMPOUND_OP(op, supported_ops, lhs_type, rhs_type)  \
    template <c::adapted_enum E> constexpr auto operator BOOST_PP_CAT(op, =) (lhs_type &lhs, rhs_type rhs) noexcept -> lhs_type & \
        CMN_ENUM_RQUIRES_CLAUSE(supported_ops)  \
    {   \
        return lhs = static_cast<lhs_type>(lazy_to_interop(lhs) op lazy_to_interop(rhs));  \
    }

//-----------------------------------------------------------------------------
//
// bitwise
//
//-----------------------------------------------------------------------------
CMN_ENUM_BINARY_OP(|, op_bitwise, E, E, E)
CMN_ENUM_BINARY_OP(|, op_bitwise | op_interoperable, E, interop_type_t<E>, E)
CMN_ENUM_BINARY_OP(|, op_bitwise | op_interoperable, interop_type_t<E>, E, E)
CMN_ENUM_COMPOUND_OP(|, op_bitwise, E, E)
CMN_ENUM_COMPOUND_OP(|, op_bitwise | op_interoperable, E, interop_type_t<E>)
CMN_ENUM_COMPOUND_OP(|, op_bitwise | op_interoperable, interop_type_t<E>, E)

CMN_ENUM_BINARY_OP(&, op_bitwise, E, E, E)
CMN_ENUM_BINARY_OP(&, op_bitwise | op_interoperable, E, interop_type_t<E>, E)
CMN_ENUM_BINARY_OP(&, op_bitwise | op_interoperable, interop_type_t<E>, E, E)
CMN_ENUM_COMPOUND_OP(&, op_bitwise, E, E)
CMN_ENUM_COMPOUND_OP(&, op_bitwise | op_interoperable, E, interop_type_t<E>)
CMN_ENUM_COMPOUND_OP(&, op_bitwise | op_interoperable, interop_type_t<E>, E)

CMN_ENUM_BINARY_OP(^, op_bitwise, E, E, E)
CMN_ENUM_BINARY_OP(^, op_bitwise | op_interoperable, E, interop_type_t<E>, E)
CMN_ENUM_BINARY_OP(^, op_bitwise | op_interoperable, interop_type_t<E>, E, E)
CMN_ENUM_COMPOUND_OP(^, op_bitwise, E, E)
CMN_ENUM_COMPOUND_OP(^, op_bitwise | op_interoperable, E, interop_type_t<E>)
CMN_ENUM_COMPOUND_OP(^, op_bitwise | op_interoperable, interop_type_t<E>, E)

CMN_ENUM_BINARY_OP(<<, op_bitwise, E, E, E)
CMN_ENUM_BINARY_OP(<<, op_bitwise | op_interoperable, E, interop_type_t<E>, E)
CMN_ENUM_BINARY_OP(<<, op_bitwise | op_interoperable, interop_type_t<E>, E, E)
CMN_ENUM_COMPOUND_OP(<<, op_bitwise, E, E)
CMN_ENUM_COMPOUND_OP(<<, op_bitwise | op_interoperable, E, interop_type_t<E>)
CMN_ENUM_COMPOUND_OP(<<, op_bitwise | op_interoperable, interop_type_t<E>, E)

CMN_ENUM_BINARY_OP(>>, op_bitwise, E, E, E)
CMN_ENUM_BINARY_OP(>>, op_bitwise | op_interoperable, E, interop_type_t<E>, E)
CMN_ENUM_BINARY_OP(>>, op_bitwise | op_interoperable, interop_type_t<E>, E, E)
CMN_ENUM_COMPOUND_OP(>>, op_bitwise, E, E)
CMN_ENUM_COMPOUND_OP(>>, op_bitwise | op_interoperable, E, interop_type_t<E>)
CMN_ENUM_COMPOUND_OP(>>, op_bitwise | op_interoperable, interop_type_t<E>, E)

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
CMN_ENUM_BINARY_OP(+, op_ariphmetic, E, E, E)
CMN_ENUM_BINARY_OP(+, op_ariphmetic | op_interoperable, E, interop_type_t<E>, E)
CMN_ENUM_BINARY_OP(+, op_ariphmetic | op_interoperable, interop_type_t<E>, E, E)
CMN_ENUM_COMPOUND_OP(+, op_ariphmetic, E, E)
CMN_ENUM_COMPOUND_OP(+, op_ariphmetic | op_interoperable, E, interop_type_t<E>)
CMN_ENUM_COMPOUND_OP(+, op_ariphmetic | op_interoperable, interop_type_t<E>, E)

CMN_ENUM_BINARY_OP(-, op_ariphmetic, E, E, E)
CMN_ENUM_BINARY_OP(-, op_ariphmetic | op_interoperable, E, interop_type_t<E>, E)
CMN_ENUM_BINARY_OP(-, op_ariphmetic | op_interoperable, interop_type_t<E>, E, E)
CMN_ENUM_COMPOUND_OP(-, op_ariphmetic, E, E)
CMN_ENUM_COMPOUND_OP(-, op_ariphmetic | op_interoperable, E, interop_type_t<E>)
CMN_ENUM_COMPOUND_OP(-, op_ariphmetic | op_interoperable, interop_type_t<E>, E)

CMN_ENUM_BINARY_OP(*, op_ariphmetic, E, E, E)
CMN_ENUM_BINARY_OP(*, op_ariphmetic | op_interoperable, E, interop_type_t<E>, E)
CMN_ENUM_BINARY_OP(*, op_ariphmetic | op_interoperable, interop_type_t<E>, E, E)
CMN_ENUM_COMPOUND_OP(*, op_ariphmetic, E, E)
CMN_ENUM_COMPOUND_OP(*, op_ariphmetic | op_interoperable, E, interop_type_t<E>)
CMN_ENUM_COMPOUND_OP(*, op_ariphmetic | op_interoperable, interop_type_t<E>, E)

CMN_ENUM_BINARY_OP(/, op_ariphmetic, E, E, E)
CMN_ENUM_BINARY_OP(/, op_ariphmetic | op_interoperable, E, interop_type_t<E>, E)
CMN_ENUM_BINARY_OP(/, op_ariphmetic | op_interoperable, interop_type_t<E>, E, E)
CMN_ENUM_COMPOUND_OP(/, op_ariphmetic, E, E)
CMN_ENUM_COMPOUND_OP(/, op_ariphmetic | op_interoperable, E, interop_type_t<E>)
CMN_ENUM_COMPOUND_OP(/, op_ariphmetic | op_interoperable, interop_type_t<E>, E)

CMN_ENUM_BINARY_OP(%, op_ariphmetic, E, E, E)
CMN_ENUM_BINARY_OP(%, op_ariphmetic | op_interoperable, E, interop_type_t<E>, E)
CMN_ENUM_BINARY_OP(%, op_ariphmetic | op_interoperable, interop_type_t<E>, E, E)
CMN_ENUM_COMPOUND_OP(%, op_ariphmetic, E, E)
CMN_ENUM_COMPOUND_OP(%, op_ariphmetic | op_interoperable, E, interop_type_t<E>)
CMN_ENUM_COMPOUND_OP(%, op_ariphmetic | op_interoperable, interop_type_t<E>, E)

//-----------------------------------------------------------------------------
//
// comparable
//
//-----------------------------------------------------------------------------
//CMN_ENUM_BINARY_OP_AUTO(<=>, op_comparable, E, E)
//CMN_ENUM_BINARY_OP_AUTO(<=>, op_comparable | op_interoperable, E, interop_type_t<E>)
//CMN_ENUM_BINARY_OP_AUTO(<=>, op_comparable | op_interoperable, interop_type_t<E>, E), bool

CMN_ENUM_BINARY_OP(==, op_comparable | op_interoperable, E, interop_type_t<E>, bool)
CMN_ENUM_BINARY_OP(==, op_comparable | op_interoperable, interop_type_t<E>, E, bool)
CMN_ENUM_BINARY_OP(!=, op_comparable | op_interoperable, E, interop_type_t<E>, bool)
CMN_ENUM_BINARY_OP(!=, op_comparable | op_interoperable, interop_type_t<E>, E, bool)

CMN_ENUM_BINARY_OP(>, op_comparable, E, E, bool)
CMN_ENUM_BINARY_OP(>, op_comparable | op_interoperable, E, interop_type_t<E>, bool)
CMN_ENUM_BINARY_OP(>, op_comparable | op_interoperable, interop_type_t<E>, E, bool)

CMN_ENUM_BINARY_OP(>=, op_comparable, E, E, bool)
CMN_ENUM_BINARY_OP(>=, op_comparable | op_interoperable, E, interop_type_t<E>, bool)
CMN_ENUM_BINARY_OP(>=, op_comparable | op_interoperable, interop_type_t<E>, E, bool)

CMN_ENUM_BINARY_OP(<, op_comparable, E, E, bool)
CMN_ENUM_BINARY_OP(<, op_comparable | op_interoperable, E, interop_type_t<E>, bool)
CMN_ENUM_BINARY_OP(<, op_comparable | op_interoperable, interop_type_t<E>, E, bool)

CMN_ENUM_BINARY_OP(<=, op_comparable, E, E, bool)
CMN_ENUM_BINARY_OP(<=, op_comparable | op_interoperable, E, interop_type_t<E>, bool)
CMN_ENUM_BINARY_OP(<=, op_comparable | op_interoperable, interop_type_t<E>, E, bool)

//-----------------------------------------------------------------------------
//
// IO
//
//-----------------------------------------------------------------------------
template <c::adapted_enum E, typename Char, typename CharTaits>
auto operator << (std::basic_ostream<Char, CharTaits> &ostr, E en) -> std::basic_ostream<Char, CharTaits> &
    requires ((ops_v<E> & op_io) == op_io)
{
    return io::printer{ en, int_<kind_v<E>>{} }.print(ostr);                                                                                     \
} 

template <c::adapted_enum E, typename Char, typename CharTaits>
auto operator >> (std::basic_istream<Char, CharTaits> &istr, E &en) -> std::basic_istream<Char, CharTaits> &
    requires ((ops_v<E> & op_io) == op_io)
{
    return io::reader{ en, int_<kind_v<E>>{} }.read(istr);
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
