#pragma once

#include <iosfwd>

#include <cmn/meta/symbols.h>
#include <cmn/io/manip/slot/forwarder.h>
#include <cmn/io/manip/slot/manip.h>
#include <cmn/io/manip/slot/util.h>

namespace cmn::tuple_::io
{

////////////////////////////////////////////////////////////////////////////////
//
// tuple_::open manipulator
//
// topen()          - "", no open symbol
// topen("*")       - "*" open symbol
// topen(reset_)    - "(" - default open symbol
//
////////////////////////////////////////////////////////////////////////////////
template <typename Char, typename CharTraits = std::char_traits<Char>>
using basic_open_manip =
    cmn::io::basic_string_slot_manip
    <
        struct basic_open_
      , Char
      , symbols<Char, CharTraits>::open_parenthese
      , symbols<Char, CharTraits>::nothing
      , CharTraits
    >
;

using open_manip = basic_open_manip<char>;
using wopen_manip = basic_open_manip<wchar_t>;

constexpr cmn::io::stream_slot_manip_forwarder<basic_open_manip> topen {};

////////////////////////////////////////////////////////////////////////////////
//
// tuple_::close manipulator
//
// tclose()          - "", no close symbol
// tclose("*")       - "*" close symbol
// tclose(reset_)    - ")" - default close symbol
//
////////////////////////////////////////////////////////////////////////////////
template <typename Char, typename CharTraits = std::char_traits<Char>>
using basic_close_manip =
    cmn::io::basic_string_slot_manip
    <
          struct basic_range_close_
		, Char
		, symbols<Char, CharTraits>::close_parenthese
        , symbols<Char, CharTraits>::nothing
		, CharTraits
    >
;

using close_manip = basic_close_manip<char>;
using wclose_manip = basic_close_manip<wchar_t>;

constexpr cmn::io::stream_slot_manip_forwarder<basic_close_manip> tclose {};

////////////////////////////////////////////////////////////////////////////////
//
// tuple_::delimiter manipulator
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

constexpr cmn::io::stream_slot_manip_forwarder<basic_delim_manip> rdelim {};

///////////////////////////////////////////////////////////////////////////////
//
// Utilities
//
using tsaver = cmn::io::manip::iword_saver<open_manip, close_manip, delim_manip>;
using wtsaver = cmn::io::manip::iword_saver<wopen_manip, wclose_manip, wdelim_manip>;

}