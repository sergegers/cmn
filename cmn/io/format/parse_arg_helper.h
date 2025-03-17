#pragma once

#include <format>
#include <utility>
#include <string_view>
#include <ranges>
#include <optional>

#include <boost/type_traits/promote.hpp>

#include <cmn/util/feature.h>

#include "util.h"

namespace cmn::io
{

///////////////////////////////////////////////////////////////////////////////
//
// Parse colon (:) delimited format string, scroll parser context and
// return range between colon (:) and colon (:) or close bracket (})
//
///////////////////////////////////////////////////////////////////////////////
template <typename Char = char>
struct parse_arg_helper
{
    using char_type = Char;
    using string_view_type = std::basic_string_view<Char>;
private:
    static constexpr auto scroll_to_sep = scroll_to_sep_<char_type>;
    static constexpr auto scroll_to_close = scroll_to_close_<char_type>;

    // set iterator to the beginning of escaped symbol sequence
    template <typename Iterator>
    [[nodiscard]] static constexpr auto get_begin_it(boost::promote_t<scroll_result_t> sr,  Iterator it) -> Iterator
    {
        return has_feature(sr, sr_esc)? ++it: it;
    }
public:
    [[nodiscard]] static constexpr auto parse_arg(auto &ctx)
    {
        auto const it = ctx.begin();

        auto const sep_sr = check_state_at(ctx, it,  sr_sym | sr_sep);
        auto const end_sep_it = check_find_symbol(ctx, it, scroll_to_sep);

        ctx.advance_to(end_sep_it);

        return std::ranges::subrange{ get_begin_it(sep_sr, it), end_sep_it };
        
    }

    [[nodiscard]] static constexpr auto parse_last_arg(auto &ctx)
    {
        auto const it = ctx.begin();

        auto const sep_sr = check_state_at(ctx, it,  sr_sym);
        auto const end_sep_it = check_find_symbol(ctx, it, scroll_to_close);

        ctx.advance_to(end_sep_it);

        return std::ranges::subrange { get_begin_it(sep_sr, it), end_sep_it };
    }

    template <typename ParseContext>
    using try_parser_arg_result_type = std::optional<std::ranges::subrange<context_iterator_t<ParseContext>>>;

    template <typename ParseContext>
    [[nodiscard]] static constexpr auto try_parse_arg(ParseContext &ctx) noexcept
        -> try_parser_arg_result_type<ParseContext>
    {
        auto const it = ctx.begin();
        auto const sep_sr = decode_state_at(ctx, it);
        if (!has_any_feature(sep_sr, sr_sym, sr_sep)) return std::nullopt;

        auto const end_sep_it = find_symbol(ctx, it, scroll_to_sep, scroll_to_close);
        ctx.advance_to(end_sep_it);
        if (end_sep_it == ctx.end()) return std::nullopt;

        return std::ranges::subrange{ get_begin_it(sep_sr, it), end_sep_it };
        
    }

    static constexpr auto parse_no_arg(auto &ctx) -> void
    {
        auto const it = ctx.begin();
        std::ignore = check_state_at(ctx, it, sr_close);
    }

    //-----------------------------------------------------------------------------
    static constexpr auto next(auto &ctx) -> void
    {
        auto it = ctx.begin();
        ctx.advance_to(++it);
    }
};

}
