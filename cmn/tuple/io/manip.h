#pragma once

#include <iosfwd>

#include <cmn/util/symbols.h>

#include <cmn/io/manip/slot/forwarder.h>
#include <cmn/io/manip/slot/manip.h>
#include <cmn/io/manip/slot/util.h>

#include "format_traits.h"

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
      , cmn::io::sink_format_traits<tag, Char, CharTraits>::open
      , sym::nothing.value<Char, CharTraits>()
    >
;

template <typename Char, typename CharTraits = std::char_traits<Char>>
constexpr cmn::io::slot_manip_forwarder<basic_open_manip<Char, CharTraits>> basic_tuple_open {};

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
		, cmn::io::sink_format_traits<tag, Char, CharTraits>::close
        , sym::nothing.value<Char, CharTraits>()
    >
;

template <typename Char, typename CharTraits = std::char_traits<Char>>
constexpr cmn::io::slot_manip_forwarder<basic_close_manip<Char, CharTraits>> basic_tuple_close {};

using close_manip = basic_close_manip<char>;
using wclose_manip = basic_close_manip<wchar_t>;

inline constexpr cmn::io::slot_manip_forwarder<close_manip> tclose {};
inline constexpr cmn::io::slot_manip_forwarder<wclose_manip> wtclose {};

////////////////////////////////////////////////////////////////////////////////
//
// tuple delimiter manipulator
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

template <typename Char, typename CharTraits = std::char_traits<Char>>
constexpr cmn::io::slot_manip_forwarder<basic_delimiter_manip<Char, CharTraits>> basic_tuple_delimiter {};

using delimiter_manip = basic_delimiter_manip<char>;
using wdelimiter_manip = basic_delimiter_manip<wchar_t>;

inline constexpr cmn::io::slot_manip_forwarder<delimiter_manip> tdelim {};
inline constexpr cmn::io::slot_manip_forwarder<wdelimiter_manip> wtdelim {};

///////////////////////////////////////////////////////////////////////////////
//
// Utilities
//
using tsaver = cmn::io::manip::iword_saver<open_manip, close_manip, delimiter_manip>;
using wtsaver = cmn::io::manip::iword_saver<wopen_manip, wclose_manip, wdelimiter_manip>;

}

namespace cmn::io_
{

using tuple_::io::basic_tuple_open;
using tuple_::io::topen;
using tuple_::io::wtopen;
using tuple_::io::basic_tuple_close;
using tuple_::io::tclose;
using tuple_::io::wtclose;
using tuple_::io::basic_tuple_delimiter;
using tuple_::io::tdelim;
using tuple_::io::wtdelim;
using tuple_::io::tsaver;
using tuple_::io::wtsaver;

}
