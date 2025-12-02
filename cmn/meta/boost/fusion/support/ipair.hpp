#pragma once

#include <cstddef>
#include <type_traits>

#include <boost/fusion/support/pair.hpp>

namespace boost::fusion
{

template <std::size_t Key_, typename Value>
using ipair = pair<std::integral_constant<std::size_t, Key_>, Value>;

//-----------------------------------------------------------------------------
template <std::size_t Key_>
using ikey = std::integral_constant<std::size_t, Key_>;

template <std::size_t Key_, typename Value>
constexpr auto make_ipair(Value &&value)
{
    return make_pair<ikey<Key_>>(std::forward<Value>(value));
}

}
