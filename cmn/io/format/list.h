#pragma once

#include <concepts>
#include <type_traits>
#include <format>
#include <string_view>
#include <algorithm>
#include <iterator>
#include <utility>

#include <boost/type_traits/promote.hpp>

#include <cmn/meta/concepts.h>
#include <cmn/util/symbols.h>
#include <cmn/util/feature.h>

namespace cmn::io
{

enum list_options_t
{
    lo_empty        = 0x0,
    lo_brackers     = 0x1,
    lo_separator    = 0x2
};

template <typename T>
struct traits
{
    static constexpr boost::promote_t<list_options_t> fmt_options = lo_empty;
};

//-----------------------------------------------------------------------------
template <typename T>
struct list
{
    T m_t;

    template <c::explicitly_convertible_to<T> Arg>
    constexpr list(Arg arg) noexcept: m_t{ static_cast<T>(arg) } {}
};

template <typename T>
struct list<T const &>
{
    T const &m_t;

    constexpr list(T const &arg) noexcept: m_t{ arg } {}
};

template <typename T> list(T const &) noexcept -> list<T const &>;


///////////////////////////////////////////////////////////////////////////////
enum scroll_result_t: char
{
    sr_close    = 0x0,      // end of format string, iterator is pointed to '}'
    sr_sep      = 0x1,      // next formatting item, iterator is pointed to separator ':'
    sr_eos      = 0x2,      // end of format string, iterator in equal to the end iterator
    sr_sym      = 0x4,      // ordinary symbol
    sr_esc      = 0x8,      // escape flag

    sr_naked    = sr_close | sr_sep | sr_eos | sr_sym
};

//-----------------------------------------------------------------------------
template <typename Char> using scroll_to = std::pair<Char, scroll_result_t>;

template <typename Char> constexpr scroll_to<Char> scroll_to_sep_ = scroll_to{ cmn::to_char(symbols<Char>::colon), sr_sep };
template <typename Char> constexpr scroll_to<Char> scroll_to_close_ = scroll_to{ cmn::to_char(symbols<Char>::close_figure_bracket), sr_close };

//-----------------------------------------------------------------------------
template <typename Iterator> using scroll_pos = std::pair<Iterator, boost::promote_t<scroll_result_t>>;

//-----------------------------------------------------------------------------
template <typename ParseContext> using context_iterator_t = typename ParseContext::iterator;
template <typename ParseContext> using context_char_t = typename ParseContext::char_type;

template <typename ParseContext> using context_scroll_to_t = scroll_to<context_char_t<ParseContext>>;
template <typename ParseContext> using context_scroll_pos_t = scroll_pos<context_iterator_t<ParseContext>>;

namespace detail
{

//-----------------------------------------------------------------------------
//
// return parse state including sr_esc flag
//
template <typename ParseContext>
[[nodiscard]] constexpr auto decode_state_at(ParseContext const &ctx, context_iterator_t<ParseContext> it) noexcept
    -> boost::promote_t<scroll_result_t>
{
    using char_type = context_char_t<ParseContext>;
    using symbols_type = symbols<char_type>;

    constexpr char_type separator_fmt = cmn::to_char(symbols_type::colon);
    constexpr char_type open_fmt = cmn::to_char(symbols_type::open_figure_bracket);
    constexpr char_type close_fmt = cmn::to_char(symbols_type::close_figure_bracket);

    boost::promote_t<scroll_result_t> state;
    if  (it == ctx.end()) state = sr_eos;
    else 
        switch (*it)
        {
        case open_fmt:
            // check to escaped { symbol
            ++it;
            state = it != ctx.end() && *it == open_fmt? sr_sym | sr_esc: sr_sym;
        break;

        case close_fmt:
            // check to escaped } symbol
            ++it;
            state = it != ctx.end() && *it == close_fmt? sr_sym | sr_esc: sr_close;
        break;

        case separator_fmt: state = sr_sep; break;
        default: state = sr_sym;
        }

    return state;
}

}

//-----------------------------------------------------------------------------
//
// check parse position state ignoring sr_esc flag
// return parse state including sr_esc flag
//
template <typename ParseContext>
[[nodiscard]] constexpr auto check_state_at(ParseContext const &ctx, context_iterator_t<ParseContext> it, 
    boost::promote_t<scroll_result_t> expected_states) -> boost::promote_t<scroll_result_t>
{
    auto const state = detail::decode_state_at(ctx, it);    
    switch (auto const naked_state = feature(state, sr_naked); naked_state)
    {
    case sr_close:
    {
        if (!has_feature(expected_states, naked_state))
            throw std::format_error("Invalid format string for list. Additional arguments");
    }
        break;

    case sr_sep:
    {
        if (!has_feature(expected_states, naked_state))
            throw std::format_error("Invalid format string for list. Not enough arguments");
    }
        break;

    case sr_eos:
        throw std::format_error("Invalid format string for list. Missing closing '}'");

    case sr_sym:
    {
        if (!has_feature(expected_states, naked_state))
            throw std::format_error("Invalid format string for list. State is unknown");
    }
        break;

    default:
        throw std::format_error("Invalid format string for list. State is unknown");
    }

    return state;
};

namespace detail
{

template <typename ParseContext>
constexpr auto check_find
(
      ParseContext const &ctx
    , context_iterator_t<ParseContext> it
    , context_scroll_to_t<ParseContext> tgt
) 
    -> context_iterator_t<ParseContext>
{
    auto const [sym, sr] = tgt;
    auto res_it = std::find(it, ctx.end(), sym);

    auto state = decode_state_at(ctx, res_it);
    if (has_feature(state, sr_esc))
    {
        // scroll escape sequence
        ++res_it; ++res_it;

        return check_find(ctx, res_it, tgt);
    }

    // verify target
    std::ignore = check_state_at(ctx, res_it, sr);
    return  res_it;
}

}

//-----------------------------------------------------------------------------
//
// return iterator pointed to the target
//
template
<
      typename ParseContext
    , typename... Tgts
>
[[nodiscard]] constexpr auto check_find_symbol
(
      ParseContext const &ctx
    , context_iterator_t<ParseContext> it
    , context_scroll_to_t<ParseContext> tgt
    , Tgts... tgts
)
    -> context_iterator_t<ParseContext>

    requires (std::same_as<Tgts, context_scroll_to_t<ParseContext>> && ...)
{
    using target_type = context_scroll_to_t<ParseContext>;
    using iterator_type = context_iterator_t<ParseContext>;

    iterator_type res;
    std::ignore = 
    (
        (ctx.end() != (res = detail::check_find(ctx, it, tgt)))
        || ... || 
        (ctx.end() != (res = detail::check_find(ctx, it, tgts)))
    );

    return res;
}

}

namespace std
{

template <typename T, typename Char>
    requires formattable<std::remove_cvref_t<T>, Char>
struct formatter<cmn::io::list<T>, Char>
{
    using underlying_formatting_type = std::remove_cvref_t<T>;
    using underlying_formatter_type = formatter<underlying_formatting_type, Char>;
    using string_view_type = std::basic_string_view<Char>;

    underlying_formatter_type m_underlying_formatter;

    template<typename ParseContext>
    constexpr auto parse(ParseContext &ctx) -> typename ParseContext::iterator
    {
        using namespace cmn::io;

        using char_type = context_char_t<ParseContext>;
        using iterator_type = context_iterator_t<ParseContext>;
        using scroll_pos_type = context_scroll_pos_t<ParseContext>;
        using traits_type = traits<underlying_formatting_type>;

        constexpr auto scroll_to_sep = scroll_to_sep_<char_type>;
        constexpr auto scroll_to_close = scroll_to_close_<char_type>;

        // set iterator to the beginning of escaped symbol sequence
        constexpr auto get_begin_it_ = [](boost::promote_t<scroll_result_t> sr,  iterator_type it) constexpr -> iterator_type
        {
            return cmn::has_feature(sr, sr_esc)? ++it: it;
        };

        constexpr bool has_brackets = cmn::has_feature(traits_type::fmt_options, lo_brackers);
        constexpr bool has_separator = cmn::has_feature(traits_type::fmt_options, lo_separator);

        auto it = ctx.begin();
        auto const begin_it = it;

        if constexpr (!has_brackets)
        {
            if constexpr (!has_separator)
            {
                //-----------------------------------------------------------------------------
                // no brackets, no separator

                std::ignore = check_state_at(ctx, it, sr_close);
                return m_underlying_formatter.parse(ctx);
            }
            else
            {
                //-----------------------------------------------------------------------------
                // separator only

                auto const sep_sr = check_state_at(ctx, it,  sr_sym | sr_close);
                auto const end_sep_it = check_find_symbol(ctx, it, scroll_to_close);
                string_view_type const separator { get_begin_it_(sep_sr, it), end_sep_it };

                m_underlying_formatter.set_separator(separator);

                ctx.advance_to(end_sep_it);
                return m_underlying_formatter.parse(ctx);
            }
        }
        else
        {
            if constexpr (!has_separator)
            {
                //-----------------------------------------------------------------------------
                // brackets only

                auto const open_sr = check_state_at(ctx, it, sr_sym | sr_sep);
                auto const end_open_br_it = check_find_symbol(ctx, it, scroll_to_sep);
                string_view_type const open_br { get_begin_it_(open_sr, it), end_open_br_it };

                it = end_open_br_it; ++it;
                auto const close_sr = check_state_at(ctx, it, sr_sym);
                auto const end_close_br_it = check_find_symbol(ctx, it, scroll_to_close);
                string_view_type const close_br { get_begin_it_(close_sr, it), end_close_br_it };

                m_underlying_formatter.set_brackets(open_br, close_br);

                ctx.advance_to(end_close_br_it);
                return m_underlying_formatter.parse(ctx);
            }
            else
            {
                //-----------------------------------------------------------------------------
                // brackets & separator

                auto const open_sr = check_state_at(ctx, it, sr_sym | sr_sep);
                auto const end_open_br_it = check_find_symbol(ctx, it, scroll_to_sep);
                string_view_type const open_br{ get_begin_it_(open_sr, it), end_open_br_it };

                it = end_open_br_it; ++it;
                auto const sep_sr = check_state_at(ctx, it,  sr_sym | sr_sep);
                auto const end_sep_it = check_find_symbol(ctx, it, scroll_to_sep);
                string_view_type const separator { get_begin_it_(sep_sr, it), end_sep_it };

                it = end_sep_it; ++it;
                auto const close_sr = check_state_at(ctx, it, sr_sym);
                auto const end_close_br_it = check_find_symbol(ctx, it, scroll_to_close);
                string_view_type const close_br { get_begin_it_(close_sr, it), end_close_br_it };

                m_underlying_formatter.set_brackets(open_br, close_br);
                m_underlying_formatter.set_separator(separator);

                ctx.advance_to(end_close_br_it);
                return m_underlying_formatter.parse(ctx);
            }
        }

    }

    template<typename FmtContext>
    constexpr auto format(cmn::io::list<T> const &t, FmtContext &ctx) const -> typename FmtContext::iterator
    {
        return m_underlying_formatter.format(t.m_t, ctx);
    }    
};

}