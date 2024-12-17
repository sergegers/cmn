#pragma once

#include <concepts>
#include <utility>
#include <ios>

namespace cmn::io::manip::c
{

///////////////////////////////////////////////////////////////////////////////
//
template <typename T>
concept decoder =
    std::default_initializable<T>
 && requires
    {
        typename T::decode_type;
        typename T::keep_type;    
    }
 && requires(typename T::decode_type in, typename T::keep_type out)
    {
        { T::decode(in) } -> std::same_as<typename T::keep_type>;    // constexpr for decode packed_value
        { T::encode(out) } -> std::same_as<typename T::decode_type>;
    }
;

///////////////////////////////////////////////////////////////////////////////
//
template <typename T, typename DecodeType>
concept decoder_of =
    decoder<T>
 && requires(DecodeType const &dt)
    {
        { T::decode(dt) } -> std::same_as<typename T::keep_type>;
    }
;

//-----------------------------------------------------------------------------
template <typename T, typename Decoder> concept decoded_by = decoder_of<Decoder, T>;

///////////////////////////////////////////////////////////////////////////////
//
// low level manipulator slot processor
//
template <typename T>
concept storage = 
    requires(std::ios_base &ios)
    {
        typename T::tag_type;
        typename T::keep_type;

        { T::index() } -> std::same_as<int>;                                            // slot index
        { T::value(std::as_const(ios)) } -> std::same_as<typename T::keep_type>;     // get value
        { T::value(ios, typename T::keep_type{}) } -> std::same_as<void>;               // set value
    }
;

template <typename T>
concept restore_storage = 
    storage<T>
 && requires(std::ios_base & ios)
    {
        { T::restore(ios) } -> std::same_as<void>;
    }
;
//-----------------------------------------------------------------------------
//
// slot_manip stuff

// NOTE: introduce concept to drop <slot/manip.h> dependency
template <typename T>
concept slot_manipulator =
    requires (std::ios_base const &cios)
    {
        typename T::decoder_type;
        typename T::decode_type;
        typename T::storage_type;
        typename T::keep_type;

        { T::index() } -> std::same_as<int>;
        { T::value(cios) } -> std::same_as<typename T::decode_type>;
        // TODO: refine concept
    }
 && storage<typename T::storage_type>
;

}
