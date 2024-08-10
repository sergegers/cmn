#pragma once

#include <concepts>
#include <type_traits>

#include <boost/optional.hpp>
#include <boost/mp11.hpp>

#include <cmn/shared/meta/concepts.h>       // c::enumerable<>
#include <cmn/shared/algorithm/detail/result.h>

namespace cmn
{

// https://stackoverflow.com/a/46282159/8452129

template
<
      c::enumerable T
    , T ... Idss_
    , typename F
    , typename Def
    , typename... Args
>
constexpr auto unordered_switch_
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
    using return_type = std::common_type_t
    <
          decltype(f(std::integral_constant<T, Idss_>{}, std::forward<Args>(args)...))...
        , decltype(default_(index, std::forward<Args>(args)...))
    >;

    boost::optional<return_type> ret{};
    if
    (
        (... || 
            (
                index == Idss_ ? 
                    ret = detail::result_{ std::forward<F>(f) }
                            .with_args(std::integral_constant<T, Idss_>{}, std::forward<Args>(args)...)
                        (), true
                    : 
                    false
            )
        )
    )
        return *ret;
    else
        return detail::result_{ default_ }.with_args(index, std::forward<Args>(args)...)();
}

template
<
      std::integral T
    , T ... Idss_
    , typename F
    , typename Def
    , typename... Args
>
constexpr auto unordered_switch_
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
    return unordered_switch_(index, boost::mp11::integer_sequence<T, Idss_...>{},
        std::forward<F>(f), std::forward<Def>(default_), std::forward<Args>(args)...);
}


}