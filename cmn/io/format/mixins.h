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
      typename D
    , typename T
    , typename Char
>
    requires c::printable<T, Char, std::char_traits<Char>>

struct out_to_stream
{
    using ostream_type = std::basic_ostream<Char>;

    constexpr auto prepare_stream(ostream_type &ostr) const -> ostream_type & = delete;

    template<typename FmtContext>
    constexpr auto format(T const &t, FmtContext &ctx) const -> typename FmtContext::iterator
    {
        std::basic_ostringstream<Char> ostr;
        static_cast<D const &>(*this).prepare_stream(ostr) << t;

        return std::ranges::copy(std::move(ostr).str(), ctx.out()).out;
    }    
};

//-----------------------------------------------------------------------------
template
<
      typename D
    , typename T
    , typename Char
>
struct skip_parse
{
    template<typename ParseContext>
    constexpr auto parse(ParseContext &ctx) -> typename ParseContext::iterator
    {
        return ctx.begin();
    }    
};

}
