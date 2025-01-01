#pragma once

#include <ranges>
#include <format>

#include <boost/type_traits/promote.hpp>

#include <cmn/io/formatter.h>

namespace cmn::io
{

template <std::ranges::input_range R>
struct traits<R>
{
    static constexpr boost::promote_t<fmt_options_t> fmt_options = fo_brackers | fo_separator;
};


}
