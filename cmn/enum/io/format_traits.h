#pragma once

#include <limits>
#include <cstddef>
#include <string>

#include <cmn/fwd.h>
#include <cmn/util/symbols.h>

#include "print.h"

namespace cmn
{

namespace enum_::io
{

struct tag;


template
<
      typename Char
    , typename CharTraits = std::char_traits<Char>
>
struct basic_sink_format_options
{
    using string_type = std::basic_string<Char, CharTraits>;
    //-----------------------------------------------------------------------------
    //
    // concept list_sink_format_options
    //
    using options_type = print_t;
    using char_type = Char;
    using char_traits_type = CharTraits;

    options_type options = []
    {
        using enum print_t;
        return brackets | delimiter | class_prefix | tail;
    }();

    string_type open = sym::open_square_bracket.as_string<Char, CharTraits>();
    string_type close = sym::close_square_bracket.as_string<Char, CharTraits>();
    // separator between bitfield or combo elements during the output
    string_type delimiter = sym::ws.as_string<Char, CharTraits>();
    //
    //-----------------------------------------------------------------------------

    std::uintptr_t mask = std::numeric_limits<std::uintptr_t>::max(); // fill with 0b11111...
    string_type scope_resolution = sym::scope_resolution.as_string<Char, CharTraits>();
};

using sink_format_options = basic_sink_format_options<char>;
using wsink_format_options = basic_sink_format_options<wchar_t>;

}

///////////////////////////////////////////////////////////////////////////////
namespace io
{

template <>
struct format_traits<enum_::io::tag>
{
    //-----------------------------------------------------------------------------
    //
    // concept format_options
    //
    using options_type = enum_::io::print_t;
    static constexpr options_type options = options_type::brackets | options_type::delimiter |
        options_type::class_prefix | options_type::tail;
    //
    //-----------------------------------------------------------------------------
    static constexpr auto mask = std::numeric_limits<std::uintptr_t>::max(); // fill with 0b11111...
};

template <c::adapted_enum E>
struct format_traits<E>: format_traits<enum_::io::tag> {};

///////////////////////////////////////////////////////////////////////////////

template 
<
      typename Char
    , typename CharTraits
>
struct sink_format_traits<enum_::io::tag, Char, CharTraits>: format_traits<enum_::io::tag>
{
    //-----------------------------------------------------------------------------
    //
    // concept list_sink_format_options
    //
    using char_type = Char;
    using char_traits_type = CharTraits;

    static constexpr auto open = sym::open_square_bracket.value<Char, CharTraits>();
    static constexpr auto close = sym::close_square_bracket.value<Char, CharTraits>();
    // separator between bitfield or combo elements during the output
    static constexpr auto delimiter = sym::ws.value<Char, CharTraits>();
    //
    //-----------------------------------------------------------------------------

    static constexpr auto scope_resolution = sym::scope_resolution.value<Char, CharTraits>();
};

template 
<
      c::adapted_enum E
    , typename Char
    , typename CharTraits
>
struct sink_format_traits<E, Char, CharTraits>: sink_format_traits<enum_::io::tag, Char, CharTraits> {};

}

}