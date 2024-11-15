#pragma once

#include <cstddef>
#include <bit>
#include <concepts>
#include <string>

#include <cmn/meta/concepts.h>
#include <cmn/meta/symbols.h>
#include <cmn/meta/type_traits.h>
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

namespace enum_
{

//-----------------------------------------------------------------------------
template <c::enumerable T>
constexpr auto to_interop_type(T t) -> interop_type_t<T>
{
    return static_cast<interop_type_t<T>>(t);
}

}

}
