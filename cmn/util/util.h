#pragma once

#include <cstddef>
#include <bit>
#include <concepts>

namespace cmn
{

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

}
