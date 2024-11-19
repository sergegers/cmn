#pragma once

#include <iosfwd>
#include <ranges>
#include <algorithm>

// boost.io
#include <boost/io/ios_state.hpp>

#include <cmn/meta/concepts.h>
#include <cmn/meta/symbols.h>
#include <cmn/io/manip/slot/forwarder.h>
#include <cmn/io/manip/slot/manip.h>
#include <cmn/io/manip/slot/util.h>

namespace cmn::range::io
{

////////////////////////////////////////////////////////////////////////////////
//
// range_open manipulator
//
// ropen()          - "", no open symbol
// ropen("*")       - "*" open symbol
// ropen(reset_)    - "<" - default open symbol
//
////////////////////////////////////////////////////////////////////////////////
template <typename Char, typename CharTraits = std::char_traits<Char>>
using basic_open_manip =
    cmn::io::basic_string_slot_manip
    <
        struct basic_open_
      , Char
      , symbols<Char, CharTraits>::open_angle_bracket
      , symbols<Char, CharTraits>::nothing
      , CharTraits
    >
;

using open_manip = basic_open_manip<char>;
using wopen_manip = basic_open_manip<wchar_t>;

constexpr cmn::io::stream_slot_manip_forwarder<basic_open_manip> ropen {};

////////////////////////////////////////////////////////////////////////////////
//
// range_close manipulator
//
// rclose()          - "", no close symbol
// rclose("*")       - "*" close symbol
// rclose(reset_)    - ">" - default close symbol
//
////////////////////////////////////////////////////////////////////////////////
template <typename Char, typename CharTraits = std::char_traits<Char>>
using basic_close_manip =
    cmn::io::basic_string_slot_manip
    <
          struct basic_range_close_
		, Char
		, symbols<Char, CharTraits>::close_angle_bracket
        , symbols<Char, CharTraits>::nothing
		, CharTraits
    >
;

using close_manip = basic_close_manip<char>;
using wclose_manip = basic_close_manip<wchar_t>;

constexpr cmn::io::stream_slot_manip_forwarder<basic_close_manip> rclose {};

////////////////////////////////////////////////////////////////////////////////
//
// range_delimiter manipulator
//
////////////////////////////////////////////////////////////////////////////////
template <typename Char, typename CharTraits = std::char_traits<Char>>
using basic_range_delim_manip =
    cmn::io::basic_string_slot_manip
    <
          struct basic_range_delim_
		, Char
		, symbols<Char, CharTraits>::comma + symbols<Char, CharTraits>::whitespace
        , symbols<Char, CharTraits>::nothing
		, CharTraits
    >
;

using delim_manip = basic_range_delim_manip<char>;
using wdelim_manip = basic_range_delim_manip<wchar_t>;

constexpr cmn::io::stream_slot_manip_forwarder<basic_range_delim_manip> rdelim {};

///////////////////////////////////////////////////////////////////////////////
//
// Utilities
//
using rsaver = cmn::io::manip::iword_saver<open_manip, close_manip, delim_manip>;
using wrsaver = cmn::io::manip::iword_saver<wopen_manip, wclose_manip, wdelim_manip>;

//-----------------------------------------------------------------------------
//
// <
//      item_0
//      item_1
//      ...
//      item_n
// >
//
struct table_out_
{
    template <typename Char, typename CharTraits>
    friend decltype(auto) operator <<(std::basic_ostream<Char, CharTraits> &ostr, table_out_)
    {
        using symbols = symbols<Char, CharTraits>;
        return ostr << 
            ropen(symbols::open_angle_bracket + symbols::endl) << 
            rclose(symbols::close_angle_bracket + symbols::endl) <<
            rdelim(symbols::endl)
        ;
    }
};

constexpr table_out_ table_out{};

//-----------------------------------------------------------------------------
//
//  <item_0, item_1, ... item_n>
//
struct compact_table_out_
{
    template <typename Char, typename CharTraits>
    friend decltype(auto) operator << (std::basic_ostream<Char, CharTraits> &ostr, compact_table_out_)
    {
        using symbols = symbols<Char, CharTraits>;
        return ostr << 
            ropen(symbols::open_angle_bracket) << 
            rclose(symbols::close_angle_bracket) <<
            rdelim(symbols::comma + symbols::whitespace)
        ;
    }
};

constexpr compact_table_out_ compact_table_out{};

template <typename Char, typename CharTraits, std::ranges::range Range>
auto operator << (std::basic_ostream<Char, CharTraits> &ostr, Range rng) -> std::basic_ostream<Char, CharTraits> &
    requires c::printable<std::ranges::range_value_t<Range>, Char, CharTraits>
    // BUG: VS 17.0.6 couldn't usr cmn::c::printable concept
    //requires requires(std::ranges::range_value_t<Range> const &val)
    //{
    //    ostr << val;    
    //}
{
    using ostream_type = std::basic_ostream<Char, CharTraits>;

    // NOTE: don't make it static
    auto const open  = basic_open_manip<Char, CharTraits>::value(ostr);
    auto const delim = basic_range_delim_manip<Char, CharTraits>::value(ostr);
    auto const close = basic_close_manip<Char, CharTraits>::value(ostr);

    ostr << open;

    std::ranges::for_each
    (
        std::move(rng),
        [first = true, &ostr, &delim](auto const &elem) mutable 
        {
            if (!first)
                ostr << delim;
            else
                first = false;

            ostr << elem;
        }
    );

    ostr << close;

    return ostr;
}

}   

namespace cmn::io
{

using range::io::ropen;
using range::io::rclose;
using range::io::rdelim;
using range::io::rsaver;
using range::io::wrsaver;

}

// inject to STL streams namespace for using ADL
namespace std
{

using cmn::range::io::operator <<;

}
