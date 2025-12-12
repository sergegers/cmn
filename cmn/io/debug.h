#pragma once

#include <ios>
#include <istream>

#include <cmn/enum/bitfield.h>

namespace cmn::io
{

enum class fmtflags_t
{
    skipws = std::ios_base::skipws,
    unitbuf = std::ios_base::unitbuf,
    uppercase = std::ios_base::uppercase,
    showbase = std::ios_base::showbase,
    showpoint = std::ios_base::showpoint,
    showpos = std::ios_base::showpos,
    left = std::ios_base::left,
    right = std::ios_base::right,
    internal = std::ios_base::internal,
    dec = std::ios_base::dec,
    oct = std::ios_base::oct,
    hex = std::ios_base::hex,
    scientific = std::ios_base::scientific,
    fixed = std::ios_base::fixed,
    boolalpha = std::ios_base::boolalpha
};

consteval auto adapt_enum_info(fmtflags_t)
{
    using enum fmtflags_t;
    return enum_::adapt_bitfield_info_helper<skipws, unitbuf, uppercase, showbase, showpoint, showpos, left, right,
                                             internal, dec, oct, hex, scientific, fixed, boolalpha>();
}

CMN_ENUM_INJECT_OPS()

///////////////////////////////////////////////////////////////////////////////
enum class iostate_t
{
    goodbit = std::ios_base::goodbit,
    eofbit = std::ios_base::eofbit,
    failbit = std::ios_base::failbit,
    badbit = std::ios_base::badbit
};

consteval auto adapt_enum_info(iostate_t)
{
    using enum iostate_t;
    return enum_::adapt_bitfield_info_helper<goodbit, eofbit, failbit, badbit>();
}

CMN_ENUM_INJECT_OPS()

///////////////////////////////////////////////////////////////////////////////
template <typename Char, typename CharTraits = std::char_traits<Char>>
struct basic_stream_cursor
{
    std::streampos  m_pos;
    iostate_t       m_state;
};

//-----------------------------------------------------------------------------
template <typename Char, typename CharTraits = std::char_traits<Char>>
struct basic_istream_wrapper
{
    using stream_type = std::basic_istream<Char, CharTraits>;
    using cursor_type = basic_stream_cursor<Char, CharTraits>;

    stream_type const &m_str;


};

}
