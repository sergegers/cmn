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

#include <cmn/io/format.h>

#include <cmn/enum/traits.h>
#include <cmn/enum/io/manip.h>
#include <cmn/range/io/manip.h>
#include <cmn/enum/op.h>

namespace cmn::io
{

template <c::adapted_enum E>
struct traits<E>
{
    static constexpr boost::promote_t<list_options_t> fmt_options = lo_brackers | lo_separator;
};

}

namespace std
{

template <cmn::c::adapted_enum E, typename Char>
struct formatter<E, Char>: 
      cmn::io::mix::out_to_stream<E, Char>
    , cmn::io::mix::skip_parse<E, Char>
{
    using ostream_type = cmn::io::mix::out_to_stream<E, Char>::ostream_type;
    using string_view_type = basic_string_view<Char>;

    static constexpr auto max_slot_size = sizeof(long) / sizeof(Char);
    using manip_str_type = cmn::basic_fixed_string<Char, max_slot_size>;

    // custom format {0:[:]}
    static constexpr auto separator_fmt = cmn::sym::colon.as_char<Char>();
    static constexpr auto end_fmt = cmn::sym::close_figure_bracket.as_char<Char>();

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

    constexpr auto prepare_stream(ostream_type &ostr) const -> ostream_type &
    {
        return ostr << m_open << m_separator << m_close;
    }
};

}
