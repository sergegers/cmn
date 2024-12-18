#pragma once

#include <cmn/meta/symbols.h>
#include <cmn/meta/concepts.h> // c::enum_<>, print_t

#include <cmn/io/manip/slot/manip.h>
#include <cmn/io/manip/slot/forwarder.h>
#include <cmn/io/manip/slot/util.h>

#include <cmn/enum/print_t.h>

namespace cmn::enum_::io
{

////////////////////////////////////////////////////////////////////////////////
//
// Manipulator bitfield_mask_manip could be used with custom enums
//
////////////////////////////////////////////////////////////////////////////////
template <c::enum_ Enum>
using bitfield_mask_manip =
    cmn::io::int_slot_manip
    <
        struct bitfield_mask_
      , Enum
      , static_cast<Enum>(std::numeric_limits<interop_type_t<Enum>>::max())
    >
;

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
      , Char
      , symbols<Char, CharTraits>::open_square_bracket
      , symbols<Char, CharTraits>::nothing
      , CharTraits
    >
;

//-----------------------------------------------------------------------------
template <typename Char, typename CharTraits = std::char_traits<Char> >
constexpr cmn::io::slot_manip_forwarder<basic_open_manip<Char, CharTraits>> basic_eopen {};

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
      , Char
      , symbols<Char, CharTraits>::close_square_bracket
      , symbols<Char, CharTraits>::nothing
      , CharTraits
    >
;

//-----------------------------------------------------------------------------
template <typename Char, typename CharTraits = std::char_traits<Char> >
constexpr cmn::io::slot_manip_forwarder<basic_close_manip<Char, CharTraits>> basic_eclose {};

using close_manip = basic_close_manip<char>;
using wclose_manip = basic_close_manip<wchar_t>;

inline constexpr cmn::io::slot_manip_forwarder<close_manip> eclose {};
inline constexpr cmn::io::slot_manip_forwarder<wclose_manip> weclose {};

////////////////////////////////////////////////////////////////////////////////
//
// Manipulator basic_bitfield_delim_manip for customizing delimiter between
// bitfield elements during output.
// Default value is " "
//
////////////////////////////////////////////////////////////////////////////////
template <typename Char, typename CharTraits = std::char_traits<Char> >
using basic_bitfield_delim_manip =
    cmn::io::basic_string_slot_manip
    <
        struct basic_bitfield_delim_
      , Char
      , symbols<Char, CharTraits>::whitespace
      , symbols<Char, CharTraits>::nothing
      , CharTraits
    >
;

//-----------------------------------------------------------------------------
template <typename Char, typename CharTraits = std::char_traits<Char> >
constexpr cmn::io::slot_manip_forwarder<basic_bitfield_delim_manip<Char, CharTraits>> basic_bitfield_delim {};

using bitfield_delim_manip = basic_bitfield_delim_manip<char>;
using wbitfield_delim_manip = basic_bitfield_delim_manip<wchar_t>;

inline constexpr cmn::io::slot_manip_forwarder<bitfield_delim_manip> bitfield_delim {};
inline constexpr cmn::io::slot_manip_forwarder<wbitfield_delim_manip> wbitfield_delim {};

///////////////////////////////////////////////////////////////////////////////
//
// Print options manipulator
//
///////////////////////////////////////////////////////////////////////////////
using print_manip =
    cmn::io::int_slot_manip
    <
          struct print_
        , print_t
        , print_t::tail
        , print_t::empty
    >
;

inline constexpr cmn::io::slot_manip_forwarder<print_manip> eprint {};

///////////////////////////////////////////////////////////////////////////////
//
// Utilities
//
template <typename Char, typename CharTraits = std::char_traits<Char>>

using basic_esaver = cmn::io::manip::iword_saver
<
    basic_open_manip<Char, CharTraits>
    , basic_close_manip<Char, CharTraits>
    , basic_bitfield_delim_manip<Char, CharTraits>
    , print_manip
>;

using esaver = basic_esaver<char>;
using wesaver = basic_esaver<wchar_t>;

}   

namespace cmn::io
{

using enum_::io::bitfield_mask;
using enum_::io::basic_eopen;
using enum_::io::eopen;
using enum_::io::weopen;
using enum_::io::basic_eclose;
using enum_::io::eclose;
using enum_::io::weclose;
using enum_::io::basic_bitfield_delim;
using enum_::io::bitfield_delim;
using enum_::io::wbitfield_delim;
using enum_::io::eprint;
using enum_::io::basic_esaver;
using enum_::io::esaver;
using enum_::io::wesaver;

}