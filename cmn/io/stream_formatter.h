#pragma once

#include <format>
#include <ostream>
#include <sstream>
#include <algorithm>
#include <utility>

namespace cmn::io
{

template <typename D, typename T, typename Char>
    requires std::formattable<std::remove_cvref_t<T>, Char>
struct stream_formatter<T, Char>
{
    using derived_type = D;
    using ostream_type = std::basic_ostream<Char>;

    constexpr auto prepare_stream(ostream_type &ostr) -> ostream_type & = delete;

    template<typename FmtContext>
    constexpr auto format(cmn::io::fmt<T> const &t, FmtContext &ctx) const -> typename FmtContext::iterator
    {
        std::basic_ostringstream<Char> ostr;
        derived_type::prepare_stream(ostr) << t;

        return std::ranges::copy(std::move(ostr).str(), ctx.out()).out;
    }    
};

}
