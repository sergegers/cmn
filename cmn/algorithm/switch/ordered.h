#pragma once

#include <concepts>
#include <type_traits>

#include <boost/mp11.hpp>

#include <cmn/meta/concepts.h>       // c::enumerable<>
#include <cmn/algorithm/binary_find.h>
#include <cmn/algorithm/detail/result.h>

namespace cmn
{

template
<
    c::enumerable T
    , T ... Idss_
    , typename F
    , typename Def
    , typename... Args
>
constexpr auto ordered_switch_
(
    T index
    , boost::mp11::integer_sequence<T, Idss_...>
    , F &&f
    , Def &&default_ = f
    , Args &&... args
)
    requires
       (... && std::invocable<F, std::integral_constant<T, Idss_>, Args...>)
    && std::invocable<Def, T, Args...>    // default case
{
    using seq_type = boost::mp11::integer_sequence<T, Idss_...>;
    using mp_list_type = boost::mp11::mp_from_sequence<seq_type>;

    auto [res, pos] = binary_find_if_mp11<mp_list_type, digit_comparer>
    (
          index
        , std::forward<F>(f)
        , std::forward<Args>(args)...
    );

    return -1 == pos ? detail::result_{ default_ }.with_args(index, std::forward<Args>(args)...)() : std::move(res);
}

template
<
      std::integral T
    , T ... Idss_
    , typename F
    , typename Def
    , typename... Args
>
constexpr auto ordered_switch_
(
     T index
    , std::integer_sequence<T, Idss_...>
    , F &&f
    , Def &&default_ = f
    , Args &&... args
)
    requires
       (... && std::invocable<F, std::integral_constant<T, Idss_>, Args...>)
    && std::invocable<Def, T, Args...>    // default case
{
    return ordered_switch_(index, boost::mp11::integer_sequence<T, Idss_...>{},
        std::forward<F>(f), std::forward<Def>(default_), std::forward<Args>(args)...);
}

}