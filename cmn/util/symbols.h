#pragma once

#include <concepts>
#include <string>
#include <ios>

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <boost/preprocessor/wstringize.hpp>

#include <cmn/meta/concepts.h>
#include <cmn/util/fixed_string.h>

namespace cmn::sym
{

using namespace literals;

////////////////////////////////////////////////////////////////////////////////
//
// symbol keys
//
////////////////////////////////////////////////////////////////////////////////
template 
<
      c::const_string auto Value_
    , c::const_string auto WValue_
>
struct valued_key_impl
{
    static constexpr decltype(Value_) value_ = Value_;
    static constexpr decltype(WValue_) wvalue_ = WValue_;

#if CMN_STATIC_TEST
    static_assert(Value_.size() == WValue_.size());
#endif
    static constexpr std::size_t size = Value_.size();

    //-----------------------------------------------------------------------------
    template 
    <
          typename Char
        , typename CharTraits = std::char_traits<Char>
    >
    static constexpr auto value() noexcept
    {
        if constexpr (std::same_as<Char, char> && std::same_as<CharTraits, std::char_traits<Char>>)
            return value_;
        else if constexpr (std::same_as<Char, wchar_t> && std::same_as<CharTraits, std::char_traits<wchar_t>>)
            return wvalue_;
        else
            static_assert(!std::same_as<Char, Char>, "Not implemented");
    }

    //-----------------------------------------------------------------------------
    template
    <
          typename Char
        , typename CharTraits = std::char_traits<Char>
    >
    static constexpr auto as_string() noexcept -> std::basic_string<Char, CharTraits>
    {
        return to_string(value<Char, CharTraits>());
    }

    //-----------------------------------------------------------------------------
    template
    <
          typename Char
        , typename CharTraits = std::char_traits<Char>
    >
    static constexpr auto c_str() noexcept -> Char const *
    {
        return value<Char, CharTraits>().c_str();
    }

    //-----------------------------------------------------------------------------
    template 
    <
          typename Char
        , typename CharTraits = std::char_traits<Char>
    > 
        requires 
        (
            size == 1
            ||
            size == 2 && Value_[1] == WValue_[1] == 0
        )
    // ReSharper disable once CppNotAllPathsReturnValue
    static constexpr auto as_char() noexcept -> Char
    {
        if constexpr (std::same_as<Char, char> && std::same_as<CharTraits, std::char_traits<Char>>)
            return value_[0];
        else if constexpr (std::same_as<Char, wchar_t> && std::same_as<CharTraits, std::char_traits<wchar_t>>)
            return wvalue_[0];
        else
            static_assert(!std::same_as<Char, Char>, "Not implemented");
    }

    //-----------------------------------------------------------------------------
    template
    <
          c::const_string auto OtherValue_
        , c::const_string auto OtherWValue_
    >
    constexpr auto operator + (valued_key_impl<OtherValue_, OtherWValue_> other) const noexcept
    {
        return valued_key_impl<value_ + other.value_, wvalue_ + other.wvalue_>{};
    }

    //-----------------------------------------------------------------------------
    template <typename Char, typename CharTraits>
    friend auto operator << (std::basic_ostream<Char, CharTraits> &ostr, valued_key_impl) -> decltype(ostr)
    {
        return ostr << valued_key_impl::value<Char, CharTraits>();
    }
};

#define CMN_SYM_KEY_TYPE(key_) BOOST_PP_CAT(key_, _t)

//-----------------------------------------------------------------------------
#define CMN_SYM_DECLARE_VALUED_KEY(key_, qsym_, qwsym_)                                                                       \
    struct CMN_SYM_KEY_TYPE(key_): valued_key_impl<BOOST_PP_CAT(qsym_, _fs), BOOST_PP_CAT(qwsym_, _wfs)> {}   \
    inline constexpr key_

#define CMN_SYM_DECLARE_VALUED_KEY_2(key_, sym_)     \
    CMN_SYM_DECLARE_VALUED_KEY(key_, BOOST_PP_STRINGIZE(sym_), BOOST_PP_WSTRINGIZE(sym_))

#define CMN_SYM_DECLARE_LETTER_KEY(key_) CMN_SYM_DECLARE_VALUED_KEY_2(key_, key_)
#define CMN_SYM_DECLARE_DIGIT_KEY(key_) CMN_SYM_DECLARE_VALUED_KEY_2(BOOST_PP_CAT(_, key_), key_)

//-----------------------------------------------------------------------------
CMN_SYM_DECLARE_VALUED_KEY(ws, " ", L" ");
CMN_SYM_DECLARE_VALUED_KEY(nothing, "", L"");
CMN_SYM_DECLARE_VALUED_KEY_2(endl, \n);
CMN_SYM_DECLARE_VALUED_KEY_2(ends, \0);
CMN_SYM_DECLARE_VALUED_KEY_2(tab, \t);
CMN_SYM_DECLARE_VALUED_KEY_2(open_square_bracket, [);
CMN_SYM_DECLARE_VALUED_KEY_2(close_square_bracket, ]);
CMN_SYM_DECLARE_VALUED_KEY_2(open_angle_bracket, <);
CMN_SYM_DECLARE_VALUED_KEY_2(close_angle_bracket, >);
CMN_SYM_DECLARE_VALUED_KEY_2(open_figure_bracket, {);
CMN_SYM_DECLARE_VALUED_KEY_2(close_figure_bracket, });
CMN_SYM_DECLARE_VALUED_KEY(open_parenthese, "(", L"(");
CMN_SYM_DECLARE_VALUED_KEY(close_parenthese, ")", L")");
CMN_SYM_DECLARE_VALUED_KEY_2(colon, :);
CMN_SYM_DECLARE_VALUED_KEY(comma, ",", L",");
CMN_SYM_DECLARE_VALUED_KEY(quote, "'", L"'");
CMN_SYM_DECLARE_VALUED_KEY(dquote, "\"", L"\"");
CMN_SYM_DECLARE_VALUED_KEY_2(asterisk, *);
CMN_SYM_DECLARE_VALUED_KEY_2(circumflex, ^);
//CMN_SYM_DECLARE_VALUED_KEY_2(octothorpe, #);
CMN_SYM_DECLARE_VALUED_KEY(octothorpe, "#", L"#");
CMN_SYM_DECLARE_VALUED_KEY_2(plus, +);
CMN_SYM_DECLARE_VALUED_KEY_2(minus, -);
CMN_SYM_DECLARE_VALUED_KEY_2(less, <);
CMN_SYM_DECLARE_VALUED_KEY_2(greater, >);

CMN_SYM_DECLARE_LETTER_KEY(a);
CMN_SYM_DECLARE_LETTER_KEY(b);
CMN_SYM_DECLARE_LETTER_KEY(c);
CMN_SYM_DECLARE_LETTER_KEY(d);
CMN_SYM_DECLARE_LETTER_KEY(e);
CMN_SYM_DECLARE_LETTER_KEY(f);
CMN_SYM_DECLARE_LETTER_KEY(g);
CMN_SYM_DECLARE_LETTER_KEY(h);
CMN_SYM_DECLARE_LETTER_KEY(i);
CMN_SYM_DECLARE_LETTER_KEY(l);
CMN_SYM_DECLARE_LETTER_KEY(m);
CMN_SYM_DECLARE_LETTER_KEY(n);
CMN_SYM_DECLARE_LETTER_KEY(o);
CMN_SYM_DECLARE_LETTER_KEY(p);
CMN_SYM_DECLARE_LETTER_KEY(r);
CMN_SYM_DECLARE_LETTER_KEY(s);
CMN_SYM_DECLARE_LETTER_KEY(t);
CMN_SYM_DECLARE_LETTER_KEY(u);
CMN_SYM_DECLARE_LETTER_KEY(v);
CMN_SYM_DECLARE_LETTER_KEY(x);
CMN_SYM_DECLARE_LETTER_KEY(y);

CMN_SYM_DECLARE_DIGIT_KEY(0);

CMN_SYM_DECLARE_LETTER_KEY(end);
CMN_SYM_DECLARE_LETTER_KEY(empty);
CMN_SYM_DECLARE_VALUED_KEY_2(nullptr_, nullptr);
CMN_SYM_DECLARE_VALUED_KEY_2(hex_prefix, 0x);
CMN_SYM_DECLARE_VALUED_KEY_2(scope_resolution, ::);
CMN_SYM_DECLARE_VALUED_KEY_2(left_arrow, <-);
CMN_SYM_DECLARE_VALUED_KEY_2(right_arrow, ->);
CMN_SYM_DECLARE_VALUED_KEY_2(void_, void);
CMN_SYM_DECLARE_VALUED_KEY_2(class_, class);
CMN_SYM_DECLARE_VALUED_KEY_2(struct_, struct);
CMN_SYM_DECLARE_VALUED_KEY_2(enum_, enum);
CMN_SYM_DECLARE_LETTER_KEY(vm);
CMN_SYM_DECLARE_LETTER_KEY(vah);
CMN_SYM_DECLARE_LETTER_KEY(vap);
CMN_SYM_DECLARE_LETTER_KEY(vach);

CMN_SYM_DECLARE_VALUED_KEY_2(def_fmt_1_arg, {}\0);

//-----------------------------------------------------------------------------
#undef CMN_SYM_DECLARE_VALUED_KEY
#undef CMN_SYM_DECLARE_VALUED_KEY_2
#undef CMN_SYM_DECLARE_LETTER_KEY
#undef CMN_SYM_DECLARE_DIGIT_KEY
#undef CMN_SYM_KEY_TYPE

}
