#pragma once

#include <string>

namespace cmn::range_::io
{

template
<
      typename Char
    , typename CharTraits = std::char_traits<Char>
>
struct basic_fmt_specs
{
    using string_type = std::basic_string<Char, CharTraits>;

    // TODO: + default enum values
    string_type     open;
    string_type     separator;
    string_type     close;
};

}