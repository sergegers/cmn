#pragma once

#include <limits>
#include <cstdint>

#include <cmn/io/source_options.h>
#include <cmn/util/symbols.h>

#include "print.h"

namespace cmn
{

namespace enum_::io
{

struct tag;

//-----------------------------------------------------------------------------
template
<
      typename Char
    , typename CharTraits
    , std::size_t Open_
    , std::size_t Close_
    , std::size_t Delim_
    , std::size_t ScopeResolution_
>
struct source_options :
    cmn::io::source_options<Char, CharTraits, Open_, Close_, Delim_>
{
private:
    using inherited = cmn::io::source_options<Char, CharTraits, Open_, Close_, Delim_>;
public:
    template <std::size_t N_> using string_t = inherited::template string_t<N_>;

    string_t<ScopeResolution_> scope_resolution;

    constexpr source_options
    (
          string_t<Open_> const &open_arg
        , string_t<Close_> const &close_arg
        , string_t<Delim_> const &delimiter_arg
        , string_t<ScopeResolution_> const &scope_resolution_arg
    ) :
        inherited{ open_arg, close_arg, delimiter_arg },
        scope_resolution{ scope_resolution_arg }
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
    , std::size_t ScopeResolution_
>
source_options
(
      basic_fixed_string<Char, Open_, CharTraits> const &open_arg
    , basic_fixed_string<Char, Close_, CharTraits> const &close_arg
    , basic_fixed_string<Char, Delim_, CharTraits> const &delimiter_arg
    , basic_fixed_string<Char, ScopeResolution_, CharTraits> const &scope_resolution_arg
)
    -> source_options<Char, CharTraits, Open_, Close_, Delim_, ScopeResolution_>;

//-----------------------------------------------------------------------------
template
<
      typename Char
    , typename CharTraits
>
struct vsource_options: cmn::io::vsource_options<Char, CharTraits>
{
private:
    using inherited = cmn::io::vsource_options<Char, CharTraits>;
public:
    using string_type = inherited::string_type;

    string_type scope_resolution;

    vsource_options
    (
          string_type const &open_arg
        , string_type const &close_arg
        , string_type const &delimiter_arg
        , string_type const &scope_resolution_arg
    ):
        inherited{ open_arg, close_arg, delimiter_arg },
        scope_resolution{ scope_resolution_arg }
    {}
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


    template 
    <
          typename Char
        , typename CharTraits
    >
    static constexpr auto source_options = enum_::io::source_options
    {
        sym::open_square_bracket.value<Char, CharTraits>(),
        sym::close_square_bracket.value<Char, CharTraits>(),

        // separator between bitfield or combo elements during the output
        sym::ws.value<Char, CharTraits>(),
        sym::scope_resolution.value<Char, CharTraits>()
    };
};

template <c::adapted_enum E>
struct format_traits<E>: format_traits<enum_::io::tag>
{};

}

}