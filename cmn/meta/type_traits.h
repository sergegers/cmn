#pragma once

#include <boost/mp11.hpp>

#include <type_traits>
#include <utility>
#include <limits>

#include <cmn/meta/concepts.h>

namespace cmn
{

template<typename T>
struct remove_rvalue_reference
{
    using type = T;
};

template<typename T>
struct remove_rvalue_reference<T&&>
{
    using type = T;
};

template<typename T>
using remove_rvalue_reference_t = typename remove_rvalue_reference<T>::type;

///////////////////////////////////////////////////////////////////////////////
//
// extended underlying_type
//
template <typename T>
struct underlying_type: std::type_identity<T> {};

template <typename T>
using underlying_type_t = typename underlying_type<T>::type;

// enum specialization
template <c::enum_ T>
struct underlying_type<T>: std::underlying_type<T> {};

// unit specialization
template <c::unit Unit>
struct underlying_type<Unit>: std::type_identity<typename Unit::underlying_type> {};

template <typename T>
using underlying_type_t = typename underlying_type<T>::type;

//-----------------------------------------------------------------------------
template <typename I>
constexpr auto to_underlying(I i) { return static_cast<underlying_type_t<I>>(i); }

template <typename T>
constexpr auto underlying_cast(T t) { return static_cast<underlying_type_t<T>>(t); }

////////////////////////////////////////////////////////////////////////////////
// Metafunction make_integer_range
// make integer sequence [B, E)
////////////////////////////////////////////////////////////////////////////////
namespace detail
{

template <c::enumerable auto B_>
using underlying_t = underlying_type_t<decltype(B_)>;

//-----------------------------------------------------------------------------
template<c::enumerable auto Add_, typename Seq>
struct add_and_convert;

template<typename A, A Add_, typename I, I... Idss_>
struct add_and_convert<Add_, std::integer_sequence<I, Idss_...>>
{
    using type = std::integer_sequence<A, static_cast<A>(static_cast<I>(Add_) + Idss_) ...>;
};

}

///////////////////////////////////////////////////////////////////////////////
//
// template <c::enumerable auto B_, decltype(B_) E_> using make_integer_sequence_t
// make std::integer_sequence<decltype(B_), B_, B_ + 1, ... E_ - 1>
//
// https://stackoverflow.com/a/40617900/8452129
// NOTE: for using std::make_integer_sequence integer type must be really integer, not enum.
//  std::make_integer_sequence has the compiler check
//
///////////////////////////////////////////////////////////////////////////////
template <c::enumerable auto B_, decltype(B_) E_> requires (E_ >= B_)
using make_integer_sequence = typename detail::add_and_convert
<
      B_
    , std::make_integer_sequence
      <
            detail::underlying_t<B_>
          , underlying_cast(E_) - underlying_cast(B_) /* half opened range */
      >
>::type;

////////////////////////////////////////////////////////////////////////////////
//
// metafunction make_index_sequence
//
////////////////////////////////////////////////////////////////////////////////
template <std::size_t B_, std::size_t E_>
using make_index_sequence = make_integer_sequence<B_, E_>;

////////////////////////////////////////////////////////////////////////////////
//
// metafunction index_swap
//
////////////////////////////////////////////////////////////////////////////////
namespace detail
{

template
<
    typename ESeq,
    typename ESeq::value_type X,
    typename ESeq::value_type Y,
    typename ESeq::value_type... Bs
>
struct index_swap_y;

template
<
    typename ValueType,
    ValueType X,
    ValueType Y,
    ValueType E,
    ValueType... Es,
    ValueType... Bs
>
struct index_swap_y<std::integer_sequence<ValueType, E, Es...>, X, Y, Bs...>
{
    using type = typename index_swap_y<std::integer_sequence<ValueType, Es...>, X, Y, Bs..., E>::type;
};

template
<
    typename ValueType,
    ValueType X,
    ValueType Y,
    ValueType... Es,
    ValueType... Bs
>
struct index_swap_y<std::integer_sequence<ValueType, Y, Es...>, X, Y, Bs...>
{
    using type = std::integer_sequence<ValueType, Bs..., X, Es...>;
};

template
<
    typename ESeq,
    typename ESeq::value_type X,
    typename ESeq::value_type Y,
    typename ESeq::value_type... Bs
>
struct idex_swap_x;

template
<
    typename ValueType,
    ValueType X,
    ValueType Y,
    ValueType E,
    ValueType... Es,
    ValueType... Bs
>
struct idex_swap_x<std::integer_sequence<ValueType, E, Es...>, X, Y, Bs...>
{
    using type = typename idex_swap_x<std::integer_sequence<ValueType, Es...>, X, Y, Bs..., E>::type;
};

template
<
    typename ValueType,
    ValueType X,
    ValueType Y,
    ValueType... Es,
    ValueType... Bs
>
struct idex_swap_x<std::integer_sequence<ValueType, X, Es...>, X, Y, Bs...>
{
    using type = typename index_swap_y<std::integer_sequence<ValueType, Es...>, X, Y, Bs..., Y>::type;
};

} 

template <typename Seq, typename Seq::value_type X, typename Seq::value_type Y>
struct index_swap : detail::idex_swap_x<Seq, X, Y> {};

template <typename Seq, typename Seq::value_type X>
struct index_swap<Seq, X, X> : std::type_identity<Seq> {};

template <typename Seq, typename Seq::value_type X, typename Seq::value_type Y>
using index_swap_t = typename index_swap<Seq, X, Y>::type;

///////////////////////////////////////////////////////////////////////////////
template <auto Int_> using make_int_t = std::integral_constant<decltype(Int_), Int_>;

///////////////////////////////////////////////////////////////////////////////
template <typename T> constexpr bool is_char_v = boost::mp11::mp_find
<
    boost::mp11::mp_list
    <
          char
        , unsigned char
        , signed char
        , wchar_t
    >,
    std::remove_cvref_t<T>
>::value;

namespace enum_
{

template <c::enumerable T>
static constexpr auto no_mask = std::numeric_limits<mask_type_t<T>>::max();

}

}