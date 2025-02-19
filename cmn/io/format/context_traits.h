#pragma once

#include <utility>

#include <boost/type_traits/promote.hpp>

#include <cmn/util/symbols.h>

namespace cmn::io
{

enum scroll_result_t: char
{
    sr_close    = 0x0,      // end of format string, iterator is pointed to '}'
    sr_sep      = 0x1,      // next formatting item, iterator is pointed to separator ':'
    sr_eos      = 0x2,      // end of format string, iterator in equal to the end iterator
    sr_sym      = 0x4,      // ordinary symbol
    sr_esc      = 0x8,      // escape flag

    sr_naked    = sr_close | sr_sep | sr_eos | sr_sym
};

//-----------------------------------------------------------------------------
template <typename Char> using scroll_to = std::pair<Char, scroll_result_t>;

template <typename Char> constexpr scroll_to<Char> scroll_to_sep_ = scroll_to{ cmn::to_char(symbols<Char>::colon), sr_sep };
template <typename Char> constexpr scroll_to<Char> scroll_to_close_ = scroll_to{ cmn::to_char(symbols<Char>::close_figure_bracket), sr_close };

//-----------------------------------------------------------------------------
template <typename Iterator> using scroll_pos = std::pair<Iterator, boost::promote_t<scroll_result_t>>;

//-----------------------------------------------------------------------------
//
// parse context and format context helpers
//
//-----------------------------------------------------------------------------
template <typename Context> using context_iterator_t = typename Context::iterator;
template <typename Context> using context_char_t = typename Context::char_type;

template <typename Context> using context_scroll_to_t = scroll_to<context_char_t<Context>>;
template <typename Context> using context_scroll_pos_t = scroll_pos<context_iterator_t<Context>>;

}
