#pragma once

#include <string>
#include <variant>

namespace cmn::enum_::io
{

template <typename Char, typename CharTraits>
struct basic_brackets
{
    using string_type = std::basic_string<Char, CharTraits>;

    string_type open;
    string_type close;
};

template <typename Char, typename CharTraits>
struct basic_delimiter
{
    using string_type = std::basic_string<Char, CharTraits>;

    string_type delimiter;
};

template <typename Char, typename CharTraits>
struct basic_brackets_delimiter
{
    using string_type = std::basic_string<Char, CharTraits>;

    string_type open;
    string_type close;
    string_type delimiter;
};

template <typename Char, typename CharTraits>
using basic_format_symbols = std::variant
<
      basic_brackets<Char, CharTraits>
    , basic_delimiter<Char, CharTraits>
    , basic_brackets_delimiter<Char, CharTraits>
>;

}
