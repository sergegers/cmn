#pragma once

#include <ostream>
#include <ranges>

#include <cmn/meta/concepts.h>
#include <cmn/range/io/manip.h>

#include "out.h"
#include "format_traits.h"

namespace cmn::range_::io
{

template <typename Char, typename CharTraits, std::ranges::range Range>
auto operator << (std::basic_ostream<Char, CharTraits> &ostr, Range rng) -> decltype(ostr)
    requires
        c::printable<std::ranges::range_value_t<Range>, Char, CharTraits>
    && !(c::c_array_of<Range, Char> && c::char_<Char>)   // skip string arrays
{
    using open_manip_type = basic_open_manip<Char, CharTraits>;
    using close_manip_type = basic_close_manip<Char, CharTraits>;
    using delimiter_manip_type = basic_delimiter_manip<Char, CharTraits>;
    using out_iterator_type = std::ostreambuf_iterator<Char, CharTraits>;

    sink_format_options const fmt_opt
    {
        // TODO: + manip
        .options = cmn::io::format_traits<Range>::options,
        .open = open_manip_type::value(ostr),
        .close = close_manip_type::value(ostr),
        .delimiter = delimiter_manip_type::value(ostr),
    };

    return out(std::move(rng), fmt_opt, out_iterator_type{ ostr }), ostr;
}

}
