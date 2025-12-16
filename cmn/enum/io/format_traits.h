#pragma once

#include <limits>
#include <cstdint>

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
struct sink_format_options
{
    using char_type = Char;
    using char_traits_type = CharTraits;
    using string_type = std::basic_string<Char, CharTraits>;

    print_t options = print_t::class_prefix | print_t::tail;
    std::uintptr_t mask = std::numeric_limits<std::uintptr_t>::max(); // fill with 0b11111...
    string_type open = sym::open_square_bracket.as_string<Char, CharTraits>();
    string_type close = sym::close_square_bracket.as_string<Char, CharTraits>();
    // separator between bitfield or combo elements during the output
    string_type delimiter = sym::ws.as_string<Char, CharTraits>();
    string_type scope_resolution = sym::scope_resolution.as_string<Char, CharTraits>();
};

}

///////////////////////////////////////////////////////////////////////////////
namespace io
{

template <>
struct format_traits<enum_::io::tag>
{
    static constexpr enum_::io::print_t options = enum_::io::print_t::class_prefix | enum_::io::print_t::tail;
    static constexpr auto mask = std::numeric_limits<std::uintptr_t>::max(); // fill with 0b11111...
};

template <c::adapted_enum E>
struct format_traits<E>: format_traits<enum_::io::tag>
{};

///////////////////////////////////////////////////////////////////////////////

template 
<
      typename Char
    , typename CharTraits
>
struct sink_format_traits<enum_::io::tag, Char, CharTraits>: format_traits<enum_::io::tag>
{
    using char_type = Char;
    using char_traits_type = CharTraits;

    static constexpr auto open = sym::open_square_bracket.value<Char, CharTraits>();
    static constexpr auto close = sym::close_square_bracket.value<Char, CharTraits>();
    // separator between bitfield or combo elements during the output
    static constexpr auto delimiter = sym::ws.value<Char, CharTraits>();
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