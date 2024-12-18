#pragma once

#include <iosfwd>

#include <cmn/meta/concepts.h>
#include <cmn/util/param.h>

namespace cmn::io
{

namespace manip
{

///////////////////////////////////////////////////////////////////////////////
//
enum class stream_type
{
	in,
	out,
	in_out = in | out
};

template <typename Char, typename CharTraits, stream_type St_> struct stream;

template <typename Char, typename CharTraits> 
struct stream<Char, CharTraits, stream_type::in> { using type = std::basic_istream<Char, CharTraits>; };

template <typename Char, typename CharTraits> 
struct stream<Char, CharTraits, stream_type::out> { using type = std::basic_ostream<Char, CharTraits>; };

///////////////////////////////////////////////////////////////////////////////
//
template <c::slot_manipulator Manip>
struct traits
{
    using decoder_type = typename Manip::decoder_type;
    using decode_type = typename Manip::decode_type;
    using storage_type = typename Manip::storage_type;
    using keep_type = typename Manip::keep_type;
};

///////////////////////////////////////////////////////////////////////////////
//
// Helpers
template <typename T>
using keep_type_t = typename T::keep_type;

template <typename T>
using decode_type_t = typename T::decode_type;


// helpers
template <typename Storage>
struct storage_tag
{
    using type = typename Storage::tag_type;    
};

}

template <c::slot_manipulator Manip>
using manip_traits = manip::traits<Manip>;

// helpers
template <c::slot_manipulator Manip>
using manip_decode_type_t = typename manip_traits<Manip>::decode_type;

}

