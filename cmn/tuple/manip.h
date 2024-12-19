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
      , symbols<Char, CharTraits>::open_parenthese
      , symbols<Char, CharTraits>::nothing
    >
;

using open_manip = basic_open_manip<char>;
using wopen_manip = basic_open_manip<wchar_t>;

inline constexpr cmn::io::slot_manip_forwarder<open_manip> topen {};
inline constexpr cmn::io::slot_manip_forwarder<wopen_manip> wtopen {};

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
		, symbols<Char, CharTraits>::close_parenthese
        , symbols<Char, CharTraits>::nothing
    >
;

using close_manip = basic_close_manip<char>;
using wclose_manip = basic_close_manip<wchar_t>;

inline constexpr cmn::io::slot_manip_forwarder<close_manip> tclose {};
inline constexpr cmn::io::slot_manip_forwarder<wclose_manip> wtclose {};

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
		, symbols<Char, CharTraits>::comma + symbols<Char, CharTraits>::whitespace
        , symbols<Char, CharTraits>::nothing
    >
;

using delim_manip = basic_delim_manip<char>;
using wdelim_manip = basic_delim_manip<wchar_t>;

inline constexpr cmn::io::slot_manip_forwarder<delim_manip> rdelim {};
inline constexpr cmn::io::slot_manip_forwarder<wdelim_manip> wrdelim {};

///////////////////////////////////////////////////////////////////////////////
//
// Utilities
//
using tsaver = cmn::io::manip::iword_saver<open_manip, close_manip, delim_manip>;
using wtsaver = cmn::io::manip::iword_saver<wopen_manip, wclose_manip, wdelim_manip>;

}