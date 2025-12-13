#pragma once

#include <cstddef>
#include <limits>

#include <boost/type_traits/promote.hpp>

#include <cmn/meta/concepts.h>
#include <cmn/meta/macro.h>

#include <cmn/util/fixed_string.h>
#include <cmn/util/symbols.h>
#include <cmn/util/fixed_string.h>

#include "print.h"

namespace cmn
{

namespace enum_::io
{

template 
<
      typename Char
    , typename CharTtraits
    , std::size_t Open_
    , std::size_t Close_
    , std::size_t Delim_
>
struct source_options_t
{
    template <std::size_t N_>
    using string_t = basic_fixed_string<Char, N_, CharTtraits>;

    string_t<Open_> open;
    string_t<Close_> close;
    string_t<Delim_> delimiter;
};




template <c::adapted_enum E>
struct format_info
{

    E CMN_ANONYMOUS_VARIABLE();
    // TODO:
    print_t options;
    E bitfield_mask;

    template <typename Char, typename CharTraits>
    constexpr auto list_symbols()
    {
        return source_options_t
        {
            .open = sym::open_square_bracket.value<Char, CharTraits>(),
            .close = sym::close_square_bracket.value<Char, CharTraits>(),

            // separator between bitfield or combo elements during the output
            .delimiter = sym::nothing.value<Char, CharTraits>() 
        };
    }
};

}

namespace io
{

template <c::adapted_enum E>
struct format_traits<E>
{
    consteval auto operator()(E e)
    {
        using enum enum_::io::print_t;
        return enum_::io::format_info
        {
            e, 
            static_cast<enum_::io::print_t>(class_prefix | tail),
            // TODO: change to mask_type
            static_cast<E>(std::numeric_limits<unsigned long>::max()) // fill with 0b11111...
        };
    }
};

}


}
