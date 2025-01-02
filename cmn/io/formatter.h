#pragma once

#include <concepts>
#include <type_traits>
#include <format>
#include <string_view>
#include <algorithm>
#include <iterator>

#include <boost/type_traits/promote.hpp>

#include <cmn/meta/concepts.h>
#include <cmn/util/symbols.h>
#include <cmn/util/feature.h>

namespace cmn::io
{

enum fmt_options_t
{
    fo_empty        = 0x0,
    fo_brackers     = 0x1,
    fo_separator    = 0x2
};

template <typename T>
struct traits
{
    static constexpr boost::promote_t<fmt_options_t> fmt_options = fo_empty;
};

//-----------------------------------------------------------------------------
template <typename T>
struct fmt
{
    T m_t;

    template <c::explicitly_convertible_to<T> Arg>
    constexpr fmt(Arg arg) noexcept: m_t{ static_cast<T>(arg) } {}
};

template <typename T>
struct fmt<T const &>
{
    T const &m_t;

    constexpr fmt(T const &arg) noexcept: m_t{ arg } {}
};

template <typename T> fmt(T const &) noexcept -> fmt<T const &>;


///////////////////////////////////////////////////////////////////////////////
enum scroll_result_t
{
    sr_end = 0x0,       // end of format string, iterator is pointed to '}'
    sr_sep  = 0x1,      // next formatting item, iterator is pointed to separator ':'
    sr_eos  = 0x2,      // end of format string, iterator in equal to the end iterator
    sr_unk  = 0x4       // state is unknown
};

//-----------------------------------------------------------------------------
template <typename ParseContext>
[[nodiscard]] constexpr auto decode_state_at(ParseContext const &ctx, typename ParseContext::iterator it) noexcept -> scroll_result_t
{
    using char_type = typename ParseContext::char_type;
    using symbols_type = symbols<char_type>;

    constexpr auto separator_fmt = cmn::to_char(symbols_type::colon);
    constexpr auto end_fmt = cmn::to_char(symbols_type::close_figure_bracket);

    scroll_result_t state;
    if  (it == ctx.end()) state = sr_eos;
    else 
        switch (*it)
        {
        case end_fmt: state = sr_end; break;
        case separator_fmt: state = sr_sep; break;
        default: state = sr_unk;
        }

    return state;
}

//-----------------------------------------------------------------------------
template <typename ParseContext>
constexpr auto check_state_at(ParseContext const &ctx, typename ParseContext::iterator it, 
    boost::promote_t<scroll_result_t> expected_states)  -> scroll_result_t
{
    auto const state = decode_state_at(ctx, it);
    switch (state)
    {
    case sr_end:
    {
        if (!has_feature(expected_states, state))
            throw std::format_error("Invalid format string for adapted enum. Additional arguments");
    }
        break;

    case sr_sep:
    {
        if (!has_feature(expected_states, state))
            throw std::format_error("Invalid format string for adapted enum. Not enough arguments");
    }
        break;

    case sr_eos:
        throw std::format_error("Invalid format string for adapted enum. Missing closing '}'");

    case sr_unk:
    {
        if (!has_feature(expected_states, state))
            throw std::format_error("Invalid format string for adapted enum. State is unknown");
    }
        break;

    default:
        throw std::format_error("Invalid format string for adapted enum. State is unknown");
    }

    return state;
};

//-----------------------------------------------------------------------------
template
<
      typename ParseContext
    , typename... Tgts
>
constexpr auto find_symbol(ParseContext &ctx, typename ParseContext::iterator it, typename ParseContext::char_type tgt, 
    Tgts... tgts) noexcept -> typename ParseContext::iterator

    requires (std::same_as<Tgts, typename ParseContext::char_type> && ...)
{
    typename ParseContext::iterator res;
    std::ignore = 
    (
        (ctx.end() != (res = std::find(it, ctx.end(), tgt))) 
        || ... || 
        (ctx.end() != (res = std::find(it, ctx.end(), tgts)))
    );

    return res;
}

template <typename ParseContext>
constexpr auto find_next(ParseContext &ctx, typename ParseContext::iterator it) noexcept -> typename ParseContext::iterator
{
    using char_type = typename ParseContext::char_type;
    using symbols_type = symbols<char_type>;
    static constexpr auto separator_fmt = cmn::to_char(symbols_type::colon);

    return find_symbol(ctx, it, separator_fmt);
}

}

namespace std
{

template <typename T, typename Char>
    requires formattable<std::remove_cvref_t<T>, Char>
struct formatter<cmn::io::fmt<T>, Char>
{
    using underlying_formatting_type = std::remove_cvref_t<T>;
    using underlying_formatter_type = formatter<underlying_formatting_type, Char>;
    using string_view_type = std::basic_string_view<Char>;

    underlying_formatter_type m_underlying_formatter;

    template<typename ParseContext>
    constexpr auto parse(ParseContext &ctx) -> typename ParseContext::iterator
    {
        using namespace cmn::io;

        using char_type = typename ParseContext::char_type;
        using symbols_type = cmn::symbols<char_type>;
        using traits_type = traits<underlying_formatting_type>;

        constexpr auto separator_fmt = cmn::to_char(symbols_type::colon);
        constexpr auto end_fmt = cmn::to_char(symbols_type::close_figure_bracket);

        constexpr bool has_brackets = cmn::has_feature(traits_type::fmt_options, fo_brackers);
        constexpr bool has_separator = cmn::has_feature(traits_type::fmt_options, fo_separator);

        auto it = ctx.begin();
        auto const begin_it = it;

        if constexpr (!has_brackets)
        {
            if constexpr (!has_separator)
            {
                //-----------------------------------------------------------------------------
                // no brackets, no separator

                check_state_at(ctx, it, sr_end);
                return m_underlying_formatter.parse(ctx);
            }
            else
            {
                //-----------------------------------------------------------------------------
                // separator only

                check_state_at(ctx, it,  sr_unk | sr_end);
                auto const end_sep_it = find_symbol(ctx, it, end_fmt);
                check_state_at(ctx, end_sep_it, sr_end);
                string_view_type const separator { it, end_sep_it };

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

                check_state_at(ctx, it, sr_unk | sr_sep);
                auto const end_open_br_it = find_symbol(ctx, it, separator_fmt);
                check_state_at(ctx, end_open_br_it, sr_sep);
                string_view_type const open_br { it, end_open_br_it };

                it = end_open_br_it; ++it;
                check_state_at(ctx, it, sr_unk);
                auto const end_close_br_it = find_symbol(ctx, it, end_fmt);
                check_state_at(ctx, end_close_br_it, sr_end);
                string_view_type const close_br { it, end_close_br_it };

                m_underlying_formatter.set_brackets(open_br, close_br);

                ctx.advance_to(end_close_br_it);
                return m_underlying_formatter.parse(ctx);
            }
            else
            {
                //-----------------------------------------------------------------------------
                // brackets & separator

                check_state_at(ctx, it, sr_unk | sr_sep);
                auto const end_open_br_it = find_symbol(ctx, it, separator_fmt);
                check_state_at(ctx, end_open_br_it, sr_sep);
                string_view_type const open_br { it, end_open_br_it };

                it = end_open_br_it; ++it;
                check_state_at(ctx, it,  sr_unk | sr_sep);
                auto const end_sep_it = find_symbol(ctx, it, separator_fmt);
                check_state_at(ctx, end_sep_it, sr_sep);
                string_view_type const separator { it, end_sep_it };

                it = end_sep_it; ++it;
                check_state_at(ctx, it, sr_unk);
                auto const end_close_br_it = find_symbol(ctx, it, end_fmt);
                check_state_at(ctx, end_close_br_it, sr_end);
                string_view_type const close_br { it, end_close_br_it };

                m_underlying_formatter.set_brackets(open_br, close_br);
                m_underlying_formatter.set_separator(separator);

                ctx.advance_to(end_close_br_it);
                return m_underlying_formatter.parse(ctx);
            }
        }

    }

    template<typename FmtContext>
    constexpr auto format(cmn::io::fmt<T> const &t, FmtContext &ctx) const -> typename FmtContext::iterator
    {
        return m_underlying_formatter.format(t.m_t, ctx);
    }    
};

}