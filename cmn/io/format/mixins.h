#pragma once

#include <ostream>
#include <sstream>
#include <algorithm>
#include <utility>
#include <type_traits>
#include <string>

#include <cmn/meta/concepts.h>

namespace cmn::io::mix
{

template
<
      typename T
    , typename Char
>
    requires c::printable<T, Char, std::char_traits<Char>>

struct out_to_stream
{
    using ostream_type = std::basic_ostream<Char>;

    constexpr auto prepare_stream(ostream_type &ostr) const -> ostream_type & = delete;

    template<typename Self, typename FmtContext>
    constexpr auto format(this Self const &self_, T const &t, FmtContext &ctx) -> typename FmtContext::iterator
    {
        std::basic_ostringstream<Char> ostr;
        self_.prepare_stream(ostr) << t;

        return std::ranges::copy(std::move(ostr).str(), ctx.out()).out;
    }    
};

//-----------------------------------------------------------------------------
template
<
      typename T
    , typename Char
>
struct skip_parse
{
    template<typename Self, typename ParseContext>
    constexpr auto parse(this Self const &, ParseContext &ctx) -> typename ParseContext::iterator
    {
        return ctx.begin();
    }    
};

}
