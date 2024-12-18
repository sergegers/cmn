#pragma once

#include <ios>
#include <cstdlib>
#include <ranges>
#include <exception>

#include <cmn/meta/symbols.h>

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

    static auto index() -> int
    {
        // call xalloc once to get an index at which we can store data for this manipulator.
        static auto const idx = std::ios_base::xalloc();
        return idx;
    }

    static auto value(std::ios_base const &ios) ->  keep_type
    {
        return const_cast<std::ios_base &>(ios).pword(index());
    }

    static auto value(std::ios_base &ios, keep_type value) -> void
    {
        ios.pword(index()) = value;
    }
};

//-----------------------------------------------------------------------------
template <typename Tag>
struct int_stream_slot_storage 
{
    using keep_type = int_keep_type;
    using tag_type = Tag;

    static auto index() -> int
    {
        // call xalloc() once to get an index at which we can store data for this manipulator.
        static auto const idx = std::ios_base::xalloc();
        return idx;
    }

    static auto value(std::ios_base const &ios)->  keep_type
    {
        return const_cast<std::ios_base &>(ios).iword(index());
    }

    static auto value(std::ios_base &ios, keep_type value) -> void
    {
        ios.iword(index()) = value;
    }
};

//-----------------------------------------------------------------------------
template <typename Tag, c::pointer CharPtr>
struct large_string_stream_slot_storage
{
public:
    using keep_type = CharPtr;
    using tag_type = Tag;
    using char_type = std::remove_pointer_t<CharPtr>;

private:
    static auto alloc(std::ios_base const &ios, std::size_t sz) -> keep_type
    {
        return std::realloc(value(ios), sz * sizeof(char_type));
    }

    static auto on_exit(std::ios_base::event evt, std::ios_base &ios, int idx) noexcept -> void
    {
        if (std::ios_base::erase_event == evt && index() == idx)
            std::free(value(ios));
    }
public:
    static auto index() -> int
    {
        // call xalloc once to get an index at which we can store data for this manipulator.
        static auto const idx = std::ios_base::xalloc();
        std::ios_base::register_callback(&on_exit, idx);

        return idx;
    }

    static auto value(std::ios_base const &ios) ->  keep_type
    {
        return const_cast<std::ios_base &>(ios).pword(index());
    }

    static auto value(std::ios_base &ios, keep_type value) -> void
    {
        static constexpr auto max_string = 0x100000;
        auto const zero = cmn::to_char(symbols<CharPtr>::zero);

        auto const it = std::ranges::find(value, value + max_string, zero);
        if (value + max_string == it)
            throw std::logic_error{ "Not null terminated string" };

        auto const sz = std::ranges::distance(value, it) + 1;
        auto slot = alloc(ios, sz);

        std::ranges::copy(value, value + sz, slot);
        ios.pword(index()) = slot;
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