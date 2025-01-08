#pragma once

#include <format>
#include <stdexcept>
#include <string_view>
#include <algorithm>
#include <utility>
#include <ostream>

#include <boost/type_traits/promote.hpp>

#include <cmn/meta/concepts.h>
#include <cmn/meta/type_traits.h>

#include <cmn/util/symbols.h>
#include <cmn/util/fixed_string.h>

#include <cmn/io/formatter.h>

#include <cmn/enum/traits.h>
#include <cmn/enum/io/manip.h>
#include <cmn/range/io/manip.h>

namespace cmn::io
{

template <c::adapted_enum E>
struct traits<E>
{
    static constexpr boost::promote_t<fmt_options_t> fmt_options = fo_brackers | fo_separator;
};

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
    using separator_manip_type  = cmn::enum_::io::basic_bitfield_separator_manip<Char>;
    using close_manip_type =cmn::enum_::io::basic_close_manip<Char>;

    open_manip_type         m_open          = open_manip_type { cmn::io::reset_ };
    separator_manip_type    m_separator     = separator_manip_type{ cmn::io::reset_ };
    close_manip_type        m_close         = close_manip_type { cmn::io::reset_ };

    constexpr auto set_brackets(string_view_type open_bracket, string_view_type close_bracket)
    {
        m_open = open_manip_type{ open_bracket };
        m_close = close_manip_type{ close_bracket };
    }

    constexpr auto set_separator(string_view_type separator) -> void
    {
        m_separator = separator_manip_type{ separator };
    }

    template<typename ParseContext>
    constexpr auto parse(ParseContext &ctx) -> typename ParseContext::iterator
    {
        return ctx.begin();
        //using enum cmn::enum_::io::detail::scroll_result_t;
        //using cmn::enum_::io::detail::parse_chunk;
        //using cmn::enum_::io::detail::check_state;

        //auto it = ctx.begin();
        //auto state = check_state(ctx, it, sr_unk | sr_next | sr_last);
        //if (state == sr_last) return it;            // {}

        ////-----------------------------------------------------------------------------
        //auto const open_end = parse_chunk(ctx, it, separator_fmt, end_fmt);
        //state = check_state(ctx, open_end, sr_next | sr_last);

        //m_open = open_manip_type{ manip_str_type { it, open_end } };
        //if (state == sr_last) return open_end;      // {0:[}

        //it = open_end; ++it;

        ////-----------------------------------------------------------------------------
        //auto const close_end = parse_chunk(ctx, it, end_fmt);
        //check_state(ctx, close_end, sr_last);

        //m_close = close_manip_type{ manip_str_type { it, close_end } };
        //return close_end;                           // {0:[:]}
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
