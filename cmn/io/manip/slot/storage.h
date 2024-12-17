#pragma once

#include <type_traits>

#include <cmn/meta/concepts.h>

namespace cmn::io::manip
{

    ////////////////////////////////////////////////////////////////////////////////
//
// Stream slot storage
// template parameter Tag added to make an unique slot for every manipulator
//
////////////////////////////////////////////////////////////////////////////////

//template <typename Tag, typename KeepType>
//struct basic_stream_slot_storage;

template <typename Tag, typename KeepType>
    requires std::is_pointer_v<KeepType>
struct basic_stream_slot_storage<Tag, KeepType>
{
    using keep_type = KeepType;
    using tag_type = Tag;

    static auto index() -> int
    {
        // call xalloc once to get an index at which we can store data for this
        // manipulator.
        static auto const idx = std::ios_base::xalloc();
        return idx;
    }
    static auto value(std::ios_base const &ios) ->  keep_type
    {
        return static_cast<keep_type>(const_cast<std::ios_base &>(ios).pword(index()));
    }

    static auto value(std::ios_base &ios, keep_type value) -> void
    {
        // set mask
        ios.pword(index()) = value;
    }
};

template <typename Tag, cmn::c::enumerable KeepType>
struct basic_stream_slot_storage<Tag, KeepType>
{
    using keep_type = KeepType;
    using tag_type = Tag;

    static auto index() -> int
    {
        // call xalloc() once to get an index at which we can store data for this
        // manipulator.
        static auto const idx = std::ios_base::xalloc();
        return idx;
    }
    static auto value(std::ios_base const &ios)->  keep_type
    {
        return static_cast<keep_type>(const_cast<std::ios_base &>(ios).iword(index()));
    }

    static auto value(std::ios_base &ios_, keep_type value) -> void
    {
        // set mask
        ios_.iword(index()) = static_cast<long>(value);
    }
};

#ifdef CHT_STATIC_TEST

static_assert(c::storage<ptr_stream_slot_storage<int>>);
static_assert(c::storage<int_stream_slot_storage<int>>);

#endif

}