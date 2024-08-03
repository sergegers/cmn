#pragma once

#include <iosfwd>
#include <string_view>
#include <tuple>
#include <concepts>

#include <cmn/meta/concepts.h>
#include <cmn/util/param.h>

#include <cmn/io/manip/slot/decoder.h>
#include <cmn/io/manip/slot/traits.h>

namespace cmn { struct test_accessor_t; }

namespace cmn::io
{

namespace manip
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
    static auto value(std::ios_base const &ios)->  keep_type
    {
        return static_cast<keep_type>(const_cast<std::ios_base &>(ios).pword(index()));
    }

    static auto value(std::ios_base &ios_, keep_type value) -> void
    {
        // set mask
        ios_.pword(index()) = value;
    }
};

template <typename Tag, cmn::c::enumerable KeepType>
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

////////////////////////////////////////////////////////////////////////////////
//
// Slot manipulator.
//
// Manipulator template parameters:
// DecodeType - loaded from/saved to stream type
// DefaultInit - value returned by manipulator.value() set before the manipulator was applied
// Default - the manipulator value set by default constructor
// Decoder - decoder
// Storage - storage
//
////////////////////////////////////////////////////////////////////////////////
struct reset_t {} constexpr reset_{};

template
<
      typename DecodeType
    , auto DefaultInit_
    , auto Default_
    , c::decoder Decoder
    , c::storage Storage
>
    requires
        std::same_as<keep_type_t<Decoder>, keep_type_t<Storage>>
     && c::decoder_of<Decoder, decltype(DefaultInit_)>
     && c::decoder_of<Decoder, decltype(Default_)>

class slot_manip final
{
private:
    using itself = slot_manip<DecodeType, DefaultInit_, Default_, Decoder, Storage>;
    friend test_accessor_t;
public:
    using decoder_type = Decoder;
    using storage_type = Storage;
    using decode_type = DecodeType;
    using keep_type = typename storage_type::keep_type;
private:

    static constexpr decoder_type decoder = {};

    static constexpr keep_type keep_real_default_init = {};
    static constexpr keep_type keep_default_init = decoder.decode(DefaultInit_);

    static inline decode_type decode_real_default_init = decoder.encode(keep_real_default_init);
    static inline decode_type decode_default_init = decoder.encode(keep_default_init);
    
    keep_type        m_value;   // value to apply

    template <typename ExtDecodeType> requires c::decoder_of<Decoder, ExtDecodeType>
    static auto decode(ExtDecodeType const &in) -> keep_type
    {
        if constexpr (keep_real_default_init == keep_default_init)
        {
            return decoder.decode(in);
        }
        else
        {
            // swap decode_real_default_init & decode_default_init
            return 
                in == decode_real_default_init?
                    keep_default_init:
                    in == decode_default_init?
                        keep_real_default_init:
                        decoder.decode(in)
            ;
        }
    }

    static auto encode(keep_type out) -> decode_type
    {
        if constexpr (keep_real_default_init == keep_default_init)
        {
            return decoder.encode(out);
        }
        else
        {
            // swap keep_real_default_init & keep_default_init
            return
            // TODO: VS 16.3.0 make switch {} after implementing std::bitcast
                out == keep_real_default_init? 
                    decode_default_init:
                    out == keep_default_init?
                        decode_real_default_init:
                        decoder.encode(out)
            ;
        }        
    }

    static auto apply_value(std::ios_base &ios, keep_type value) -> void
    {
        storage_type::value(ios, value);
    }

    auto apply(std::ios_base &ios) const -> void
    {
        apply_value(ios, m_value);
    }
public:
    //-----------------------------------------------------------------------------
    //
    // ctors
    //

    template <typename ExtDecodeType> requires c::decoder_of<Decoder, ExtDecodeType>
    explicit slot_manip(ExtDecodeType const &in):
        m_value { decode(in) }
    {}

    // reset stream to initial state (as if no manipulator was applied)
    explicit slot_manip(reset_t):
        m_value { keep_real_default_init }
    {}

    slot_manip() :
        m_value { keep_default_init }
    {}

    //-----------------------------------------------------------------------------
    static auto index() -> int { return storage_type::index(); }

    static auto value(std::ios_base const &ios) -> decode_type
    {
        return encode(storage_type::value(ios));
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
    , auto Default_ = DefaultInit_
    , typename CharTraits = std::char_traits<Char>
>
using basic_string_slot_manip =
    slot_manip
    <
        std::basic_string<Char, CharTraits>
      , DefaultInit_
      , Default_
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
    , auto Default_ = DefaultInit_
>
using int_slot_manip = slot_manip
<
      Int 
    , DefaultInit_
    , Default_
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
    , auto Default_ = DefaultInit_
>
    requires std::is_pointer_v<Ptr>

using ptr_slot_manip = slot_manip
<
      Ptr 
    , DefaultInit_
    , Default_
    , ptr_decoder<Ptr>
    , decode_param_t<ptr_storage_prm<TagOrStorage>>
>;

namespace detail
{

////////////////////////////////////////////////////////////////////////////////
// Callers
//
// Caller argument keep policy:
// keep arguments by value or constant reference
// support caller copying
//

template <typename Manip, typename... KeepArgs>
struct slot_manip_caller final
{
private:
    using itself = slot_manip_caller;
public:
    std::tuple<KeepArgs...>     m_args;

    constexpr explicit slot_manip_caller(KeepArgs const &... args) : 
        m_args { args... } {}

    template
    <
          typename Char
        , typename CharTraits
    >
    friend decltype(auto) operator << (std::basic_ostream<Char, CharTraits> & ostr, slot_manip_caller const &caller)
    {
        return ostr << std::apply
        (
            [](auto &&... args) { return Manip { args... }; },
            caller.m_args
        );
    }

    template
    <
          typename Char
        , typename CharTraits
    >
    friend decltype(auto) operator >> (std::basic_istream<Char, CharTraits> & istr, slot_manip_caller const &caller)
    {
        return istr >> std::apply
        (
            [](auto &&... args) { return Manip { args... }; },
            caller.m_args
        );
    }
};

template 
<
    template <typename Char, typename CharTraits> typename ManipT, 
    typename... KeepArgs
>
struct stream_slot_manip_caller final
{
private:
    using itself = stream_slot_manip_caller;
public:
    std::tuple<KeepArgs...> m_args;

    constexpr explicit stream_slot_manip_caller(KeepArgs const &... args) : 
        m_args { args... } {}

    template
    <
          typename Char
        , typename CharTraits
    >
    friend decltype(auto) operator << (std::basic_ostream<Char, CharTraits> & ostr, stream_slot_manip_caller const &caller)
    {
        return ostr << std::apply
        (
            [](auto &&... args) { return ManipT<Char, CharTraits> { args... }; },
            caller.m_args
        );
    }

    template
    <
          typename Char
        , typename CharTraits
    >
    friend decltype(auto) operator >> (std::basic_istream<Char, CharTraits> & istr, stream_slot_manip_caller const &caller)
    {
        return istr >> std::apply
        (
            [](auto &&... args) { return ManipT<Char, CharTraits> { args... }; },
            caller.m_args
        );
    }
};

//
////////////////////////////////////////////////////////////////////////////////
	
} 

}

using manip::reset_;
using manip::int_slot_manip;
using manip::basic_string_slot_manip;
using manip::ptr_slot_manip;

}