#pragma once

#include <format>

#if __has_include(<boost/fusion/concepts.hpp>)
#   include <boost/fusion/concepts.hpp>
#else
#   include <cmn/meta/boost/fusion/concepts.hpp>
#endif

#include <cmn/fwd.h>
#include <cmn/util/symbols.h>
#include <cmn/enum/bitfield.h>

namespace cmn
{

namespace tuple_::io
{

struct tag;

enum class options_t
{
      empty = 0x0
    , brackets = 0x1
    , delimiter = 0x2
};

consteval auto adapt_enum_info(options_t)
{
    using enum options_t;
    return enum_::adapt_bitfield_info_helper<brackets, delimiter>();
}

CMN_ENUM_INJECT_OPS()
//-----------------------------------------------------------------------------
template
<
    typename Char
    , typename CharTraits = std::char_traits<Char>
>
struct sink_format_options
{
    using string_type = std::basic_string<Char, CharTraits>;
    //-----------------------------------------------------------------------------
    //
    // concept list_sink_format_options
    //
    using options_type = options_t;
    using char_type = Char;
    using char_traits_type = CharTraits;

    options_type options = []
    {
            using enum options_t;
            return brackets | delimiter;
    }();

    string_type open = sym::open_parenthese.as_string<Char, CharTraits>();
    string_type close = sym::close_parenthese.as_string<Char, CharTraits>();
    // separator between bitfield or combo elements during the output
    string_type delimiter = (sym::comma + sym::ws).as_string<Char, CharTraits>();
    //
    //-----------------------------------------------------------------------------
};

}

namespace io
{

template <>
struct format_traits<tuple_::io::tag>
{
    //-----------------------------------------------------------------------------
    //
    // concept format_options
    //
    using options_type = tuple_::io::options_t;
    static constexpr options_type options = options_type::brackets | options_type::delimiter;
    //
    //-----------------------------------------------------------------------------
};

template <boost::c::fus_sequence Seq>
struct format_traits<Seq> : format_traits<tuple_::io::tag> {};

///////////////////////////////////////////////////////////////////////////////

template
<
      typename Char
    , typename CharTraits
>
struct sink_format_traits<tuple_::io::tag, Char, CharTraits>: format_traits<tuple_::io::tag>
{
    //-----------------------------------------------------------------------------
    //
    // concept list_sink_format_options
    //
    using char_type = Char;
    using char_traits_type = CharTraits;

    static constexpr auto open = sym::open_parenthese.value<Char, CharTraits>();
    static constexpr auto close = sym::close_parenthese.value<Char, CharTraits>();
    // separator between bitfield or combo elements during the output
    static constexpr auto delimiter = (sym::comma + sym::ws).value<Char, CharTraits>();
    //
    //-----------------------------------------------------------------------------

    static constexpr auto scope_resolution = sym::scope_resolution.value<Char, CharTraits>();
};

template
<
      boost::c::fus_sequence Seq
    , typename Char
    , typename CharTraits
>
struct sink_format_traits<Seq, Char, CharTraits> : sink_format_traits<tuple_::io::tag, Char, CharTraits> {};

}

}

