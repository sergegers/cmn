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
      , symbols<Char, CharTraits>::open_angle_bracket
      , symbols<Char, CharTraits>::nothing
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
		, symbols<Char, CharTraits>::close_angle_bracket
        , symbols<Char, CharTraits>::nothing
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
// range separator manipulator
//
////////////////////////////////////////////////////////////////////////////////
template <typename Char, typename CharTraits = std::char_traits<Char>>
using basic_separator_manip =
    cmn::io::basic_string_slot_manip
    <
          struct basic_separator_
		, symbols<Char, CharTraits>::comma + symbols<Char, CharTraits>::whitespace
        , symbols<Char, CharTraits>::nothing
    >
;

//-----------------------------------------------------------------------------
template <typename Char, typename CharTraits = std::char_traits<Char>>
constexpr cmn::io::slot_manip_forwarder<basic_separator_manip<Char, CharTraits>> basic_range_separator {};

using separator_manip = basic_separator_manip<char>;
using wseparator_manip = basic_separator_manip<wchar_t>;

inline constexpr cmn::io::slot_manip_forwarder<separator_manip> rsep {};
inline constexpr cmn::io::slot_manip_forwarder<wseparator_manip> wrsep {};

///////////////////////////////////////////////////////////////////////////////
//
// Utilities
//
using rsaver = cmn::io::manip::iword_saver<open_manip, close_manip, separator_manip>;
using wrsaver = cmn::io::manip::iword_saver<wopen_manip, wclose_manip, wseparator_manip>;

}

namespace cmn::io
{

using range_::io::basic_range_open;
using range_::io::ropen;
using range_::io::wropen;
using range_::io::basic_range_close;
using range_::io::rclose;
using range_::io::wrclose;
using range_::io::basic_range_separator;
using range_::io::rsep;
using range_::io::wrsep;
using range_::io::rsaver;
using range_::io::wrsaver;

}
