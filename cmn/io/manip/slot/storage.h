#pragma once

#include <ios>
#include <cstdlib>
// ReSharper disable CppUnusedIncludeDirective
#include <ranges>
#include <exception>
// ReSharper restore CppUnusedIncludeDirective

#include <cmn/util/symbols.h>

#include "fwd.h"

namespace cmn::io::manip
{

////////////////////////////////////////////////////////////////////////////////
//
// Stream slot storage
// template parameter Tag added to make an unique slot for every manipulator
//
////////////////////////////////////////////////////////////////////////////////

template <typename Tag>
struct ptr_stream_slot_storage
{
    using keep_type = ptr_keep_type;
    using tag_type = Tag;

    static auto index(std::ios_base &/*ios*/) -> int
    {
        // call xalloc once to get an index at which we can store data for this manipulator.
        static auto const idx = std::ios_base::xalloc();
        return idx;
    }

    static auto value(std::ios_base &ios) ->  keep_type
    {
        return ios.pword(index(ios));
    }

    static auto value(std::ios_base &ios, keep_type value) -> void
    {
        ios.pword(index(ios)) = value;
    }
};

//-----------------------------------------------------------------------------
template <typename Tag>
struct int_stream_slot_storage 
{
    using keep_type = int_keep_type;
    using tag_type = Tag;

    static auto index(std::ios_base &/*ios*/) -> int
    {
        // call xalloc() once to get an index at which we can store data for this manipulator.
        static auto const idx = std::ios_base::xalloc();
        return idx;
    }

    static auto value(std::ios_base &ios)->  keep_type
    {
        return ios.iword(index(ios));
    }

    static auto value(std::ios_base &ios, keep_type value) -> void
    {
        ios.iword(index(ios)) = value;
    }
};

//-----------------------------------------------------------------------------
template
<
      typename Tag
    , typename Char
    , typename CharTraits = std::char_traits<Char>
>
struct large_string_stream_slot_storage
{
public:
    using keep_type = std::basic_string_view<Char, CharTraits>;
    using tag_type = Tag;
    using char_type = Char;

private:
    static auto buffer(std::ios_base &ios)
    {
        return static_cast<char_type *>(ios.pword(index(ios)));
    }

    static auto realloc(std::ios_base &ios, std::size_t sz) -> char_type *
    {
        return static_cast<char_type *>(std::realloc(buffer(ios), sz * sizeof(char_type)));
    }

    static auto on_exit(std::ios_base::event evt, std::ios_base &ios, int idx) noexcept -> void
    {
        if (std::ios_base::erase_event == evt && index(ios) == idx)
            std::free(buffer(ios));
    }
public:
    static auto index(std::ios_base &ios) -> int
    {
        // call xalloc once to get an index at which we can store data for this manipulator.
        static auto const idx = [](std::ios_base &ios)
        {
            auto const idx = std::ios_base::xalloc();
            ios.register_callback(&on_exit, idx);
            return idx;
        }
        (ios);

        return idx;
    }

    static auto value(std::ios_base &ios) ->  keep_type
    {
        auto const buffer_ = buffer(ios);
        return buffer_? keep_type{ buffer_}: keep_type{};
    }

    static auto value(std::ios_base &ios, keep_type value) -> void
    {
        using namespace sym;

        auto const null_pos = value.size();
        auto const new_buffer = realloc(ios, null_pos + 1);

        std::ranges::copy(value, new_buffer);
        // terminating null
        new_buffer[null_pos] = ends.as_char<Char, CharTraits>();
        ios.pword(index(ios)) = new_buffer;
    }
};

//-----------------------------------------------------------------------------
#ifdef CMN_STATIC_TEST

namespace detail
{

static_assert(c::storage<ptr_stream_slot_storage<struct ptr_tag>>);
static_assert(c::storage<int_stream_slot_storage<struct int_tag>>);
static_assert(c::storage<large_string_stream_slot_storage<struct string_tag, char *>>);

}

#endif

}