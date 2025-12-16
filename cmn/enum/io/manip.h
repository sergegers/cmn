#pragma once

#include <cmn/meta/concepts.h>      // c::enum_<>, print_t

#include <cmn/io/manip/slot/manip.h>
#include <cmn/io/manip/slot/forwarder.h>
#include <cmn/io/manip/slot/util.h>
#include <cmn/io/manip/format_options.h>

#include "format_traits.h"

namespace cmn::enum_::io
{

////////////////////////////////////////////////////////////////////////////////
//
// Manipulator bitfield_mask_manip could be used with custom enums
//
////////////////////////////////////////////////////////////////////////////////
template <c::enum_ E>
using bitfield_mask_manip = cmn::io::int_slot_manip
<
      struct bitfield_mask_
    , static_cast<E>(cmn::io::format_traits<tag>::mask)
>;

inline constexpr cmn::io::deduce_slot_manip_forwarder<bitfield_mask_manip> bitfield_mask {};

////////////////////////////////////////////////////////////////////////////////
//
// eopen manipulator
//
// eopen()      - "", no open symbol
// eopen("<")    - "<" open symbol
// eopen(reset_) - "[", default open symbol
//
////////////////////////////////////////////////////////////////////////////////
template <typename Char, typename CharTraits = std::char_traits<Char> >
using basic_open_manip =
    cmn::io::basic_string_slot_manip
    <
        struct basic_enum_open_
      , cmn::io::sink_format_traits<tag, Char, CharTraits>::open
      , sym::nothing.value<Char, CharTraits>()
    >
;

//-----------------------------------------------------------------------------
template <typename Char, typename CharTraits = std::char_traits<Char> >
constexpr cmn::io::slot_manip_forwarder<basic_open_manip<Char, CharTraits>> basic_enum_open {};

using open_manip = basic_open_manip<char>;
using wopen_manip = basic_open_manip<wchar_t>;

inline constexpr cmn::io::slot_manip_forwarder<open_manip> eopen {};
inline constexpr cmn::io::slot_manip_forwarder<wopen_manip> weopen {};

////////////////////////////////////////////////////////////////////////////////
//
// eclose manipulator
//
// eclose()      - "", no close symbol
// eclose(">")    - ">" close symbol
// eclose(reset_) -> "]" - default close symbol
//
////////////////////////////////////////////////////////////////////////////////
template <typename Char, typename CharTraits = std::char_traits<Char> >
using basic_close_manip =
    cmn::io::basic_string_slot_manip
    <
        struct basic_enum_close_
      , cmn::io::sink_format_traits<tag, Char, CharTraits>::close
      , sym::nothing.value<Char, CharTraits>()
    >
;

//-----------------------------------------------------------------------------
template <typename Char, typename CharTraits = std::char_traits<Char> >
constexpr cmn::io::slot_manip_forwarder<basic_close_manip<Char, CharTraits>> basic_enum_close {};

using close_manip = basic_close_manip<char>;
using wclose_manip = basic_close_manip<wchar_t>;

inline constexpr cmn::io::slot_manip_forwarder<close_manip> eclose {};
inline constexpr cmn::io::slot_manip_forwarder<wclose_manip> weclose {};

////////////////////////////////////////////////////////////////////////////////
//
// Manipulator basic_bitfield_delimiter_manip for customizing separator between
// bitfield or combo elements during the output.
// Default value is " "
//
////////////////////////////////////////////////////////////////////////////////
template <typename Char, typename CharTraits = std::char_traits<Char> >
using basic_bitfield_delimiter_manip =
    cmn::io::basic_string_slot_manip
    <
        struct basic_bitfield_delimiter_
      , cmn::io::sink_format_traits<tag, Char, CharTraits>::delimiter
      , sym::nothing.value<Char, CharTraits>()
    >
;

//-----------------------------------------------------------------------------
template <typename Char, typename CharTraits = std::char_traits<Char> >
constexpr cmn::io::slot_manip_forwarder<basic_bitfield_delimiter_manip<Char, CharTraits>> basic_bitfield_separator {};

using bitfield_delimiter_manip = basic_bitfield_delimiter_manip<char>;
using wbitfield_delimiter_manip = basic_bitfield_delimiter_manip<wchar_t>;

inline constexpr cmn::io::slot_manip_forwarder<bitfield_delimiter_manip> bfdelim {};
inline constexpr cmn::io::slot_manip_forwarder<wbitfield_delimiter_manip> wbfdelim {};

///////////////////////////////////////////////////////////////////////////////
//
// Print options manipulator
//
///////////////////////////////////////////////////////////////////////////////
using print_manip = cmn::io::manip::format_options_manip<print_t>;
inline constexpr cmn::io::manip::format_options_forwarder<print_t> eprint {};

///////////////////////////////////////////////////////////////////////////////
//
// Utilities
//
template <typename Char, typename CharTraits = std::char_traits<Char>>

using basic_esaver = cmn::io::manip::iword_saver
<
      basic_open_manip<Char, CharTraits>
    , basic_close_manip<Char, CharTraits>
    , basic_bitfield_delimiter_manip<Char, CharTraits>
    , print_manip
>;

using esaver = basic_esaver<char>;
using wesaver = basic_esaver<wchar_t>;

}   

namespace cmn::io
{

using enum_::io::bitfield_mask;
using enum_::io::basic_enum_open;
using enum_::io::eopen;
using enum_::io::weopen;
using enum_::io::basic_enum_close;
using enum_::io::eclose;
using enum_::io::weclose;
using enum_::io::basic_bitfield_separator;
using enum_::io::bfdelim;
using enum_::io::wbfdelim;
using enum_::io::eprint;
using enum_::io::basic_esaver;
using enum_::io::esaver;
using enum_::io::wesaver;

}