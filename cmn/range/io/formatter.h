#pragma once

#include <format>
#include <string>
#include <stdexcept>
#include <ranges>
#include <type_traits>
#include <concepts>

#include <cmn/io/formatter.h>

namespace std
{

template
<
      std::ranges::input_range R
    , typename Char
>
    requires std::formattable<R, Char> && std::same_as<std::remove_cvref_t<R>, R>

struct formatter<cmn::io::fmt<R>, Char>
{
    using underlying_formatter_type = std::range_formatter<R, Char>;

    template <typename ParseContext>
    constexpr auto parse(ParseContext &ctx) -> typename ParseContext::iteratror
    {
        
    }
};

}
