#pragma once

#include <ostream>
#include <sstream>
#include <algorithm>
#include <utility>
#include <type_traits>
#include <string>

#include <cmn/meta/concepts.h>

#include <cmn/io/format/context_traits.h>

namespace cmn::io::mix
{

template
<
      typename T
    , typename Char
    , typename CharTraits = std::char_traits<Char>
>
    requires c::printable<T, Char, CharTraits>

struct out_to_stream
{
    using ostream_type = std::basic_ostream<Char, CharTraits>;

    constexpr auto prepare_stream(this auto const &, ostream_type &ostr) -> ostream_type & = delete;

    template<typename FmtContext>
    constexpr auto format(this auto &self_, T const &t, FmtContext &ctx) -> context_iterator_t<FmtContext>
    {
        std::basic_ostringstream<Char, CharTraits> sstr;
        self_.prepare_stream(sstr) << t;

        return std::ranges::copy(std::move(sstr).str(), ctx.out()).out;
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
    template<typename ParseContext>
    constexpr auto parse(this auto const &, ParseContext &ctx) -> context_iterator_t<ParseContext>
    {
        return ctx.begin();
    }    
};

}
