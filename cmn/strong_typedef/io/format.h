#pragma once

#include <format>
#include <concepts>

#include <cmn/io/format.h>

#include <cmn/strong_typedef/strong_typedef.h>

namespace cmn::io
{

template <typename StrongTypedef, typename Char>
struct strong_typedef_formatter:
    mix::out_to_stream
    <
          strong_typedef_formatter<StrongTypedef, Char>
        , StrongTypedef
        , Char
    >
{
    using out_to_stream_mix_type = mix::out_to_stream<strong_typedef_formatter, StrongTypedef, Char>;
    using ostream_type = typename out_to_stream_mix_type::ostream_type;

    int_fmt_t m_fmt_options = int_fmt_t::default_;

    /* CRTP override */ constexpr auto prepare_stream(ostream_type &ostr) const -> ostream_type &
    {
        return ostr << int_fmt(m_fmt_options);
    }

    template<typename ParseContext>
    constexpr auto parse(ParseContext &ctx) -> typename ParseContext::iterator
    {
        using namespace cmn::io;
        using enum int_fmt_t;

        using char_type = typename ParseContext::char_type;
        using symbols_type = symbols<char_type>;

        constexpr auto separator_fmt = cmn::to_char(symbols_type::colon);
        constexpr auto end_fmt = cmn::to_char(symbols_type::close_figure_bracket);

        auto it = ctx.begin();
        int_fmt_t fmt_options = empty;

        switch (check_state_at(ctx, it,  sr_unk | sr_sep | sr_end))  // NOLINT(clang-diagnostic-switch-enum)
        {
        case sr_sep: ++it; break;
        case sr_end: return it;
        case sr_unk: break;

        default:
            throw std::format_error("Unexpected");
        }

        while (sr_end != check_state_at(ctx, it,  sr_unk | sr_end))
        {
            switch (*it++)
            {
            case to_char(symbols_type::x): fmt_options |= hex; break;
            case to_char(symbols_type::octothorpe): fmt_options |= showbase; break;
            case to_char(symbols_type::a): fmt_options |= asm_; break;
            case to_char(symbols_type::l): fmt_options |= long_; break;
            case to_char(symbols_type::u): fmt_options |= uppercase; break;
            case to_char(symbols_type::whitespace): fmt_options |= sign; break;
            case to_char(symbols_type::plus): fmt_options |= forcesign; break;

            default:
                    throw std::format_error{ "Unexpected "};
            }
        }

        m_fmt_options = fmt_options;
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
