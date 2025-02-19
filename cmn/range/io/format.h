#pragma once

#include <ranges>
#include <format>

#include <boost/type_traits/promote.hpp>

#include <cmn/util/symbols.h>
#include <cmn/io/format.h>

namespace cmn
{

template <std::ranges::input_range R>
struct io::traits<R>
{
    static constexpr boost::promote_t<list_options_t> fmt_options = lo_brackers | lo_separator;
};

namespace range_::io
{

template <typename T>
struct list: cmn::io::list<T>
{
    using cmn::io::list<T>::list;
};

template <typename T> list(T const &) noexcept -> list<T const &>;

}

}

namespace std
{

template <std::ranges::input_range R, typename Char>
    // TODO:
    //requires formattable<std::remove_cvref_t<R>, Char>
struct formatter<cmn::range_::io::list<R>, Char>: cmn::io::list_formatter<R, Char>
{
private:
    using inherited = cmn::io::list_formatter<R, Char>;
    using symbols_type = cmn::symbols<Char>;

    static constexpr auto table_open_br = symbols_type::open_angle_bracket + symbols_type::endl;
    static constexpr auto table_close_br = symbols_type::endl + symbols_type::close_angle_bracket;
    static constexpr auto table_sep = symbols_type::endl;

    static constexpr auto compact_table_open_br = symbols_type::open_angle_bracket;
    static constexpr auto compact_table_close_br = symbols_type::close_angle_bracket;
    static constexpr auto compact_table_sep = symbols_type::comma + symbols_type::whitespace;
public:

    template<typename ParseContext>
    constexpr auto parse(ParseContext &ctx) -> cmn::io::context_iterator_t<ParseContext>
    {
        using namespace cmn::io;


        auto it = ctx.begin();
        switch (*it++)
        {
        case to_char(symbols_type::t):  // t - table_out
        {
            this->m_underlying_formatter.set_brackets(table_open_br, table_close_br);
            this->m_underlying_formatter.set_separator(table_sep);
        }
        return it;

        case to_char(symbols_type::c): // c - compact_table_out
        {
            this->m_underlying_formatter.set_brackets(compact_table_open_br, compact_table_close_br);
            this->m_underlying_formatter.set_separator(compact_table_sep);
        }
        return it;

        default:
            return inherited::parse(ctx);
        }
    }    
};

}
