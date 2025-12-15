#pragma once

#include <cstddef>
#include <string>

#include <cmn/util/fixed_string.h>

namespace cmn::io
{

template
<
      typename Char
    , typename CharTraits
    , std::size_t Open_
    , std::size_t Close_
    , std::size_t Delim_
>
struct source_options
{
    template <std::size_t N_>
    using string_t = basic_fixed_string<Char, N_, CharTraits>;

    string_t<Open_>     open;
    string_t<Close_>    close;
    string_t<Delim_>    delimiter;

    constexpr source_options
    (
          string_t<Open_> const& open_arg
        , string_t<Close_> const& close_arg
        , string_t<Delim_> const& delimiter_arg
    ) :
        open{ open_arg }, close{ close_arg }, delimiter{ delimiter_arg }
    {
    }
};

template
<
      typename Char
    , typename CharTraits
    , std::size_t Open_
    , std::size_t Close_
    , std::size_t Delim_
>
source_options
(
      basic_fixed_string<Char, Open_, CharTraits> const& open_arg
    , basic_fixed_string<Char, Close_, CharTraits> const& close_arg
    , basic_fixed_string<Char, Delim_, CharTraits> const& delimiter_arg
)
    -> source_options<Char, CharTraits, Open_, Close_, Delim_>;

///////////////////////////////////////////////////////////////////////////////
template
<
      typename Char
    , typename CharTraits
>
struct vsource_options
{
    using string_type = std::basic_string<Char, CharTraits>;

    string_type open;
    string_type close;
    string_type delimiter;
};

}
