#pragma once

#include <format>
#include <stdexcept>
#include <string_view>
#include <sstream>
#include <algorithm>
#include <utility>

#include <boost/type_traits/promote.hpp>

#include <cmn/meta/concepts.h>
#include <cmn/meta/type_traits.h>
#include <cmn/meta/symbols.h>

#include <cmn/util/fixed_string.h>
#include <cmn/util/feature.h>

#include <cmn/enum/traits.h>
#include <cmn/enum/io/manip.h>

namespace cmn::enum_::io::detail
{

enum scroll_result_t
{
    sr_last = 0x0,      // end of format string, iterator is pointed to '}'
    sr_next = 0x1,      // next formatting item, iterator is pointed to ','
    sr_eos  = 0x2,      // end of format string, iterator in equal to the end iterator
    sr_unk  = 0x4       // state is unknown
};

//-----------------------------------------------------------------------------
template <typename ParseContext>
constexpr auto check_state(ParseContext &ctx, typename ParseContext::iterator it, 
    boost::promote_t<scroll_result_t> expected_states)  -> scroll_result_t
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
        case end_fmt: state = sr_last; break;
        case separator_fmt: state = sr_next; break;
        default: state = sr_unk;
        }

    switch (state)
    {
    case sr_last:
    {
        if (!has_feature(expected_states, state))
            throw std::format_error("Invalid format string for adapted enum. Additional arguments");
    }
        break;

    case sr_next:
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
constexpr auto parse_chunk(ParseContext &ctx, typename ParseContext::iterator it, typename ParseContext::char_type tgt, 
    Tgts... tgts) noexcept -> typename ParseContext::iterator
{
    typename ParseContext::iterator res;
    std::ignore = ((ctx.end() != (res = find(it, ctx.end(), tgt))) || ... || (ctx.end() != (res = find(it, ctx.end(), tgts))));

    return res;
}

template <typename ParseContext>
constexpr auto parse_chunk(ParseContext &ctx, typename ParseContext::iterator it) noexcept -> typename ParseContext::iterator
{
    using char_type = typename ParseContext::char_type;
    using symbols_type = symbols<char_type>;

    static constexpr auto separator_fmt = cmn::to_char(symbols_type::colon);
    return find(it, ctx.end(), separator_fmt);
}

}

namespace std
{

template <cmn::c::adapted_enum E, typename Char>
struct formatter<E, Char>
{
    using symbols_type = cmn::symbols<Char>;
    using string_view_type = basic_string_view<Char>;

    static constexpr auto max_slot_size = sizeof(long) / sizeof(Char);
    using manip_str_type = cmn::basic_fixed_string<Char, max_slot_size>;

    // custom format {0:[:]}
    static constexpr auto separator_fmt = cmn::to_char(symbols_type::colon);
    static constexpr auto end_fmt = cmn::to_char(symbols_type::close_figure_bracket);

    using open_manip_type = cmn::enum_::io::basic_open_manip<Char>;
    using close_manip_type =cmn::enum_::io::basic_close_manip<Char>;

    open_manip_type     m_open      = open_manip_type { cmn::io::reset_ };
    close_manip_type    m_close     = close_manip_type { cmn::io::reset_ };

    template<typename ParseContext>
    constexpr auto parse(ParseContext &ctx) -> typename ParseContext::iterator
    {
        using enum cmn::enum_::io::detail::scroll_result_t;
        using cmn::enum_::io::detail::parse_chunk;
        using cmn::enum_::io::detail::check_state;

        auto it = ctx.begin();
        auto state = check_state(ctx, it, sr_unk | sr_next | sr_last);
        if (state == sr_last) return it;            // {}

        //-----------------------------------------------------------------------------
        auto const open_end = parse_chunk(ctx, it, separator_fmt, end_fmt);
        state = check_state(ctx, open_end, sr_next | sr_last);

        m_open = open_manip_type{ manip_str_type { it, open_end } };
        if (state == sr_last) return open_end;      // {0:[}

        it = open_end; ++it;

        //-----------------------------------------------------------------------------
        auto const close_end = parse_chunk(ctx, it, end_fmt);
        check_state(ctx, close_end, sr_last);

        m_close = close_manip_type{ manip_str_type { it, close_end } };
        return close_end;                           // {0:[:]}
    }
 
    template<typename FmtContext>
    constexpr auto format(E en, FmtContext &ctx) const -> typename FmtContext::iterator
    {
        using ostring_stream_type = basic_ostringstream<Char>;
        using namespace cmn::enum_;

        ostring_stream_type ostr;
        ostr << m_open << m_close;

        io::printer<E, kind_v<E>>{ en, cmn::int_<kind_v<E>>{} }.print(ostr);
 
        return ranges::copy(std::move(ostr).str(), ctx.out()).out;
    }    
};

//-----------------------------------------------------------------------------
template <cmn::c::adapted_enum E, typename Char>
    requires (cmn::in(cmn::enum_::kind_v<E>, cmn::enum_::kind_t::bitfield, cmn::enum_::kind_t::combo))
struct formatter<E, Char>
{
    using symbols_type = cmn::symbols<Char>;
    using string_view_type = basic_string_view<Char>;

    static constexpr auto max_slot_size = sizeof(long) / sizeof(Char);
    using manip_str_type = cmn::basic_fixed_string<Char, max_slot_size>;

    // custom format {0:[: :]}
    static constexpr auto separator_fmt = cmn::to_char(symbols_type::colon);
    static constexpr auto end_fmt = cmn::to_char(symbols_type::close_figure_bracket);

    using open_manip_type       = cmn::enum_::io::basic_open_manip<Char>;
    using separator_manip_type  = cmn::enum_::io::basic_bitfield_separator_manip<Char>;
    using close_manip_type      = cmn::enum_::io::basic_close_manip<Char>;

    open_manip_type         m_open          = open_manip_type { cmn::io::reset_ };
    separator_manip_type    m_separator     = separator_manip_type{ cmn::io::reset_ };
    close_manip_type        m_close         = close_manip_type { cmn::io::reset_ };

    template<typename ParseContext>
    constexpr auto parse(ParseContext &ctx) -> typename ParseContext::iterator
    {
        using enum cmn::enum_::io::detail::scroll_result_t;
        using cmn::enum_::io::detail::parse_chunk;
        using cmn::enum_::io::detail::check_state;

        auto it = ctx.begin();
        auto state = check_state(ctx, it, sr_unk | sr_next | sr_last);
        if (state == sr_last) return it;            // {}

        //-----------------------------------------------------------------------------
        auto const open_end = parse_chunk(ctx, it, separator_fmt, end_fmt);
        state = check_state(ctx, open_end, sr_next | sr_last);

        m_open = open_manip_type{ manip_str_type { it, open_end } };
        if (state == sr_last) return open_end;      // {0:[}

        it = open_end; ++it;

        //-----------------------------------------------------------------------------
        auto const separator_end = parse_chunk(ctx, it, separator_fmt, end_fmt);
        state = check_state(ctx, separator_end, sr_next | sr_last);

        m_separator = separator_manip_type { manip_str_type { it, separator_end } };
        if (state == sr_last) return separator_end;     // {0:[: }

        it = separator_end; ++ it;

        //-----------------------------------------------------------------------------
        auto const close_end = parse_chunk(ctx, it, end_fmt);
        check_state(ctx, close_end, sr_last);

        m_close = close_manip_type{ manip_str_type { it, close_end } };
        return close_end;                           // {0:[: :]}
    }
 
    template<typename FmtContext>
    constexpr auto format(E en, FmtContext &ctx) const -> typename FmtContext::iterator
    {
        using ostring_stream_type = basic_ostringstream<Char>;
        using namespace cmn::enum_;

        ostring_stream_type ostr;
        ostr << m_open << m_separator << m_close;
        
        io::printer<E, kind_v<E>>{ en, cmn::int_<kind_v<E>>{} }.print(ostr);
 
        return ranges::copy(std::move(ostr).str(), ctx.out()).out;
    }    
};

}
