#pragma once

#include <iosfwd>

#include <cmn/meta/symbols.h>
#include <cmn/io/manip/slot/forwarder.h>
#include <cmn/io/manip/slot/manip.h>
#include <cmn/io/manip/slot/util.h>

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
// range_::delimiter manipulator
//
////////////////////////////////////////////////////////////////////////////////
template <typename Char, typename CharTraits = std::char_traits<Char>>
using basic_delim_manip =
    cmn::io::basic_string_slot_manip
    <
          struct basic_range_delim_
		, Char
		, symbols<Char, CharTraits>::comma + symbols<Char, CharTraits>::whitespace
        , symbols<Char, CharTraits>::nothing
		, CharTraits
    >
;

using delim_manip = basic_delim_manip<char>;
using wdelim_manip = basic_delim_manip<wchar_t>;

constexpr cmn::io::stream_slot_manip_forwarder<basic_range_delim_manip> rdelim {};

///////////////////////////////////////////////////////////////////////////////
//
// Utilities
//
using rsaver = cmn::io::manip::iword_saver<open_manip, close_manip, delim_manip>;
using wrsaver = cmn::io::manip::iword_saver<wopen_manip, wclose_manip, wdelim_manip>;

}