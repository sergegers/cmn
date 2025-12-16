#pragma once

#include <format>

#include <boost/type_traits/promote.hpp>

#if __has_include(<boost/fusion/concepts.hpp>)
#   include <boost/fusion/concepts.hpp>
#else
#   include <cmn/meta/boost/fusion/concepts.hpp>
#endif

#include <cmn/io/format.h>

template <boost::c::fus_sequence S>
struct cmn::io::traits<S>
{
    static constexpr boost::promote_t<list_options_t> fmt_options = lo_brackers | lo_separator;
};

