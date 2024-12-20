#pragma once

#include <string_view>
#include <string>
#include <array>
#include <cstdint>
#include <type_traits>
#include <bit>
// ReSharper disable once CppUnusedIncludeDirective
#include <ranges>

#include <cmn/meta/concepts.h>
#include <cmn/util/fixed_string.h>

#include "fwd.h"

namespace cmn::io::manip
{

////////////////////////////////////////////////////////////////////////////////
//
// Decoder template parameters:
// InType - input type
//
// Decoder methods:
// auto decode(decode_type)->keep_type
// auto encode(keep_type)->decode_type
//
////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
// Used for small char and string types
//
template <typename String>
class string_decoder;

template 
<
	  typename Char
	, typename CharTraits
>
    // slot_room() >= 1
    requires (sizeof(int_keep_type) / sizeof(Char) >= 1)

class string_decoder<std::basic_string<Char, CharTraits>> final
{
public:
    using decode_type = std::basic_string<Char, CharTraits>;
    using keep_type = int_keep_type;
private:
    using string_view_type = std::basic_string_view<Char, CharTraits>;

    static constexpr auto slot_room() -> std::size_t { return sizeof(keep_type) / sizeof(Char); }
    //-----------------------------------------------------------------------------
    //
    // value operations
    //

    // n counts the trailing zero
    static constexpr auto length(std::size_t n)
    {
        // cut trailing zero if N_ == slot_room()
        return n >= slot_room()? slot_room() - 1: n;
    }

    static constexpr auto bits_in_char() { return sizeof(Char) * CHAR_BIT; }
public:
    //-----------------------------------------------------------------------------
    //
    // decoder concept
    //

    // encode - read chars from slot w/o null
    static constexpr auto encode(keep_type decoded) noexcept -> decode_type
    {
        // for little endian
        auto const arr = std::bit_cast<std::array<Char const, slot_room()>>(decoded);
        // is decoded string null terminated?
        auto const it = std::ranges::find(arr, '\0');
        auto const sz = it == std::ranges::end(arr)? 
            slot_room():
            std::ranges::distance(std::ranges::begin(arr), it)
        ;
        return decode_type { arr.data(), sz };
    }

    //-----------------------------------------------------------------------------
    //
    // decoder_of concept
    //
    static constexpr auto decode(string_view_type str) -> keep_type
    {
        if (slot_room() < str.length()) 
            throw std::logic_error{ "String is too big." };

        keep_type res { 0 };
        for (auto idx = 0ul; idx < str.length(); ++idx)
            res |= static_cast<keep_type>(str[idx]) << bits_in_char() * idx;

        // if slot_room() > str.length() trailing zero will be appended
        return res;
    }


    template <std::size_t N_> requires (slot_room() >= N_)
    static constexpr auto static_decode(basic_fixed_string<Char, N_, CharTraits> const &str) noexcept -> keep_type
    {
        // endian independent implementation, 
        // also it keeps function constexpr
        return
            []<std::size_t... Idss_>
            (basic_fixed_string<Char, N_, CharTraits> const &str_, std::index_sequence<Idss_...>) constexpr -> keep_type
            {
                return (0 | ... | (keep_type{ str_[Idss_] } << bits_in_char() * Idss_));
            }
        (
              str
            , std::make_index_sequence<length(N_)>{}
        );
    }
};

///////////////////////////////////////////////////////////////////////////////
//
// Used for large char and string types
//
template <typename String>
class large_string_decoder;

template 
<
	  typename Char
	, typename CharTraits
>
class large_string_decoder<std::basic_string<Char, CharTraits>> final
{
public:
    using string_view_type  = std::basic_string_view<Char, CharTraits>;

    using decode_type = std::basic_string<Char, CharTraits>;
    using keep_type = string_view_type;

    //-----------------------------------------------------------------------------
    //
    // decoder concept
    //

    static constexpr auto encode(keep_type decoded) noexcept -> decode_type
    {
        return decode_type { decoded };
    }

    // decode_type implicitly converted to string_view_type
    static constexpr auto decode(string_view_type str) -> keep_type
    {
        return str;
    }

    //-----------------------------------------------------------------------------
    //
    // decoder_of concept
    //

    // based_fixed_string<> implicitly converted to string_view_type
    //static constexpr auto decode(string_view_type str) -> keep_type

    template <std::size_t N_>
    static constexpr auto static_decode(basic_fixed_string<Char, N_, CharTraits> const &str) noexcept
    {
        return str;
    }
};

///////////////////////////////////////////////////////////////////////////////
//
// Used for integral types
//
template <c::enumerable Int>
    requires (sizeof(int_keep_type) >= sizeof(Int))

class int_decoder final
{
public:
    using decode_type = Int;
    using keep_type = int_keep_type;
    //-----------------------------------------------------------------------------
    //
    // decoder concept
    //
    static constexpr auto encode(keep_type out) noexcept -> decode_type
    {
        return static_cast<decode_type>(out);
    }

    static constexpr auto decode(decode_type in) noexcept -> keep_type
    {
        return static_cast<keep_type>(in);
    }

    //-----------------------------------------------------------------------------
    //
    // decoder_of concept
    //
    static constexpr auto static_decode(decode_type in) noexcept -> keep_type
    {
        return decode(in);
    }
};

///////////////////////////////////////////////////////////////////////////////
//
// Used for pointer types
//
template<c::pointer Ptr>
class ptr_decoder final
{
public:
    using decode_type = Ptr;
    using keep_type = ptr_keep_type;
    //-----------------------------------------------------------------------------
    //
    // decoder concept
    //
    static constexpr auto decode(decode_type in) noexcept -> keep_type
    {
        using pointless_type = std::remove_pointer_t<decode_type>;
        if constexpr (std::is_const_v<pointless_type>)
        {
            // remove constant from pointer
            using mut_ptr_type = std::add_pointer_t<std::remove_const_t<pointless_type>>;
            return const_cast<mut_ptr_type>(in);
        }
        else
            return in;
    }

    static constexpr auto encode(keep_type out) -> decode_type
    {
        return static_cast<decode_type>(out);
    }

    //-----------------------------------------------------------------------------
    //
    // decoder_of concept
    //
    static constexpr auto static_decode(decode_type in) noexcept -> keep_type
    {
        return decode(in);
    }
};

}
