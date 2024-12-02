#pragma once

#include <concepts>
#include <type_traits>
#include <cstddef>
#include <bit>
#include <string>
#include <tuple>
#include <limits>

#include <cmn/meta/concepts.h>
#include <cmn/meta/type_traits.h>
#include <cmn/meta/symbols.h>
#include <cmn/util/fixed_string.h>

namespace cmn
{

inline namespace literals
{

// TODO: replace with C++ 23 literals
constexpr auto operator ""_uz (unsigned long long n) -> std::size_t { return n; }
constexpr auto operator ""_z (unsigned long long n) -> std::ptrdiff_t { return n; }

}

////////////////////////////////////////////////////////////////////////////////
//
// https://www.bfilipek.com/2019/02/2lines3featuresoverload.html?m=1
//
// Use case:
//
//    boost::variant<int, float, std::string> intFloatString { "Hello" };
//    boost::apply_visitor
//    (
//      overloaded
//      {
//        [](int& i) { i*= 2; },
//        [](float& f) { f*= 2.0f; },
//        [](std::string& s) { s = s + s; }
//      },
//      intFloatString
//    );
//
////////////////////////////////////////////////////////////////////////////////
template<typename... Ts> struct overloaded : Ts... { using Ts::operator()...; };
// Not needed in C++ 20
//template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

///////////////////////////////////////////////////////////////////////////////
//
// bsf bit scan forward - returns less significant set bit position index
//
///////////////////////////////////////////////////////////////////////////////
constexpr auto bsf(std::unsigned_integral auto value) -> std::size_t
{
    return static_cast<std::size_t>(std::countr_zero(value));
}

///////////////////////////////////////////////////////////////////////////////
//
// bsr bit scan reverse - returns most significant set bit position index
//
///////////////////////////////////////////////////////////////////////////////
constexpr auto bsr(std::unsigned_integral auto value) -> std::size_t
{
    return static_cast<std::size_t>(std::bit_width(value) - 1);
}

//
// https://stackoverflow.com/a/4609795/8452129
//
template <std::integral Int>
constexpr auto sgn(Int val) -> std::make_signed_t<Int>
{
    static constexpr auto zero = Int{ 0 };
    return (zero < val) - (val < zero);
}


// https://stackoverflow.com/q/52591393/8452129
template <std::unsigned_integral T> constexpr auto log10(T param) -> std::size_t
{
    using namespace literals;
    std::size_t result{ 1_uz };

     while(T{} != (param /= T{ 10 })) ++result;
     return result;
}

template <std::unsigned_integral T> constexpr auto pow10(T param) -> std::size_t
{
    using namespace literals;
    std::size_t result{ 1_uz };

    while (T{} != param--) result *= 10_uz;
    return result;
}

///////////////////////////////////////////////////////////////////////////////
//
// int to string conversion
//
namespace detail
{

template <std::unsigned_integral auto N_, typename Char, std::size_t Idx_>
consteval auto digit_to_char(int_<Idx_>) -> Char
{
    auto const count = log10(N_);
    auto const pos = count - Idx_ - 1;
    auto digit = N_ % pow10(pos + 1);
    digit -= N_ % pow10(pos);
    digit /= pow10(pos);
    return static_cast<Char>(digit + 0x30);
}

template <std::unsigned_integral auto N_, typename Char, typename CharTraits, std::size_t... Idss_>
constexpr auto itoa_digits(std::index_sequence<Idss_...>) -> basic_fixed_string<Char, log10(N_), CharTraits>
{
    auto const count = log10(N_);
    Char const s_literal[count + 1]
    {
          digit_to_char<N_, Char>(int_<Idss_>{})...
        , to_char(symbols<Char, CharTraits>::ends)
    };
    return { s_literal };
}

template <std::unsigned_integral auto N_, typename Char, typename CharTraits, std::size_t... Idss_>
constexpr auto neg_itoa_digits(std::index_sequence<Idss_...>) -> basic_fixed_string<Char, log10(N_) + 1, CharTraits>
{
    using symbols_type = symbols<Char, CharTraits>;
    auto const count = log10(N_);
    Char const s_literal[count + 2]
    {
          to_char(symbols_type::minus)
        , digit_to_char<N_, Char>(int_<Idss_>{})...
        , to_char(symbols_type::ends)
    };
    return { s_literal };
}

}

template
<
      std::integral auto N_
    , typename Char = char
    , typename CharTraits = std::char_traits<Char>
>
constexpr auto itoa() -> c::basic_string<Char, CharTraits> auto
{
    using unsigned_type = std::make_unsigned_t<decltype(N_)>;
    if constexpr (N_ >= 0)
    {
        constexpr auto p = static_cast<unsigned_type>(N_);
        constexpr auto count = log10(p);
        using result_type = basic_fixed_string<Char, count, CharTraits>;
        return result_type { detail::itoa_digits<p, Char, CharTraits>(std::make_index_sequence<count>{}) };
    }
    else
    {
        constexpr auto p = static_cast<unsigned_type>(-N_);
        constexpr auto count = log10(p);
        using result_type = basic_fixed_string<Char, count + 1, CharTraits>;
        return result_type { detail::neg_itoa_digits<p, Char, CharTraits>(std::make_index_sequence<count>{}) };
    }
}

//-----------------------------------------------------------------------------
template <typename T>
struct assert_type_complete
{
    static_assert(c::complete<T>);
};

//-----------------------------------------------------------------------------
template <std::size_t Idx_, typename... Args>
constexpr decltype(auto) forward_nth(Args &&... args)
{
    return []<c::instance_of<std::tuple> ArgTpl>(ArgTpl &&arg_tpl) -> decltype(auto)
    {
        return std::get<Idx_>(std::forward<ArgTpl>(arg_tpl));
    }
        (std::forward_as_tuple(std::forward<Args>(args)...))
    ;
}

//-----------------------------------------------------------------------------
//
// static_cast<> to Dst but keep cvr modifiers
//
//-----------------------------------------------------------------------------
template <typename Dst, typename Src>
constexpr auto keep_cvr_cast(Src &&src) noexcept-> copy_cvr_t<Src &&, Dst>
{
    using result_type = copy_cvr_t<Src &&, Dst>;
    return static_cast<result_type>(std::forward<Src>(src));
}

//-----------------------------------------------------------------------------
template <c::enumerable T>
constexpr auto to_interop(T t) -> interop_type_t<T>
{
    return static_cast<interop_type_t<T>>(t);
}

//-----------------------------------------------------------------------------
constexpr auto lazy_to_interop(c::adapted_enum auto t) { return to_interop(t); }
// suppress double conversion
constexpr auto lazy_to_interop(c::enumerable auto t) { return t; }

//-----------------------------------------------------------------------------
template <c::enumerable T>
constexpr auto to_underlying(T t) { return static_cast<underlying_type_t<T>>(t); }

////////////////////////////////////////////////////////////////////////////////
//
// Metafunction make_integer_range
// make integer sequence [B, E)
//
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
          , to_underlying(E_) - to_underlying(B_) /* half opened range */
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

///////////////////////////////////////////////////////////////////////////////
//
// same as boost::mp11::mp_from_sequence but supports c::enumerable types
//
///////////////////////////////////////////////////////////////////////////////
template <typename Seq> using mp_from_sequence = typename detail::mp_from_sequence_impl<Seq>::type;

///////////////////////////////////////////////////////////////////////////////
//
// from_std_to_mp_sequence<>
//
///////////////////////////////////////////////////////////////////////////////
namespace detail
{

using namespace boost::mp11;

template <typename T> struct from_std_to_mp_sequence_impl;

template <c::enumerable I, I... Idss_>
struct from_std_to_mp_sequence_impl<std::integer_sequence<I, Idss_...>>
{
    using type = integer_sequence<I, Idss_...>;
};

template <c::enumerable I, I... Idss_>
struct from_std_to_mp_sequence_impl<integer_sequence<I, Idss_...>>
{
    using type = integer_sequence<I, Idss_...>;
};

}

template <typename T> using from_std_to_mp_sequence = typename detail::from_std_to_mp_sequence_impl<T>::type;

///////////////////////////////////////////////////////////////////////////////
//
// enumerable utils
//
///////////////////////////////////////////////////////////////////////////////

template <c::enumerable T> struct mask_type: interop_type<T> {};
template <c::scoped_enum T> struct mask_type<T>: std::type_identity<T> {};

// could be integer or enum
template <c::enumerable T> using mask_type_t = typename mask_type<T>::type;

//-----------------------------------------------------------------------------
template <c::enumerable T>
static constexpr auto no_mask = static_cast<mask_type_t<T>>(
    std::numeric_limits<std::make_unsigned_t<interop_type_t<T>>>::max());

}
