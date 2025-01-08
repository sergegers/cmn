#pragma once

#include <format>
#include <concepts>

#include "strong_typedef.h"

namespace std
{

template <typename U, std::integral T, T Default_, typename Char>
struct formatter<cmn::strong_typedef_impl<U, T, Default_>, Char>
{
    using underlying_formatter = std::formatter<T, Char>;

    underlying_formatter m_underlying_formatter;

    template<typename ParseContext>
    constexpr auto parse(ParseContext &ctx) -> typename ParseContext::iterator
    {
        return m_underlying_formatter.parse(ctx);
    }

    template<typename FmtContext>
    constexpr auto format(U const &u, FmtContext &ctx) const -> typename FmtContext::iterator
    {
        return m_underlying_formatter.format(static_cast<T>(u), ctx);
    }
};

template <std::integral T, typename Tag, T Default_, typename Char>
struct formatter<cmn::strong_typedef<T, Tag, Default_>, Char>
{
    using strong_typedef_type = cmn::strong_typedef<T, Tag, Default_>;
    using underlying_formatter = std::formatter<T, Char>;

    underlying_formatter m_underlying_formatter;

    template<typename ParseContext>
    constexpr auto parse(ParseContext &ctx) -> typename ParseContext::iterator
    {
        return m_underlying_formatter.parse(ctx);
    }

    template<typename FmtContext>
    constexpr auto format(strong_typedef_type const &t, FmtContext &ctx) const -> typename FmtContext::iterator
    {
        return m_underlying_formatter.format(static_cast<T>(t), ctx);
    }
};

}
