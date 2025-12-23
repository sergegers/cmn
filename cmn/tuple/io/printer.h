#pragma once

#include <ranges>
#include <ostream>

#if __has_include(<boost/fusion/concepts.hpp>)
#   include <boost/fusion/concepts.hpp>
#else
#   include <cmn/meta/boost/fusion/concepts.hpp>
#endif

#include "out.h"
#include "format_traits.h"

namespace cmn::tuple_::io
{

template <typename Char, typename CharTraits, boost::c::fus_sequence Seq>
    requires !std::ranges::range<Seq>   // skip ranges
auto operator << (std::basic_ostream<Char, CharTraits>& ostr, Seq const &seq) -> decltype(ostr)
{
    using open_manip_type = basic_open_manip<Char, CharTraits>;
    using close_manip_type = basic_close_manip<Char, CharTraits>;
    using delimiter_manip_type = basic_delimiter_manip<Char, CharTraits>;
    using out_iterator_type = std::ostreambuf_iterator<Char, CharTraits>;

    sink_format_options const fmt_opt
    {
        // TODO: + manip
        .options = cmn::io::format_traits<Seq>::options,
        .open = open_manip_type::value(ostr),
        .close = close_manip_type::value(ostr),
        .delimiter = delimiter_manip_type::value(ostr),
    };

    return out(seq, fmt_opt, out_iterator_type{ ostr }), ostr;
}

//-----------------------------------------------------------------------------
template <typename Char, typename CharTraits, boost::c::fus_sequence Seq>
    requires !std::ranges::range<Seq>   // skip ranges
auto operator >> (std::basic_istream<Char, CharTraits>& istr, Seq& seq) -> decltype(istr)
{
    throw not_implemented();
}

}
