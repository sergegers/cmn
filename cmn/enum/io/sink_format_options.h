#pragma once

#include <string>
#include <limits>
#include <cstddef>

#include <cmn/util/symbols.h>

#include "print.h"
#include "format_symbols.h"

namespace cmn::enum_::io
{

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

    using format_symbols_type = basic_format_symbols<Char, CharTraits>;

    options_type options = []
    {
        using enum print_t;
        return brackets | delimiter | class_prefix | tail;
    }();

    format_symbols_type format_fymbols;
    //
    //-----------------------------------------------------------------------------

    std::uintptr_t mask = std::numeric_limits<std::uintptr_t>::max(); // fill with 0b11111...
    string_type scope_resolution = sym::scope_resolution.as_string<Char, CharTraits>();
};

using sink_format_options = basic_sink_format_options<char>;
using wsink_format_options = basic_sink_format_options<wchar_t>;

///////////////////////////////////////////////////////////////////////////////
template </*c::adapted_enum E, */typename Char, typename CharTraits>
constexpr auto get_sink_format_options
(
      print_t opt
    , std::basic_string<Char, CharTraits> const &open
    , std::basic_string<Char, CharTraits> const& close
    , std::basic_string<Char, CharTraits> const& delimiter_
)
{
    using enum print_t;
    using string_type = std::basic_string<Char, CharTraits>;
    using format_symbols_type = basic_format_symbols<Char, CharTraits>;

    auto format_symbols = []
    (
        string_type const& open
        , string_type const& close
        , string_type const& delimiter_

    ) -> format_symbols_type
    {

    };
    
    using brackets_type = basic_brackets<Char, CharTraits>;
    using delimiter_type = basic_delimiter<Char, CharTraits>;
    using brackets_delimiter_type = basic_brackets_delimiter<Char, CharTraits>;

    return basic_sink_format_options
    {
        .options = opt,
        .open = has_feature(opt, brackets) ? open : sym::nothing.as_string<Char, CharTraits>(),
        .close = has_feature(opt, brackets) ? close : sym::nothing.as_string<Char, CharTraits>(),
        .delimiter = has_feature(opt, delimiter) ? delimiter_ : sym::nothing.as_string<Char, CharTraits>()
    };
}

}
