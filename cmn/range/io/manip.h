#pragma once

#include <iosfwd>

#include <cmn/util/symbols.h>

#include <cmn/io/manip/slot/forwarder.h>
#include <cmn/io/manip/slot/manip.h>
#include <cmn/io/manip/slot/util.h>

#include "format_traits.h"

namespace cmn::range_::io
{

////////////////////////////////////////////////////////////////////////////////
//
// range_::open manipulator
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
      , cmn::io::sink_format_traits<tag, Char, CharTraits>::open
      , sym::nothing.value<Char, CharTraits>()
    >
;

//-----------------------------------------------------------------------------
template <typename Char, typename CharTraits = std::char_traits<Char>>
constexpr cmn::io::slot_manip_forwarder<basic_open_manip<Char, CharTraits>> basic_range_open {};

using open_manip = basic_open_manip<char>;
using wopen_manip = basic_open_manip<wchar_t>;

inline constexpr cmn::io::slot_manip_forwarder<open_manip> ropen {};
inline constexpr cmn::io::slot_manip_forwarder<wopen_manip> wropen {};

////////////////////////////////////////////////////////////////////////////////
//
// range_::close manipulator
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
		, cmn::io::sink_format_traits<tag, Char, CharTraits>::close
        , sym::nothing.value<Char, CharTraits>()
    >
;

//-----------------------------------------------------------------------------
template <typename Char, typename CharTraits = std::char_traits<Char>>
constexpr cmn::io::slot_manip_forwarder<basic_close_manip<Char, CharTraits>> basic_range_close {};

using close_manip = basic_close_manip<char>;
using wclose_manip = basic_close_manip<wchar_t>;

inline constexpr cmn::io::slot_manip_forwarder<close_manip> rclose {};
inline constexpr cmn::io::slot_manip_forwarder<wclose_manip> wrclose {};

////////////////////////////////////////////////////////////////////////////////
//
// range delimiter manipulator
//
////////////////////////////////////////////////////////////////////////////////
template <typename Char, typename CharTraits = std::char_traits<Char>>
using basic_delimiter_manip =
    cmn::io::basic_string_slot_manip
    <
          struct basic_delimiter_
		, cmn::io::sink_format_traits<tag, Char, CharTraits>::delimiter
        , sym::nothing.value<Char, CharTraits>()
    >
;

//-----------------------------------------------------------------------------
template <typename Char, typename CharTraits = std::char_traits<Char>>
constexpr cmn::io::slot_manip_forwarder<basic_delimiter_manip<Char, CharTraits>> basic_range_delimiter {};

using delimiter_manip = basic_delimiter_manip<char>;
using wdelimiter_manip = basic_delimiter_manip<wchar_t>;

inline constexpr cmn::io::slot_manip_forwarder<delimiter_manip> rdelim {};
inline constexpr cmn::io::slot_manip_forwarder<wdelimiter_manip> wrdelim {};

///////////////////////////////////////////////////////////////////////////////
//
// Utilities
//
using rsaver = cmn::io::manip::iword_saver<open_manip, close_manip, delimiter_manip>;
using wrsaver = cmn::io::manip::iword_saver<wopen_manip, wclose_manip, wdelimiter_manip>;

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
    friend decltype(auto) operator << (std::basic_ostream<Char, CharTraits>& ostr, table_out_)
    {
        using namespace sym;

        return ostr <<
            basic_range_open<Char, CharTraits>((open_angle_bracket + endl).value<Char, CharTraits>() <<
                basic_range_close<Char, CharTraits>((endl + close_angle_bracket).value<Char, CharTraits>()) <<
                basic_range_delimiter<Char, CharTraits>(endl.value<Char, CharTraits>()))
        ;
    }
};

inline constexpr table_out_ table_out{};

//-----------------------------------------------------------------------------
//
//  <item_0, item_1, ... item_n>
//
struct compact_table_out_
{
    template <typename Char, typename CharTraits>
    friend decltype(auto) operator << (std::basic_ostream<Char, CharTraits>& ostr, compact_table_out_)
    {
        using namespace sym;

        return ostr <<
            basic_range_open<Char, CharTraits>(open_angle_bracket.value<Char, CharTraits>()) <<
            basic_range_close<Char, CharTraits>(close_angle_bracket.value<Char, CharTraits>()) <<
            basic_range_delimiter<Char, CharTraits>((comma + ws).value<Char, CharTraits>())
        ;
    }
};

inline constexpr compact_table_out_ compact_table_out{};

}

namespace cmn::io
{

using range_::io::basic_range_open;
using range_::io::ropen;
using range_::io::wropen;
using range_::io::basic_range_close;
using range_::io::rclose;
using range_::io::wrclose;
using range_::io::basic_range_delimiter;
using range_::io::rdelim;
using range_::io::wrdelim;
using range_::io::rsaver;
using range_::io::wrsaver;

}
