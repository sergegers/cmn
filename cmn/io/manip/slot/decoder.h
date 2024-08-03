#pragma once

#include <string_view>
#include <string>
#include <array>
#include <cstdint>
#include <type_traits>
#include <bit>
#include <ranges>

#include <cmn/meta/concepts.h>
#include <cmn/util/fixed_string.h>

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
// Used for char and string types
//
template <typename String>
class string_decoder;

template 
<
	  typename Char
	, typename CharTraits
>
    // slot_room() >= 1
    requires (sizeof(std::intptr_t) / sizeof(Char) >= 1)

class string_decoder<std::basic_string<Char, CharTraits>> final
{
public:
    using decode_type = std::basic_string<Char, CharTraits>;
    using keep_type = std::intptr_t;
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
    // decoder interface
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

    // N_ counts the trailing zero
    template <std::size_t N_>
        requires (slot_room() >= N_ - 1)
    static constexpr auto decode(std::array<Char, N_> const &arr) noexcept -> keep_type
    {
        // endian independent implementation, 
        // also it keeps function constexpr
        return
            []<std::size_t... Idss_>
            (std::array<Char, N_> const &arr_, std::index_sequence<Idss_...>) constexpr -> keep_type
            {
                return (0 | ... | (keep_type{ arr_[Idss_] } << bits_in_char() * Idss_));
            }
        (
              arr
            , std::make_index_sequence<length(N_)>{}
        );
    }

    template <std::size_t N_>
        requires (slot_room() >= N_)
    static constexpr auto decode(basic_fixed_string<Char, N_, CharTraits> const &str) noexcept -> keep_type
    {
        return decode(str.data_());
    }

    static auto decode(string_view_type str) -> keep_type
    {
        if (slot_room() < str.length()) 
            throw std::logic_error{ "String is too big." };

        keep_type res { 0 };
        for (auto idx = 0; idx < str.length(); ++idx)
            res |= static_cast<keep_type>(str[idx]) << bits_in_char() * idx;

        // if slot_room() > str.length() trailing zero will be appended
        return res;
    }
};

///////////////////////////////////////////////////////////////////////////////
//
// Used for integral types
//
template <c::enumerable Int>
    // sizeof(keep_type) >= sizeof(decode_type)
    requires (sizeof(void *) >= sizeof(Int))

class int_decoder final
{
public:
    using decode_type = Int;
    using keep_type = std::intptr_t;
    //-----------------------------------------------------------------------------
    //
    // decoder interface
    //
    static constexpr auto decode(decode_type in) noexcept -> keep_type
    {
        return static_cast<keep_type>(in);
    }

    static constexpr auto encode(keep_type out) noexcept -> decode_type
    {
        return static_cast<decode_type>(out);
    }
};

///////////////////////////////////////////////////////////////////////////////
//
// Used for pointer types
//
template <typename Ptr>
    requires std::is_pointer_v<Ptr>
class ptr_decoder final
{
public:
    using decode_type = Ptr;
    using keep_type = void *;
    //-----------------------------------------------------------------------------
    //
    // decoder interface
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
};

}
