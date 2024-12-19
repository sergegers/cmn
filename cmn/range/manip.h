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
constexpr cmn::io::slot_manip_forwarder<basic_open_manip<Char, CharTraits>> basic_ropen {};

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
constexpr cmn::io::slot_manip_forwarder<basic_close_manip<Char, CharTraits>> basic_rclose {};

using close_manip = basic_close_manip<char>;
using wclose_manip = basic_close_manip<wchar_t>;

inline constexpr cmn::io::slot_manip_forwarder<close_manip> rclose {};
inline constexpr cmn::io::slot_manip_forwarder<wclose_manip> wrclose {};

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
		, symbols<Char, CharTraits>::comma + symbols<Char, CharTraits>::whitespace
        , symbols<Char, CharTraits>::nothing
    >
;

//-----------------------------------------------------------------------------
template <typename Char, typename CharTraits = std::char_traits<Char>>
constexpr cmn::io::slot_manip_forwarder<basic_delim_manip<Char, CharTraits>> basic_rdelim {};

using delim_manip = basic_delim_manip<char>;
using wdelim_manip = basic_delim_manip<wchar_t>;

inline constexpr cmn::io::slot_manip_forwarder<delim_manip> rdelim {};
inline constexpr cmn::io::slot_manip_forwarder<wdelim_manip> wrdelim {};

///////////////////////////////////////////////////////////////////////////////
//
// Utilities
//
using rsaver = cmn::io::manip::iword_saver<open_manip, close_manip, delim_manip>;
using wrsaver = cmn::io::manip::iword_saver<wopen_manip, wclose_manip, wdelim_manip>;

}

namespace cmn::io
{

using range_::io::basic_ropen;
using range_::io::ropen;
using range_::io::wropen;
using range_::io::basic_rclose;
using range_::io::rclose;
using range_::io::wrclose;
using range_::io::basic_rdelim;
using range_::io::rdelim;
using range_::io::wrdelim;
using range_::io::rsaver;
using range_::io::wrsaver;

}
