#pragma once

#include <iosfwd>
#include <string_view>
#include <tuple>
#include <concepts>

#include <cmn/meta/concepts.h>
#include <cmn/meta/type_traits.h>
#include <cmn/meta/macro.h>

#include <cmn/io/manip/slot/decoder.h>
#include <cmn/io/manip/slot/traits.h>
#include <cmn/io/manip/slot/storage.h>
#include <cmn/io/manip/slot/param.h>

namespace cmn { struct test_accessor_t; }

namespace cmn::io
{

namespace manip
{

////////////////////////////////////////////////////////////////////////////////
//
// Slot manipulator.
//
// Manipulator template parameters:
// DecodeType - loaded from/saved to stream type
// DefaultInit - value returned by manipulator.value() set before the manipulator was applied
// DefaultManip - value set by the manipulator w/o arguments
// Decoder - decoder
// Storage - storage
//
////////////////////////////////////////////////////////////////////////////////

// reset stream to initial state (as if no manipulator was applied)
struct reset_t {} constexpr reset_{};

template
<
      typename DecodeType
    , auto DefaultInit_
    , auto DefaultManip_
    , c::decoder Decoder
    , c::storage Storage
>
    requires
        std::same_as<keep_type_t<Decoder>, keep_type_t<Storage>>
     && c::static_decoder_for<Decoder, decltype(DefaultInit_)>
     && c::static_decoder_for<Decoder, decltype(DefaultManip_)>

class slot_manip final
{
private:
    using itself = slot_manip;
    friend test_accessor_t;
public:
    using decoder_type = Decoder;
    using storage_type = Storage;
    using decode_type = DecodeType;
    using keep_type = typename storage_type::keep_type;
private:
    static constexpr decoder_type       decoder = {};

    //-----------------------------------------------------------------------------
    //
    // interact with Storage through this class
    //
    class RS_PASS_BY_VALUE_ATTR unswapped_keep_type final
    {
    private:
        static constexpr keep_type real_default_init = {};  // zero initialized
        static constexpr auto default_init = decoder.static_decode(DefaultInit_);

        keep_type m_value;
    public:
        constexpr explicit unswapped_keep_type(keep_type value): m_value{ value } {}

        constexpr explicit operator keep_type () const
        {
            if constexpr (real_default_init != default_init)
            {
                // swap real_default_init & default_init
                if (m_value == real_default_init) return default_init;
                if (m_value == default_init) return real_default_init;

                return m_value;
            }
            else
            {
                return m_value;
            }
        }
    };

    //-----------------------------------------------------------------------------
    struct storage_wrapper
    {
        static auto index(std::ios_base &ios) -> int { return storage_type::index(ios); }

        static auto value(std::ios_base &ios) -> keep_type
        {
            return static_cast<keep_type>(unswapped_keep_type { storage_type::value(ios) });
        }

        static auto value(std::ios_base &ios, keep_type value) -> void
        {
            storage_type::value(ios, static_cast<keep_type>(unswapped_keep_type{ value }));
        }
    };

    //
    //-----------------------------------------------------------------------------

    static constexpr storage_wrapper    storage = {};

    keep_type                           m_value;   // value to apply

    auto apply(std::ios_base &ios) const -> void
    {
        storage.value(ios, m_value);
    }
public:
    //-----------------------------------------------------------------------------
    //
    // ctors
    //
    constexpr explicit slot_manip(c::decoded_by<Decoder> auto const &in):
        m_value { decoder.decode(in) }
    {}

    // reset stream to initial state (as if no manipulator was applied)
    constexpr explicit slot_manip(reset_t):
        m_value { decoder.decode(DefaultInit_) }
    {}

    constexpr slot_manip() :
        m_value { decoder.decode(DefaultManip_) }
    {}

    //-----------------------------------------------------------------------------
    //
    // slot_manipulator concept
    //
    static auto index(std::ios_base &ios) -> int { return storage_wrapper::index(ios); }

    static auto value(std::ios_base &ios) -> decode_type
    {
        return decoder.encode(storage.value(ios));
    }

    template
    <
          typename Char
        , typename CharTraits
    >
    friend decltype(auto) operator << (std::basic_ostream<Char, CharTraits> &ostr, itself const &self)
    {
        return self.apply(ostr), ostr;
    }

    template
    <
          typename Char
        , typename CharTraits
    >
    friend decltype(auto) operator >> (std::basic_istream<Char, CharTraits> &istr, itself const &self)
    {
        return self.apply(istr), istr;
    }
};

////////////////////////////////////////////////////////////////////////////////
//
// basic_string_slot_manip
//
////////////////////////////////////////////////////////////////////////////////
template
<
      typename TagOrStorage                         // = int_stream_slot_storage<TagOrStorage>
    , c::string auto DefaultInit_
    , c::string auto DefaultManip_ = DefaultInit_
>
    requires
        std::same_as<char_t<decltype(DefaultInit_)>, char_t<decltype(DefaultManip_)>>
     && std::same_as<char_traits_t<decltype(DefaultInit_)>, char_traits_t<decltype(DefaultManip_)>>

using basic_string_slot_manip =
    slot_manip
    <
        std::basic_string<char_t<decltype(DefaultInit_)>, char_traits_t<decltype(DefaultInit_)>>
      , DefaultInit_
      , DefaultManip_
      , string_decoder<std::basic_string<char_t<decltype(DefaultInit_)>, char_traits_t<decltype(DefaultInit_)>>>
      , decode_param_t<storage_prm<TagOrStorage, int_keep_type>>
    >
;

////////////////////////////////////////////////////////////////////////////////
//
// basic_large_string_slot_manip
//
////////////////////////////////////////////////////////////////////////////////
template <c::string auto DefaultInit_>
using ls_decode_type_t = std::basic_string<char_t<decltype(DefaultInit_)>, char_traits_t<decltype(DefaultInit_)>>;

template
<
      typename TagOrStorage                         // = int_stream_slot_storage<TagOrStorage>
    , c::string auto DefaultInit_
    , c::string auto DefaultManip_ = DefaultInit_
>
    requires
        std::same_as<char_t<decltype(DefaultInit_)>, char_t<decltype(DefaultManip_)>>
     && std::same_as<char_traits_t<decltype(DefaultInit_)>, char_traits_t<decltype(DefaultManip_)>>

using basic_large_string_slot_manip =
    slot_manip
    <
        ls_decode_type_t<DefaultInit_>
      , DefaultInit_
      , DefaultManip_
      , large_string_decoder<ls_decode_type_t<DefaultInit_>>
      , large_string_stream_slot_storage<TagOrStorage, char_t<decltype(DefaultInit_)>, char_traits_t<decltype(DefaultInit_)>>
    >
;

////////////////////////////////////////////////////////////////////////////////
//
// int_slot_manip
//
// see example:
//   using out_size_manip = cmn::io::int_slot_manip<struct out_size_, bool, false, true>;
//   constexpr auto const skip_size = cmn::io::slot_manip_forwarder<out_size_manip>{}(true);
//   constexpr auto const show_size = cmn::io::slot_manip_forwarder<out_size_manip>{}(false);
//
// DefaultInit_ - initial manipulator value
// Default_ - value after applying manipulator w/o parameter
//
////////////////////////////////////////////////////////////////////////////////
template
<
      typename TagOrStorage // = int_stream_slot_storage
    , c::enumerable auto DefaultInit_
    , decltype(DefaultInit_) DefaultManip_ = DefaultInit_
>
using int_slot_manip = slot_manip
<
      decltype(DefaultInit_)
    , DefaultInit_
    , DefaultManip_
    , int_decoder<decltype(DefaultInit_)>
    , decode_param_t<storage_prm<TagOrStorage, int_keep_type>>
>;

////////////////////////////////////////////////////////////////////////////////
//
// ptr_slot_manip
//
////////////////////////////////////////////////////////////////////////////////
template
<
      typename TagOrStorage // = ptr_stream_slot_storage
    , c::pointer auto DefaultInit_
    , decltype(DefaultInit_) DefaultManip_ = DefaultInit_
>
using ptr_slot_manip = slot_manip
<
      decltype(DefaultInit_)
    , DefaultInit_
    , DefaultManip_
    , ptr_decoder<decltype(DefaultInit_)>
    , decode_param_t<storage_prm<TagOrStorage, ptr_keep_type>>
>;

}

using manip::reset_;
using manip::int_slot_manip;
using manip::basic_string_slot_manip;
using manip::ptr_slot_manip;

}