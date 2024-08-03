#pragma once

#include <iosfwd>

#include <cmn/io/manip/slot/concepts.h>
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

///////////////////////////////////////////////////////////////////////////////
//
// Parameter selector
//
///////////////////////////////////////////////////////////////////////////////

template <typename TagOrStorage> struct tag_prm {};
template <typename TagOrStorage> struct ptr_storage_prm {};
template <typename TagOrStorage> struct int_storage_prm {};

// helpers
template <typename Storage>
struct storage_tag
{
    using type = typename Storage::tag_type;    
};

// forward
template <typename Tag, typename KeepType>
struct basic_stream_slot_storage;

template <typename Tag>
using ptr_stream_slot_storage = basic_stream_slot_storage<Tag, void *>;

template <typename Tag>
using int_stream_slot_storage = basic_stream_slot_storage<Tag, std::intptr_t>;

}

template <manip::c::slot_manipulator Manip>
using manip_traits = manip::traits<Manip>;

// helpers
template <manip::c::slot_manipulator Manip>
using manip_decode_type_t = typename manip_traits<Manip>::decode_type;

}

// Parameter decoders
namespace cmn
{

template <typename TagOrStorage>
struct decode_param<io::manip::tag_prm<TagOrStorage>>
{
    using type = TagOrStorage;
};

template <io::manip::c::storage TagOrStorage>
struct decode_param<io::manip::tag_prm<TagOrStorage>>
{
    using type = io::manip::storage_tag<TagOrStorage>;
};

//-----------------------------------------------------------------------------
template <typename TagOrStorage>
struct decode_param<io::manip::ptr_storage_prm<TagOrStorage>>
{
    using type = io::manip::ptr_stream_slot_storage<TagOrStorage>;
};

template <io::manip::c::storage TagOrStorage>
struct decode_param<io::manip::ptr_storage_prm<TagOrStorage>>
{
    using type = TagOrStorage;
};

//-----------------------------------------------------------------------------
template <typename TagOrStorage>
struct decode_param<io::manip::int_storage_prm<TagOrStorage>>
{
    using type = io::manip::int_stream_slot_storage<TagOrStorage>;
};

template <io::manip::c::storage TagOrStorage>
struct decode_param<io::manip::int_storage_prm<TagOrStorage>>
{
    using type = TagOrStorage;
};

}