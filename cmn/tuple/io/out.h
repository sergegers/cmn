#pragma once

#include <boost/fusion/algorithm/iteration/fold.hpp>

#if __has_include(<boost/mp11/concepts.hpp>)
#   include <boost/mp11/concepts.hpp>
#else
#   include <cmn/meta/boost/mp11/concepts.hpp>
#endif

#if __has_include(<boost/fusion/concepts.hpp>)
#   include <boost/fusion/concepts.hpp>
#else
#   include <cmn/meta/boost/fusion/concepts.hpp>
#endif

#include <cmn/error/exception.h>

#include "manip.h"

namespace cmn::tuple_::io
{

struct out_fn
{
    template <c::list_sink_format_options FmtOpt>
    constexpr auto operator ()(boost::c::fus_sequence auto seq, FmtOpt const& fmt_opt, auto out_it) const -> void
    {
        using char_type = cmn::io::char_type_t<FmtOpt>;
        using char_traits_type = cmn::io::char_traits_type_t<FmtOpt>;

        auto const& open = fmt_opt.open;
        auto const& close = fmt_opt.close;
        auto const& delim = fmt_opt.delimiter;

        std::ranges::copy(open, out_it);

        out_it = boost::fusion::fold
        (
            seq,
            out_it,
            [first = true, &delim](auto out_it, auto const &elem) mutable
            {
                if (!first)
                    std::ranges::copy(delim, out_it);
                else
                    first = false;

                std::format_to(out_it, sym::def_fmt_1_arg.value<char_type, char_traits_type>().m_data._Elems, elem);

                return out_it;
            }
        );
        
        std::ranges::copy(close, out_it);
    }
}

inline constexpr out{};

}
