#pragma once

#include <limits>
#include <cstdint>

#include <cmn/meta/traits/format.h>
#include <cmn/util/symbols.h>

#include "print.h"

namespace cmn::io
{

template <>
struct format_traits<enum_tag>
{
    static constexpr enum_::io::print_t options = enum_::io::print_t::class_prefix | enum_::io::print_t::tail;
    static constexpr auto mask = std::numeric_limits<std::uintptr_t>::max(); // fill with 0b11111...


    template <typename Char, typename CharTraits>

    // ReSharper disable once CppRedundantQualifier
    static constexpr auto source_options = cmn::io::source_options
    {
        sym::open_square_bracket.value<Char, CharTraits>(),
        sym::close_square_bracket.value<Char, CharTraits>(),

        // separator between bitfield or combo elements during the output
        sym::ws.value<Char, CharTraits>()
    };
};

template <c::adapted_enum E>
struct format_traits<E>: format_traits<enum_tag>
{};

}
