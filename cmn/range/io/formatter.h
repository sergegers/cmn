#pragma once

#include <ranges>
#include <format>

#include <cmn/io/format.h>

#include "format_traits.h"

namespace std
{

template <std::ranges::input_range R, typename Char>
    // TODO:
    //requires formattable<std::remove_cvref_t<R>, Char>
struct formatter<cmn::io::list<R>, Char>: cmn::io::list_formatter<R, Char>
{
private:
    using inherited = cmn::io::list_formatter<R, Char>;

    static constexpr auto table_open_br = (cmn::sym::open_angle_bracket + cmn::sym::endl).value<Char>();
    static constexpr auto table_close_br = (cmn::sym::endl + cmn::sym::close_angle_bracket).value<Char>();
    static constexpr auto table_sep = cmn::sym::endl.value<Char>();
public:

    template<typename ParseContext>
    constexpr auto parse(ParseContext &ctx) -> cmn::io::context_iterator_t<ParseContext>
    {
        using namespace cmn;

        auto it = ctx.begin();
        switch (*it++)
        {
        case sym::t.as_char<Char>():  // t - table_out
        {
            this->m_underlying_formatter.set_brackets(table_open_br, table_close_br);
            this->m_underlying_formatter.set_separator(table_sep);
        }
        return it;

        case sym::c.as_char<Char>(): // c - compact_table_out
        {
            using traits_type = cmn::io::sink_format_traits<R, char>;

            this->m_underlying_formatter.set_brackets(traits_type::open, traits_type::close);
            this->m_underlying_formatter.set_separator(traits_type::delimiter);
        }
        return it;

        default:
            return inherited::parse(ctx);
        }
    }    
};

}
