#pragma once

#include <type_traits>
#include <utility>
#include <limits>

#include <boost/mp11.hpp>

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

//-----------------------------------------------------------------------------
//
// Trait template <typename T> copy_const
//
//-----------------------------------------------------------------------------
template <typename Src, typename Dst> struct copy_const: std::type_identity<Dst> {};
template <typename Src, typename Dst> struct copy_const<Src, Dst const>: std::type_identity<Dst> {};
template <typename Src, typename Dst> struct copy_const<Src const, Dst>: std::type_identity<Dst const> {};

template <typename Src, typename Dst> using copy_const_t = typename copy_const<Src, Dst>::type;
//-----------------------------------------------------------------------------
//
// Trait template <typename T> copy_volatile
//
//-----------------------------------------------------------------------------
template <typename Src, typename Dst> struct copy_volatile: std::type_identity<Dst> {};
template <typename Src, typename Dst> struct copy_volatile<Src, Dst volatile>: std::type_identity<Dst> {};
template <typename Src, typename Dst> struct copy_volatile<Src volatile, Dst>: std::type_identity<Dst volatile> {};

template <typename Src, typename Dst> using copy_volatile_t = typename copy_volatile<Src, Dst>::type;

//-----------------------------------------------------------------------------
//
// Trait template <typename T> copy_lvalue_reference
//
//-----------------------------------------------------------------------------
template <typename Src, typename Dst> struct copy_lvalue_reference: std::type_identity<Dst> {};
template <typename Src, typename Dst> struct copy_lvalue_reference<Src, Dst &>: std::type_identity<Dst> {};
template <typename Src, typename Dst> struct copy_lvalue_reference<Src &, Dst>: std::type_identity<Dst &> {};

template <typename Src, typename Dst> using copy_lvalue_reference_t = typename copy_lvalue_reference<Src, Dst>::type;

//-----------------------------------------------------------------------------
//
// Trait template <typename T> copy_rvalue_reference
//
//-----------------------------------------------------------------------------
template <typename Src, typename Dst> struct copy_rvalue_reference: std::type_identity<Dst> {};
template <typename Src, typename Dst> struct copy_rvalue_reference<Src, Dst &&>: std::type_identity<Dst> {};
template <typename Src, typename Dst> struct copy_rvalue_reference<Src &&, Dst>: std::type_identity<Dst &&> {};

template <typename Src, typename Dst> using copy_rvalue_reference_t = typename copy_rvalue_reference<Src, Dst>::type;

//-----------------------------------------------------------------------------
//
// Trait template <typename T> copy_reference
//
// NOTE: because references are collapsed to minimize type instantiation do not use
// copy_lvalue_reference_t, copy_rvalue_reference_t, but reimplement trait
//
//-----------------------------------------------------------------------------
template <typename Src, typename Dst> struct copy_reference: std::type_identity<Dst> {};
template <typename Src, typename Dst> struct copy_reference<Src, Dst &>: std::type_identity<Dst> {};
template <typename Src, typename Dst> struct copy_reference<Src &, Dst>: std::type_identity<Dst &> {};
template <typename Src, typename Dst> struct copy_reference<Src, Dst &&>: std::type_identity<Dst> {};
template <typename Src, typename Dst> struct copy_reference<Src &&, Dst>: std::type_identity<Dst &&> {};

template <typename Src, typename Dst> using copy_reference_t = typename copy_reference<Src, Dst>::type;

//-----------------------------------------------------------------------------
//
// Trait template <typename T> copy_cvr
//
//-----------------------------------------------------------------------------
template <typename Src, typename Dst>
using copy_cvr = copy_reference<Src, copy_volatile_t<Src, copy_const_t<Src, std::remove_reference_t<Dst>>>>;

template <typename Src, typename Dst>
using copy_cvr_t = typename copy_cvr<Src, Dst>::type;

//-----------------------------------------------------------------------------
//
// Trait template <typename T> deep_add_const
//
//-----------------------------------------------------------------------------
template <typename T> struct deep_add_const : std::type_identity<T const> {};
template <typename T> struct deep_add_const<T &> : std::type_identity<T const &> {};
template <typename T> struct deep_add_const<T *> : std::type_identity<T const * const> {};

template <typename T> using deep_add_const_t = typename deep_add_const<T>::type;

//-----------------------------------------------------------------------------
//
// Trait template <typename T> deep_remove_const
//
//-----------------------------------------------------------------------------
template <typename T> struct deep_remove_const : std::type_identity<T> {};
template <typename T> struct deep_remove_const<T const> : std::type_identity<T> {};
template <typename T> struct deep_remove_const<T const &> : std::type_identity<T &> {};
template <typename T> struct deep_remove_const<T const *> : std::type_identity<T *> {};
template <typename T> struct deep_remove_const<T const * const> : std::type_identity<T *> {};

template <typename T>
using deep_remove_const_t = typename deep_remove_const<T>::type;

//-----------------------------------------------------------------------------
//
// Trait template <typename T> deep_copy_const
//
//-----------------------------------------------------------------------------
template <typename Src, typename Dst> struct deep_copy_const : copy_const<Src, Dst> {};
template <typename Src, typename Dst> struct deep_copy_const<Src, Dst &> : std::add_lvalue_reference<copy_const_t<Src, Dst>> {};
template <typename Src, typename Dst> struct deep_copy_const<Src, Dst *> : std::add_pointer<copy_const_t<Src, Dst>> {};

template <typename Src, typename Dst> using deep_copy_const_t = typename deep_copy_const<Src, Dst>::type;

///////////////////////////////////////////////////////////////////////////////
//
// extended underlying_type
//

///////////////////////////////////////////////////////////////////////////////
template <typename T>
struct underlying_type : std::type_identity<T> {};

// enum specialization
template <c::enum_ T>
struct underlying_type<T>: std::underlying_type<T> {};

// unit specialization
template <c::unit T>
struct underlying_type<T>: std::type_identity<typename T::underlying_type> {};

template <c::enumerable T>
using underlying_type_t = typename underlying_type<T>::type;

//-----------------------------------------------------------------------------
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
using make_enumerable_sequence = typename detail::add_and_convert
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
using make_index_sequence = make_enumerable_sequence<B_, E_>;

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
//
// make_integer_sequence_reverse
// make_index_sequence_reverse
//
// https://stackoverflow.com/a/51409050/8452129
//
///////////////////////////////////////////////////////////////////////////////
namespace detail
{

template <typename I, I... Is>
constexpr auto make_integer_sequence_reverse_impl(std::integer_sequence<I, Is...>)
{
    return std::index_sequence<sizeof...(Is) - 1U - Is...>{};
}

}

template <std::integral I, I Size_>
using make_integer_sequence_reverse = 
    decltype(detail::make_integer_sequence_reverse_impl(std::make_integer_sequence<decltype(Size_), Size_>{}));

template <std::size_t Size_>
using make_index_sequence_reverse = make_integer_sequence_reverse<std::size_t, Size_>;

///////////////////////////////////////////////////////////////////////////////
//
// enumerable_sequence_for<> can make mp11 sequence from any enumerable type
//
///////////////////////////////////////////////////////////////////////////////
template <c::enumerable auto Idx_, decltype(Idx_)... Idss_>
using mp_from_enumerables = boost::mp11::mp_list_c<decltype(Idx_), Idx_, Idss_...>;

///////////////////////////////////////////////////////////////////////////////
//
// mp_from_sequence<> can make mp11 sequence from any enumerable type
//
///////////////////////////////////////////////////////////////////////////////
namespace detail
{

using namespace boost::mp11;

template <typename Seq> struct mp_from_sequence_impl;

template <c::enumerable I, I... Idss_>
struct mp_from_sequence_impl<std::integer_sequence<I, Idss_...>>
{
    using type = mp_list_c<I, Idss_...>;
};

template <c::enumerable I, I... Idss_>
struct mp_from_sequence_impl<integer_sequence<I, Idss_...>>
{
    using type = mp_list_c<I, Idss_...>;
};

}

template <typename Seq> using mp_from_sequence = typename detail::mp_from_sequence_impl<Seq>::type;

///////////////////////////////////////////////////////////////////////////////
template <c::enumerable auto Int_> using int_ = std::integral_constant<decltype(Int_), Int_>;

///////////////////////////////////////////////////////////////////////////////
namespace detail
{

using namespace boost::mp11;

using char_types_t = mp_list
<
      char
    , unsigned char
    , signed char
    , wchar_t
>;

template <typename T>
consteval auto is_char() -> bool
{
    return mp_find<char_types_t, std::remove_cvref_t<T>>::value < mp_size<char_types_t>::value;
}

}

template <typename T> constexpr bool is_char_v = detail::is_char<T>();

///////////////////////////////////////////////////////////////////////////////
//
// Usable with 'if constexpr' expression for conditional compilation
//
template <bool Val_, typename...>
constexpr bool dependent_v = Val_;

namespace enum_
{

template <c::enumerable T>
static constexpr auto no_mask = std::numeric_limits<interop_type_t<T>>::max();

}

}