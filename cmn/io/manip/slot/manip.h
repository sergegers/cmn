#pragma once

#include <iosfwd>
#include <string_view>
#include <tuple>
#include <concepts>

#include <cmn/meta/concepts.h>
#include <cmn/meta/macro.h>

#include <cmn/util/param.h>

#include <cmn/io/manip/slot/decoder.h>
#include <cmn/io/manip/slot/traits.h>
#include <cmn/io/manip/slot/storage.h>

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
     && c::decoder_of<Decoder, decltype(DefaultInit_)>
     && c::decoder_of<Decoder, decltype(DefaultManip_)>

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
        static constexpr keep_type real_default_init = 0;
        static constexpr keep_type default_init = decoder.decode(DefaultInit_);

        keep_type m_value;
    public:
        constexpr explicit unswapped_keep_type(keep_type value): m_value{ value } {}

        constexpr explicit operator keep_type () const
        {
            if constexpr (real_default_init != default_init)
            {
                // swap real_default_init & default_init
                switch (m_value)
                {
                case real_default_init: return default_init;
                case default_init: return real_default_init;
                default: return m_value;
                }
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
        static auto index() -> int { return storage_type::index(); }

        static auto value(std::ios_base const &ios) -> keep_type
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
    static auto index() -> int { return storage_wrapper::index(); }

    static auto value(std::ios_base const &ios) -> decode_type
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
      typename TagOrStorage // = int_stream_slot_storage
    , std::integral Char
    , auto DefaultInit_
    , auto DefaultManip_ = DefaultInit_
    , typename CharTraits = std::char_traits<Char>
>
using basic_string_slot_manip =
    slot_manip
    <
        std::basic_string<Char, CharTraits>
      , DefaultInit_
      , DefaultManip_
      , string_decoder<std::basic_string<Char, CharTraits>>
      , decode_param_t<int_storage_prm<TagOrStorage>>
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
    , cmn::c::enumerable Int
    , auto DefaultInit_
    , auto DefaultManip_ = DefaultInit_
>
using int_slot_manip = slot_manip
<
      Int 
    , DefaultInit_
    , DefaultManip_
    , int_decoder<Int>
    , decode_param_t<int_storage_prm<TagOrStorage>>
>;

////////////////////////////////////////////////////////////////////////////////
//
// ptr_slot_manip
//
////////////////////////////////////////////////////////////////////////////////
template
<
      typename TagOrStorage // = ptr_stream_slot_storage
    , typename Ptr
    , auto DefaultInit_
    , auto DefaultManip_ = DefaultInit_
>
    requires std::is_pointer_v<Ptr>

using ptr_slot_manip = slot_manip
<
      Ptr 
    , DefaultInit_
    , DefaultManip_
    , ptr_decoder<Ptr>
    , decode_param_t<ptr_storage_prm<TagOrStorage>>
>;

}

using manip::reset_;
using manip::int_slot_manip;
using manip::basic_string_slot_manip;
using manip::ptr_slot_manip;

}