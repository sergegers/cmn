#pragma once

#include <concepts>
#include <type_traits>
#include <format>
#include <string_view>
#include <iterator>

#include <boost/type_traits/promote.hpp>

#include <cmn/meta/concepts.h>
#include <cmn/util/feature.h>

#include <cmn/io/format/mix/parse_arg.h>

namespace cmn::io
{

enum list_options_t
{
    lo_empty        = 0x0,
    lo_brackers     = 0x1,
    lo_separator    = 0x2
};

template <typename T>
struct traits
{
    static constexpr boost::promote_t<list_options_t> fmt_options = lo_empty;
};

//-----------------------------------------------------------------------------
template <typename T>
struct list
{
    T m_t;

    template <c::explicitly_convertible_to<T> Arg>
    constexpr list(Arg arg) noexcept: m_t{ static_cast<T>(arg) } {}
};

template <typename T>
struct list<T const &>
{
    T const &m_t;

    constexpr list(T const &arg) noexcept: m_t{ arg } {}
};

template <typename T> list(T const &) noexcept -> list<T const &>;

///////////////////////////////////////////////////////////////////////////////
template
<
      typename T
    , typename Char
>
    requires std::formattable<std::remove_cvref_t<T>, Char>
struct list_formatter: mix::parse_arg<Char>
{
private:
    using inherited = mix::parse_arg<Char>;
protected:
    using underlying_formatting_type = std::remove_cvref_t<T>;
    using underlying_formatter_type = std::formatter<underlying_formatting_type, Char>;
    using string_view_type = std::basic_string_view<Char>;

    underlying_formatter_type m_underlying_formatter;
public:
    template<typename ParseContext>
    constexpr auto parse(ParseContext &ctx) -> context_iterator_t<ParseContext>
    {
        using namespace cmn::io;

        using char_type = context_char_t<ParseContext>;
        using iterator_type = context_iterator_t<ParseContext>;
        using traits_type = traits<underlying_formatting_type>;

        constexpr auto scroll_to_sep = scroll_to_sep_<char_type>;
        constexpr auto scroll_to_close = scroll_to_close_<char_type>;

        // set iterator to the beginning of escaped symbol sequence
        constexpr auto get_begin_it_ = [](boost::promote_t<scroll_result_t> sr,  iterator_type it) constexpr -> iterator_type
        {
            return cmn::has_feature(sr, sr_esc)? ++it: it;
        };

        constexpr bool has_brackets = cmn::has_feature(traits_type::fmt_options, lo_brackers);
        constexpr bool has_separator = cmn::has_feature(traits_type::fmt_options, lo_separator);

        if constexpr (!has_brackets)
        {
            if constexpr (!has_separator)
            {
                //-----------------------------------------------------------------------------
                // no brackets, no separator
                
                inherited::parse_no_arg(ctx);
                return m_underlying_formatter.parse(ctx);
            }
            else
            {
                //-----------------------------------------------------------------------------
                // separator only

                string_view_type const separator { inherited::parse_last_arg(ctx) };
                m_underlying_formatter.set_separator(separator);

                return m_underlying_formatter.parse(ctx);
            }
        }
        else
        {
            if constexpr (!has_separator)
            {
                //-----------------------------------------------------------------------------
                // brackets only

                string_view_type const open_br { inherited::parse_arg_(ctx) };

                inherited::next(ctx);
                string_view_type const close_br { inherited::parse_last_arg(ctx) };

                m_underlying_formatter.set_brackets(open_br, close_br);
                return m_underlying_formatter.parse(ctx);
            }
            else
            {
                //-----------------------------------------------------------------------------
                // brackets & separator

                string_view_type const open_br{ inherited::parse_arg_(ctx) };

                inherited::next(ctx);
                string_view_type const separator { inherited::parse_arg_(ctx) };

                inherited::next(ctx);
                string_view_type const close_br { inherited::parse_last_arg(ctx) };

                m_underlying_formatter.set_brackets(open_br, close_br);
                m_underlying_formatter.set_separator(separator);

                return m_underlying_formatter.parse(ctx);
            }
        }

    }

    template<typename FmtContext>
    constexpr auto format(list<T> const &t, FmtContext &ctx) const -> typename FmtContext::iterator
    {
        return m_underlying_formatter.format(t.m_t, ctx);
    }    
};

}

namespace std
{

template <typename T, typename Char>
    requires formattable<std::remove_cvref_t<T>, Char>
struct formatter<cmn::io::list<T>, Char>: cmn::io::list_formatter<T, Char> {};

}