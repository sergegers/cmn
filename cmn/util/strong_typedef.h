#pragma once

#include <type_traits>
#include <concepts>

#include <gsl/gsl>

// boost
#include <boost/operators.hpp>
#include <boost/functional/hash.hpp>
// boost.serialization
#include <boost/serialization/traits.hpp>
// boost.io
#include <boost/io/ios_state.hpp>

#include <cmn/cfg/macro.h>

#include <cmn/shared/meta/concepts.h>
#include <cmn/shared/meta/type_traits.h>

namespace cmn
{

namespace io
{

template <c::unit Unit> constexpr bool enable_luxury_io_v = strong_typedef_fmt_traits<Unit>::enable_luxury_io;
template <c::unit Unit> constexpr int_fmt_t default_v = strong_typedef_fmt_traits<Unit>::default_;

//-----------------------------------------------------------------------------
struct access
{
    template <typename Char, typename CharTraits>
    static auto out(std::basic_ostream<Char, CharTraits> &ostr, c::unit auto const &unit) -> void { unit.out(ostr), ostr; }

    template <typename Char, typename CharTraits>
    static auto in(std::basic_istream<Char, CharTraits> &istr, c::unit auto &unit) -> void { unit.in(istr), istr; }

    static auto value_ref(c::unit auto &unit) -> auto& { return unit.m_t; }
};

}

//-----------------------------------------------------------------------------
//
template <typename U, std::integral T, T Default_ = {} >
class RS_PASS_BY_VALUE_ATTR strong_typedef_impl :
      public strong_typedef_tag
    //, private boost::totally_ordered<U>
    //, private boost::totally_ordered<U, T>
    , public boost::serialization::traits
    <
        U, 
        boost::serialization::primitive_type,
        boost::serialization::track_never
    >
{
private:
    using itself = strong_typedef_impl;
    friend io::access;
protected:
    T m_t = Default_;

    // NOTE: in(), out() implementation must be consistent, 
    // because they are used in XML serialization

    //-----------------------------------------------------------------------------
    template <typename Char, typename CharTraits>
    /* CRTP polymorphic*/ auto out(std::basic_ostream<Char, CharTraits> &ostr) const -> void { ostr << m_t; }

    // unary plus (+) here to promote char types to int ones
    // https://isocpp.org/wiki/faq/input-output#print-char-or-ptr-as-number
    template <typename Char, typename CharTraits>
        requires is_char_v<T>
    /* CRTP polymorphic*/ auto out(std::basic_ostream<Char, CharTraits> &ostr) const -> void { ostr << +m_t; }

    //-----------------------------------------------------------------------------
    template <typename Char, typename CharTraits>
    /* CRTP polymorphic*/ auto in(std::basic_istream<Char, CharTraits> &istr) -> void { istr >> m_t; }

    template <typename Char, typename CharTraits>
        requires is_char_v<T>
    /* CRTP polymorphic*/ auto in(std::basic_istream<Char, CharTraits> &istr) -> void
    {
        decltype(+m_t) t;   // to be consistent with char output
        istr >> t;
        m_t = t;
    }
public:
    using strong_typedef_type = U;
    //-----------------------------------------------------------------------------
    // unit concept interface
    using underlying_type = T;
    //-----------------------------------------------------------------------------

    constexpr strong_typedef_impl() = default;
    constexpr explicit strong_typedef_impl(T t) noexcept : m_t { t } {}

    constexpr strong_typedef_impl(itself const &) = default;
    constexpr strong_typedef_impl(itself &&) = default;

    constexpr auto operator=(itself const &) noexcept -> itself& = default;
    constexpr auto operator=(itself &&) noexcept -> itself& = default;

    constexpr auto operator=(T const &rhs) noexcept -> itself& { return m_t = rhs, *this; }
    constexpr auto operator=(T &&rhs) noexcept -> itself& { return m_t = std::move(rhs), *this; }

    constexpr explicit operator T () const noexcept { return m_t; }
    constexpr explicit operator T & () noexcept { return m_t; }
    // can't be generated due to bases
    constexpr auto operator <=> (itself const & rhs) const noexcept { return m_t <=> rhs.m_t; }
    constexpr auto operator <=> (T rhs) const noexcept { return m_t <=> rhs; }

    constexpr auto operator == (itself const & rhs) const noexcept -> bool { return m_t == rhs.m_t; }
    constexpr auto operator == (T rhs) const noexcept -> bool { return m_t == rhs; }

    /* CRTP polymorphic */
    constexpr auto     operator !() const noexcept -> bool { return !m_t; }
    constexpr explicit operator bool() const noexcept { return !!static_cast<U const &>(*this); }

    friend auto hash_value(U const &u) noexcept { return boost::hash<T>{}(u.m_t); }

    constexpr auto value() -> T& { return m_t; }
    constexpr auto value() const -> T { return m_t; }
};

template
<
      typename Char
    , typename CharTraits
>
auto operator << (std::basic_ostream<Char, CharTraits> &ostr, c::unit auto const &unit)->
    std::basic_ostream<Char, CharTraits> &
{ 
    boost::io::ios_all_saver const _{ ostr };
    return io::access::out(ostr, unit), ostr;
}

template
<
      typename Char
    , typename CharTraits
>
auto operator << (std::basic_ostream<Char, CharTraits> &ostr, c::fmt_unit auto const &/*unit*/)->
    std::basic_ostream<Char, CharTraits> &
{ 
    static_assert
    (
        sizeof(Char) == 0,
        "If you want to luxury output strong typedef, include header <cmn/shared/util/strong_typedef_io.h>"
    );
    return ostr;
}

template
<
      typename Char
    , typename CharTraits
>
auto operator >> (std::basic_istream<Char, CharTraits> &istr, c::unit auto &unit)->
    std::basic_istream<Char, CharTraits> &
{
    boost::io::ios_all_saver const _{ istr };
    return io::access::in(istr, unit), istr;
}

template
<
      typename Char
    , typename CharTraits
>
auto operator >> (std::basic_istream<Char, CharTraits> &istr, c::fmt_unit auto &unit)->
    std::basic_istream<Char, CharTraits> &
{
    static_assert
    (
        sizeof(Char) == 0,
        "If you want to luxury input strong typedef, include header <cmn/shared/util/strong_typedef_io.h>"
    );
    return istr;        
}

//-----------------------------------------------------------------------------
template
<
      typename U
    , std::integral T
    , T Default_ = {}
    , std::signed_integral Ptrdiff = std::ptrdiff_t
>
class RS_PASS_BY_VALUE_ATTR strong_typedef_ptr_impl :
      public strong_typedef_impl<U, T, Default_>
    , private boost::additive<U, Ptrdiff>
    , private boost::unit_steppable<U>
{
private:
    using inherited     = strong_typedef_impl<U, T, Default_>;
    using itself        = strong_typedef_ptr_impl;
public:
    using difference_type = Ptrdiff;

    using inherited::inherited;
    using inherited::operator =;

#pragma warning(suppress: 26434)
    constexpr auto operator <=> (itself const &rhs) const noexcept { return inherited::operator<=>(rhs); }
#pragma warning(suppress: 26434)
    constexpr auto operator <=> (T rhs) const noexcept { return inherited::operator<=>(rhs); }

    constexpr auto operator += (Ptrdiff rhs) -> U& { return this->m_t += rhs, static_cast<U &>(*this); }
    constexpr auto operator -= (Ptrdiff rhs) -> U& { return this->m_t -= rhs, static_cast<U &>(*this); }

    constexpr auto operator ++() -> U & { return ++this->m_t, static_cast<U &>(*this); }
    constexpr auto operator --() -> U & { return --this->m_t, static_cast<U &>(*this); }

    friend constexpr auto operator - (U const &lhs, U const &rhs) -> Ptrdiff { return static_cast<Ptrdiff>(lhs.m_t - rhs.m_t); }
};

//-----------------------------------------------------------------------------
template
<
      typename U
    , std::integral T
    , T Default_ = {}
    , std::signed_integral Ptrdiff = std::ptrdiff_t
>
class RS_PASS_BY_VALUE_ATTR strong_typedef_ptr_interop_impl :
      public strong_typedef_impl<U, T, Default_>
    //, private boost::additive<U, Ptrdiff>
    , private boost::unit_steppable<U>
{
private:
    using inherited     = strong_typedef_impl<U, T, Default_>;
    using itself        = strong_typedef_ptr_interop_impl;
public:
    using difference_type = Ptrdiff;

    using inherited::inherited;
    using inherited::operator =;

#pragma warning(suppress: 26434)
    constexpr auto operator <=> (itself const &rhs) const noexcept { return inherited::operator<=>(rhs); }
#pragma warning(suppress: 26434)
    constexpr auto operator <=> (c::int_convertible_to<T> auto rhs) const noexcept
    {
        return inherited::operator <=> (gsl::narrow_cast<T>(rhs));
    }

    // allow interop operations
    constexpr auto operator += (c::int_convertible_to<Ptrdiff> auto rhs) -> U &
    {
        this->m_t += gsl::narrow_cast<Ptrdiff>(rhs);
        return static_cast<U &>(*this);
    }

    friend constexpr auto operator + (U const &lhs, c::int_convertible_to<Ptrdiff> auto rhs) -> U
    {
        auto res = lhs;
        return std::move(res += rhs);
    }

    constexpr auto operator -= (c::int_convertible_to<Ptrdiff> auto rhs) -> U &
    {
        this->m_t -= gsl::narrow_cast<Ptrdiff>(rhs);
        return static_cast<U &>(*this);
    }

    friend constexpr auto operator - (U const &lhs, c::int_convertible_to<Ptrdiff> auto rhs) -> U
    {
        auto res = lhs;
        return std::move(res -= rhs); 
    }

    constexpr auto operator ++ () -> U & { return ++this->m_t, static_cast<U &>(*this); }
    constexpr auto operator -- () -> U & { return --this->m_t, static_cast<U &>(*this); }

    friend constexpr auto operator - (U const &lhs, U const &rhs) -> Ptrdiff
    { return gsl::narrow_cast<Ptrdiff>(lhs.m_t - rhs.m_t); }
};

//-----------------------------------------------------------------------------
template <typename U, std::integral T, T Default_ = {}>
class RS_PASS_BY_VALUE_ATTR strong_typedef_flag_impl :
      public strong_typedef_impl<U, T, Default_>
    , private boost::bitwise<U, U>
{
private:
    using inherited = strong_typedef_impl<U, T, Default_>;
    using itself = strong_typedef_flag_impl;
public:
    using inherited::inherited;
    using inherited::operator =;

    constexpr auto operator <=> (itself const &rhs) const noexcept { return inherited::operator<=>(rhs); }
    constexpr auto operator <=> (T rhs) const noexcept { return inherited::operator<=>(rhs); }

    constexpr auto operator ~ () const -> U { return U { ~this->m_t }; }
    constexpr auto operator ^= (U const &rhs) -> U& { return this->m_t ^= rhs.m_t, static_cast<U &>(*this); }
    constexpr auto operator &= (U const &rhs) -> U& { return this->m_t &= rhs.m_t, static_cast<U &>(*this); }
    constexpr auto operator |= (U const &rhs) -> U& { return this->m_t |= rhs.m_t, static_cast<U &>(*this); }
};

//-----------------------------------------------------------------------------
template <typename U, std::integral T, T Default_ = {}>
class RS_PASS_BY_VALUE_ATTR strong_typedef_flag_interop_impl :
      public strong_typedef_flag_impl<U, T, Default_>
    , private boost::bitwise<U, T>
{
private:
    using inherited     = strong_typedef_flag_impl<U, T, Default_>;
    using itself        = strong_typedef_flag_interop_impl;
public:
    using inherited::inherited;
    using inherited::operator=;

    constexpr auto operator <=> (itself const &rhs) const noexcept { return inherited::operator <=> (rhs); }
    constexpr auto operator <=> (T rhs) const noexcept { return inherited::operator <=> (rhs); }

    using inherited::operator ^=;
    using inherited::operator &=;
    using inherited::operator |=;

    constexpr auto operator ^= (T const &rhs) -> U& { return this->m_t ^= rhs, static_cast<U &>(*this); }
    constexpr auto operator &= (T const &rhs) -> U& { return this->m_t &= rhs, static_cast<U &>(*this); }
    constexpr auto operator |= (T const &rhs) -> U& { return this->m_t |= rhs, static_cast<U &>(*this); }
};

//-----------------------------------------------------------------------------
template <typename U, std::integral T, T Default_ = T{}>
class RS_PASS_BY_VALUE_ATTR strong_typedef_integral_impl :
      public strong_typedef_impl<U, T, Default_>
    , private boost::integer_arithmetic<U>
    , private boost::unit_steppable<U>
{
private:
    using inherited     = strong_typedef_impl<U, T, Default_>;
    using itself        = strong_typedef_integral_impl;
public:
    using inherited::inherited;
    using inherited::operator=;

#pragma warning(suppress: 26434)
    constexpr auto operator <=> (itself const &rhs) const noexcept { return inherited::operator <=> (rhs); }
#pragma warning(suppress: 26434)
    constexpr auto operator <=> (T rhs) const noexcept { return inherited::operator <=> (rhs); }

    constexpr auto operator += (U const &rhs) -> U& { return this->m_t += rhs.m_t, static_cast<U &>(*this); }
    constexpr auto operator -= (U const &rhs) -> U& { return this->m_t -= rhs.m_t, static_cast<U &>(*this); }
    constexpr auto operator *= (U const &rhs) -> U& { return this->m_t *= rhs.m_t, static_cast<U &>(*this); }
    constexpr auto operator /= (U const &rhs) -> U& { return this->m_t /= rhs.m_t, static_cast<U &>(*this); }
    constexpr auto operator %= (U const &rhs) -> U& { return this->m_t %= rhs.m_t, static_cast<U &>(*this); }
    constexpr auto operator ++ () -> U& { return ++this->m_t, static_cast<U &>(*this); }
    constexpr auto operator -- () -> U& { return --this->m_t, static_cast<U &>(*this); }
};

//-----------------------------------------------------------------------------
template <typename U, std::integral T, T Default_ = {}>
class RS_PASS_BY_VALUE_ATTR strong_typedef_integral_interop_impl :
      public strong_typedef_integral_impl<U, T>
    //, private boost::integer_arithmetic<U, T>
{
private:
    using inherited     = strong_typedef_integral_impl<U, T, Default_>;
    using itself        = strong_typedef_integral_interop_impl;
public:
    using inherited::inherited;
    using inherited::operator =;

#pragma warning(suppress: 26434)
    constexpr auto operator <=> (itself const &rhs) const noexcept { return inherited::operator <=> (rhs); }
#pragma warning(suppress: 26434)
    constexpr auto operator <=> (c::int_convertible_to<T> auto rhs) const noexcept
    {
        return inherited::operator <=> (gsl::narrow_cast<T>(rhs));
    }

    using inherited::operator +=;
    using inherited::operator -=;
    using inherited::operator *=;
    using inherited::operator /=;
    using inherited::operator %=;

    constexpr auto operator += (c::int_convertible_to<T> auto const &rhs) -> U &
    {
        this->m_t += gsl::narrow_cast<T>(rhs);
        return static_cast<U &>(*this);
    }

    friend constexpr auto operator + (U const &lhs, c::int_convertible_to<T> auto rhs) -> U
    {
        auto res = lhs;
        return std::move(res += rhs);
    }

    constexpr auto operator -=(c::int_convertible_to<T> auto rhs) -> U &
    {
        this->m_t -= gsl::narrow_cast<T>(rhs);
        return static_cast<U &>(*this);
    }

    friend constexpr auto operator - (U const &lhs, c::int_convertible_to<T> auto rhs) -> U
    {
        auto res = lhs;
        return std::move(res -= rhs);
    }

    constexpr auto operator *= (T rhs) -> U &
    {
        this->m_t *= gsl::narrow_cast<T>(rhs);
        return static_cast<U &>(*this);
    }

    friend constexpr auto operator * (U const &lhs, c::int_convertible_to<T> auto rhs) -> U
    {
        auto res = lhs;
        return std::move(res *= rhs);
    }

    constexpr auto operator /= (T rhs) -> U &
    {
        this->m_t /= gsl::narrow_cast<T>(rhs);
        return static_cast<U &>(*this);
    }

    friend constexpr auto operator / (U const &lhs, c::int_convertible_to<T> auto rhs) -> U
    {
        auto res = lhs;
        return std::move(res /= rhs);
    }

    constexpr auto operator %= (T rhs) -> U &
    {
        this->m_t %= gsl::narrow_cast<T>(rhs);
        return static_cast<U &>(*this);
    }

    friend constexpr auto operator % (U const &lhs, c::int_convertible_to<T> auto rhs) -> U
    {
        auto res = lhs;
        return std::move(res %= rhs);
    }
};

//-----------------------------------------------------------------------------
template 
<
      typename T
    , typename Tag
    , T Default_
    , template <typename, typename TT, TT, typename...> typename ImplT
    , typename... Args
>
class RS_PASS_BY_VALUE_ATTR st_facade final:
    public ImplT<st_facade<T, Tag, Default_, ImplT, Args...>, T, Default_, Args...>
{
private:
    using itself = st_facade;
    using inherited = ImplT<itself, T, Default_, Args...>;
public:
    using inherited::inherited;
};

template <std::integral T, typename Tag, T Default_ = {}> 
using strong_typedef = st_facade<T, Tag, Default_, strong_typedef_impl>;

template <std::integral T, typename Tag, T Default_ = {}, std::signed_integral Ptrdiff = std::ptrdiff_t> 
using strong_typedef_ptr = st_facade<T, Tag, Default_, strong_typedef_ptr_impl, Ptrdiff>;

template <std::integral T, typename Tag, T Default_ = {}> 
using strong_typedef_flag = st_facade<T, Tag, Default_, strong_typedef_flag_impl>;

template <std::integral T, typename Tag, T Default_ = {}> 
using strong_typedef_flag_interop = st_facade<T, Tag, Default_, strong_typedef_flag_interop_impl>;

template <std::integral T, typename Tag, T Default_ = {}> 
using strong_typedef_integral = st_facade<T, Tag, Default_, strong_typedef_integral_impl>;

template <std::integral T, typename Tag, T Default_ = {}> 
using strong_typedef_integral_interop = st_facade<T, Tag, Default_, strong_typedef_integral_interop_impl>;

}

#ifdef UNITY_BUILD
#   include <cmn/shared/util/strong_typedef_io.h>
#endif
