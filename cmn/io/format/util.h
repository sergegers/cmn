#pragma once

#include <algorithm>
#include <format>
#include <utility>

#include <boost/type_traits/promote.hpp>

#include <cmn/util/symbols.h>
#include <cmn/io/format/context_traits.h>

namespace cmn::io
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

    constexpr auto separator_fmt = sym::colon.as_char<char_type>();
    constexpr auto open_fmt = sym::open_figure_bracket.as_char< char_type>();
    constexpr auto close_fmt = sym::close_figure_bracket.as_char< char_type>();

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

//-----------------------------------------------------------------------------
//
// drop escape flag
//
[[nodiscard]] constexpr auto naked(boost::promote_t<scroll_result_t> sr) -> scroll_result_t
{
    return get_feature(sr, sr_naked);
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
    auto const state = decode_state_at(ctx, it);    
    switch (auto const naked_state = naked(state); naked_state)
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
[[nodiscard]] constexpr auto find_tgt
(
      ParseContext const &ctx
    , context_iterator_t<ParseContext> it
    , context_scroll_to_t<ParseContext> tgt
) noexcept
    -> context_iterator_t<ParseContext>
{
    auto const [sym, sr] = tgt;
    auto res_it = std::find(it, ctx.end(), sym);

    auto state = decode_state_at(ctx, res_it);
    if (has_feature(state, sr_esc))
    {
        // scroll escape sequence
        ++res_it; ++res_it;

        return find_tgt(ctx, res_it, tgt);
    }

    // verify target
    auto const real_sr = get_feature(decode_state_at(ctx, res_it), sr_naked);
    return (real_sr == sr)? res_it: ctx.end();
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
[[nodiscard]] constexpr auto find_symbol
(
      ParseContext const &ctx
    , context_iterator_t<ParseContext> it
    , context_scroll_to_t<ParseContext> tgt
    , Tgts... tgts
) noexcept
    -> context_iterator_t<ParseContext>

    requires (std::same_as<Tgts, context_scroll_to_t<ParseContext>> && ...)
{
    using iterator_type = context_iterator_t<ParseContext>;

    iterator_type res;
    std::ignore = 
    (
        (ctx.end() != (res = detail::find_tgt(ctx, it, tgt)))
        || ... || 
        (ctx.end() != (res = detail::find_tgt(ctx, it, tgts)))
    );

    return res;
}

//-----------------------------------------------------------------------------
//
// return iterator pointed to the target or throw
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
    using iterator_type = context_iterator_t<ParseContext>;

    iterator_type res;
    std::ignore = 
    (
        (ctx.end() != (res = detail::find_tgt(ctx, it, tgt)))
        || ... || 
        (ctx.end() != (res = detail::find_tgt(ctx, it, tgts)))
    );

    if (ctx.end() == it) throw std::format_error("Targets are not found");

    return res;
}

}
