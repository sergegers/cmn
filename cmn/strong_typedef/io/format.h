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
        m_fmt_options = empty;

        switch (check_state_at(ctx, it,  sr_sym | sr_sep | sr_close))  // NOLINT(clang-diagnostic-switch-enum)
        {
        case sr_sep: ++it; break;
        case sr_close: return it;
        case sr_sym: break;

        default:
            throw std::format_error("Unexpected");
        }

        while (sr_close != check_state_at(ctx, it,  sr_sym | sr_close))
        {
            switch (*it++)
            {
            case to_char(symbols_type::octothorpe): m_fmt_options |= showbase | lowercase; break;
            case to_char(symbols_type::x): m_fmt_options |= hex | c | nosign; break;
            case to_char(symbols_type::a): m_fmt_options |= long_asm_up_hex; break;
            case to_char(symbols_type::u): m_fmt_options |= uppercase; break;

            default:
                    throw std::format_error{ "Unexpected "};
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
