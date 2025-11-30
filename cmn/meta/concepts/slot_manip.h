#pragma once

#include <ios>
#include <concepts>

namespace cmn::c
{

///////////////////////////////////////////////////////////////////////////////
//
// Slot manipulator concepts
//
///////////////////////////////////////////////////////////////////////////////
template <typename U, typename V>
concept interoperable_with = std::equality_comparable_with<U, V>;

//-----------------------------------------------------------------------------
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
        { T::decode(in) } -> std::same_as<typename T::keep_type>;
        { T::encode(out) } -> std::same_as<typename T::decode_type>;
    }
;

//-----------------------------------------------------------------------------
template <typename T, typename DecodeType>
concept decoder_for = decoder<T> && requires(DecodeType const &dt) 
{
    { T::decode(dt) } -> std::same_as<typename T::keep_type>;
};

//-----------------------------------------------------------------------------
template <typename T, typename Decoder>
concept decoded_by = decoder_for<Decoder, T>;

//-----------------------------------------------------------------------------
template <typename T, typename DecodeType>
concept static_decoder_for = decoder_for<T, DecodeType> && requires(DecodeType const &dt) 
{
    { T::static_decode(dt) } -> interoperable_with<typename T::keep_type>;
};

///////////////////////////////////////////////////////////////////////////////
//
// low level manipulator slot processor
//
template <typename T>
concept storage = std::default_initializable<T> && requires(std::ios_base &ios) 
{
    typename T::tag_type;
    typename T::keep_type;

    { T::index(ios) } -> std::same_as<int>; // slot index
    { T::value(ios) } -> std::same_as<typename T::keep_type>; // get value
    { T::value(ios, typename T::keep_type{}) } -> std::same_as<void>; // set value
};

template <typename T>
concept restore_storage = storage<T> && requires(std::ios_base &ios) 
{
    { T::restore(ios) } -> std::same_as<void>;
};
//-----------------------------------------------------------------------------
//
// slot_manip stuff

// NOTE: introduce concept to drop <slot/manip.h> dependency
template <typename T>
concept slot_manipulator = 
    requires(std::ios_base &ios) 
    {
        typename T::decoder_type;
        typename T::decode_type;
        typename T::storage_type;
        typename T::keep_type;

        { T::index(ios) } -> std::same_as<int>;
        { T::value(ios) } -> std::same_as<typename T::decode_type>;
        // TODO: refine concept
    } 
 && storage<typename T::storage_type>
;

}
