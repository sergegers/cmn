// ReSharper disable CppClangTidyCppcoreguidelinesMacroUsage
#pragma once

#include <iosfwd>

#include "util.h"
#include "traits.h"

namespace cmn::enum_::op
{

template <typename Enum>
using to_mask_t = typename utils<Enum>::mask_type;

template <typename Enum>
struct fwd
{
    using enum_type = std::decay_t<Enum>;
    using utils_type = utils<enum_type>;
    using mask_type = typename utils_type::mask_type;

    Enum m_val;

    constexpr      fwd(Enum val): m_val{ std::forward<Enum>(val) } {}
    constexpr auto to_mask() const { return utils_type::to_mask(m_val); }
    constexpr auto to_mask() noexcept -> auto& { return utils_type::to_mask(m_val); }
    constexpr auto to_mask_val() const { return utils_type::to_mask_val(m_val); }
};

// NOTE: no need to full qualify names because the macros below are used locally only

#define CMN_PP_ENUM_CONSTEXPR_BINARY_OP(op_)  \
    template <typename Enum> constexpr auto operator op_ (fwd<Enum> const &lhs, fwd<Enum> const &rhs) noexcept  \
    {   \
        return static_cast<Enum>(lhs.to_mask() op_ rhs.to_mask());  \
    } 

#define CMN_PP_ENUM_CONSTEXPR_BINARY_OP_AUTO(op_)  \
    template <typename Enum> constexpr auto operator op_(fwd<Enum> const &lhs, fwd<Enum> const &rhs) noexcept  \
    {   \
        return lhs.to_mask() op_ rhs.to_mask();  \
    }

#define CMN_PP_ENUM_CONSTEXPR_UNARY_OP(op_)  \
    template <typename Enum> constexpr auto operator op_ (fwd<Enum> const &f) noexcept  \
    {   \
        return static_cast<Enum>(op_ f.to_mask());  \
    }

#define CMN_PP_ENUM_MUTABLE_PREFIX_UNARY_OP(op_)  \
    template <typename Enum> auto operator op_ (fwd<Enum &> f) noexcept  \
    {   \
        return static_cast<Enum>(op_ f.to_mask());  \
    }

#define CMN_PP_ENUM_MUTABLE_POSTFIX_UNARY_OP(op_)  \
    template <typename Enum> auto operator op_ (fwd<Enum &> f, int) noexcept  \
    {   \
        auto const tmp = f.m_val;   \
        return f.m_val = static_cast<Enum>(op_ f.to_mask()), tmp;  \
    }

#define CMN_PP_ENUM_COMPOUND_OP(op_)  \
    template <typename Enum> auto operator op_(fwd<Enum &> lhs, fwd<Enum> const &rhs) noexcept  \
    {   \
        return (lhs.to_mask() op_ rhs.to_mask()), lhs.m_val;  \
    }

// bitwise
CMN_PP_ENUM_CONSTEXPR_BINARY_OP(|)
CMN_PP_ENUM_CONSTEXPR_BINARY_OP(&)
CMN_PP_ENUM_CONSTEXPR_BINARY_OP(^)
CMN_PP_ENUM_CONSTEXPR_UNARY_OP(~)
CMN_PP_ENUM_COMPOUND_OP(|=)
CMN_PP_ENUM_COMPOUND_OP(&=)
CMN_PP_ENUM_COMPOUND_OP(^=)

// unit steppable
CMN_PP_ENUM_MUTABLE_PREFIX_UNARY_OP(++)
CMN_PP_ENUM_MUTABLE_POSTFIX_UNARY_OP(++)
CMN_PP_ENUM_MUTABLE_PREFIX_UNARY_OP(--)
CMN_PP_ENUM_MUTABLE_POSTFIX_UNARY_OP(--)

// arithmetic
CMN_PP_ENUM_CONSTEXPR_BINARY_OP(+)
CMN_PP_ENUM_CONSTEXPR_BINARY_OP(-)

// comparable
CMN_PP_ENUM_CONSTEXPR_BINARY_OP_AUTO(==)
CMN_PP_ENUM_CONSTEXPR_BINARY_OP_AUTO(!=)
CMN_PP_ENUM_CONSTEXPR_BINARY_OP_AUTO(>)
CMN_PP_ENUM_CONSTEXPR_BINARY_OP_AUTO(>=)
CMN_PP_ENUM_CONSTEXPR_BINARY_OP_AUTO(<)
CMN_PP_ENUM_CONSTEXPR_BINARY_OP_AUTO(<=)

#undef CMN_PP_ENUM_CONSTEXPR_BINARY_OP
#undef CMN_PP_ENUM_CONSTEXPR_BINARY_OP_AUTO
#undef CMN_PP_ENUM_CONSTEXPR_UNARY_OP
#undef CMN_PP_ENUM_MUTABLE_PREFIX_UNARY_OP
#undef CMN_PP_ENUM_MUTABLE_POSTFIX_UNARY_OP
#undef CMN_PP_ENUM_COMPOUND_OP

}

namespace cmn::enum_::io
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


