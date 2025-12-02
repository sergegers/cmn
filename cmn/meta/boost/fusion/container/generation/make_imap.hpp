#pragma once

#include <cstddef>
#include <type_traits>

#include <boost/fusion/container/generation/make_map.hpp>

#if __has_include(<boost/fusion/container/imap.hpp>)
#   include <boost/fusion/container/imap.hpp>
#else
#   include <cmn/meta/boost/fusion/container/imap.hpp>
#endif

namespace boost::fusion
{

template <std::size_t... Keys_, typename... Values>
constexpr auto make_imap(Values &&...values)
{
    return make_map<std::integral_constant<std::size_t, Keys_>...>(std::forward<Values>(values)...);
}

}
