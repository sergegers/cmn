#pragma once

#include <ranges>
#include <format>

#include <boost/type_traits/promote.hpp>

#include <cmn/io/format.h>

template <std::ranges::input_range R>
struct cmn::io::traits<R>
{
    static constexpr boost::promote_t<list_options_t> fmt_options = lo_brackers | lo_separator;
};
