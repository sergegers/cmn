#pragma once

#include <cmn/meta/concepts.h>
#include <cmn/util/param.h>

#include "storage.h"

namespace cmn
{

namespace io::manip
{

///////////////////////////////////////////////////////////////////////////////
//
// Parameter selector
//
///////////////////////////////////////////////////////////////////////////////

template <typename TagOrStorage> struct tag_prm {};
template <typename TagOrStorage, typename KeepType> struct storage_prm {};

}

///////////////////////////////////////////////////////////////////////////////
//
// Parameter decoders
//
///////////////////////////////////////////////////////////////////////////////
template <typename TagOrStorage>
struct decode_param<io::manip::tag_prm<TagOrStorage>>
{
    using type = TagOrStorage;
};

template <c::storage TagOrStorage>
struct decode_param<io::manip::tag_prm<TagOrStorage>>
{
    using type = io::manip::storage_tag<TagOrStorage>;
};

//-----------------------------------------------------------------------------
template <c::storage TagOrStorage, typename KeepType>
struct decode_param<io::manip::storage_prm<TagOrStorage, KeepType>>
{
    static_assert(std::same_as<KeepType, io::manip::keep_type_t<TagOrStorage>>);
    using type = TagOrStorage;
};

template <typename TagOrStorage> requires !c::storage<TagOrStorage>
struct decode_param<io::manip::storage_prm<TagOrStorage, io::manip::ptr_keep_type>>
{
    using type = io::manip::ptr_stream_slot_storage<TagOrStorage>;
};

template <typename TagOrStorage> requires !c::storage<TagOrStorage>
struct decode_param<io::manip::storage_prm<TagOrStorage, io::manip::int_keep_type>>
{
    using type = io::manip::int_stream_slot_storage<TagOrStorage>;
};

}