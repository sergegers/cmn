#pragma once

#include <format>
#include <concepts>

#include <cmn/io/format.h>
#include <cmn/enum/feature.h>

#include <cmn/strong_typedef/strong_typedef.h>

#include "int_fmt.h"
#include "manip.h"

namespace cmn::io
{

template <typename T, typename Char>
struct strong_typedef_formatter: mix::out_to_stream<T, Char>
{
    using out_to_stream_mix_type = mix::out_to_stream<T, Char>;
    using ostream_type = out_to_stream_mix_type::ostream_type;

    int_fmt_t m_fmt_opt = int_fmt_t::default_;

    /* CRTP override */ constexpr auto prepare_stream(ostream_type &ostr) const -> ostream_type &
    {
        return ostr << int_fmt(m_fmt_opt);
    }

    template<typename ParseContext>
    constexpr auto parse(ParseContext &ctx) -> ParseContext::iterator
    {
        using namespace cmn::io;
        using enum int_fmt_t;

        using char_type = context_char_t<ParseContext>;

        constexpr auto separator_fmt = sym::colon.as_char<Char>();
        constexpr auto end_fmt = sym::close_figure_bracket.as_char<Char>();

        auto it = ctx.begin();

        switch (check_state_at(ctx, it, sr_sym | sr_sep | sr_close))
        {
        case sr_sep: ++it; break;
        case sr_close: return it;
        case sr_sym: break;

        default:
            throw std::format_error("Unexpected");
        }

        while (sr_close != check_state_at(ctx, it,  sr_sym | sr_close))
        {
            switch (auto ch = *it++)
            {
            case sym::octothorpe.as_char<Char>(): m_fmt_opt = set_features(m_fmt_opt, showbase, lowercase); break;
            case sym::x.as_char<Char>(): m_fmt_opt = set_features(m_fmt_opt, hex, c, nosign); break;
            case sym::a.as_char<Char>(): m_fmt_opt = set_features(m_fmt_opt, long_asm_up_hex); break;
            case sym::s.as_char<Char>(): m_fmt_opt = set_features(m_fmt_opt, short_); break;
            case sym::u.as_char<Char>(): m_fmt_opt = set_features(m_fmt_opt, uppercase); break;

            default:
                    throw std::format_error{ std::format("Unexpected formatting symbol [{}]", ch)};
            }
        }

        return it;
    }
};

}

namespace std
{

template <typename U, std::integral T, T Default_, typename Char>
struct formatter<cmn::strong_typedef_impl<U, T, Default_>, Char>: cmn::io::strong_typedef_formatter<U, Char> {};

template <std::integral T, typename Tag, T Default_, typename Char>
struct formatter<cmn::strong_typedef<T, Tag, Default_>, Char>:
    cmn::io::strong_typedef_formatter<cmn::strong_typedef<T, Tag, Default_>, Char> {};

}
