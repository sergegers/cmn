#pragma once

#include <concepts>
#include <type_traits>
#include <limits>
#include <utility>      // integer_sequence<>

#include <boost/mp11.hpp>

#include <cmn/meta/concepts.h>       // c::enumerable<>
#include <cmn/meta/type_traits.h>    // underlying_type_t<>

#include <cmn/algorithm/switch/unordered.h>
#include <cmn/algorithm/switch/ordered.h>
#include <cmn/algorithm/switch/sequenced.h>
#include <cmn/algorithm/detail/result.h>

namespace cmn
{

namespace detail
{

template <typename Int, Int Val0_, Int Val1_, Int... Vals_>
consteval auto sequenced(std::integer_sequence<Int, Val0_, Val1_, Vals_...>) -> bool
{
    return []<Int... OrdVals_, Int... OrdIdss_>
    (
          std::integer_sequence<Int, OrdVals_...>
        , std::integer_sequence<Int, OrdIdss_...>
    ) constexpr
    {
        // for every case label value == Val0_ + step * index
        constexpr auto step = Val1_ - Val0_;
        return ((OrdVals_ == Val0_ + OrdIdss_ * step) && ...);
    }
    (
          std::integer_sequence<Int, Val0_, Val1_, Vals_...>{}
        // all case label count == Vals_... and Val0_ and Val1_
        , std::make_integer_sequence<Int, sizeof... (Vals_) + 2>{}
    );
}

template <typename Int, Int Val_, Int... Vals_>
consteval auto ordered(std::integer_sequence<Int, Val_, Vals_...>) -> bool
{
    constexpr auto max = std::numeric_limits<Int>::max();
    static_assert((Val_ < max) && ((Vals_ < max) && ...));

    // to compare every case label value with its next sibling
    // shift case label sequence to left (drop first case label) and
    // insert max_int instead the last label (so any label value will be
    // less then max_int if the is no case labels with max_int values, see
    // static_assert() above). If for any case label Val_(N) < Val_(N+1)
    // the sequence is ordered.
    return []<Int... Lhss_, Int... Rhss_>
        (
              std::integer_sequence<Int, Lhss_...>
            , std::integer_sequence<Int, Rhss_...>
        ) constexpr noexcept -> bool
    {
        return ((Lhss_ < Rhss_) && ...);
    }
    (
          std::integer_sequence<Int, Val_, Vals_...>{}
        , std::integer_sequence<Int, Vals_..., max>{}
    );
}

}

// boost.mp1 sequence version
template
<
      c::enumerable T
    , T ... Idss_
    , typename F
    , typename Def
    , typename... Args
>
constexpr auto switch_
(
    T index
    , boost::mp11::integer_sequence<T, Idss_...> case_labels
    , F &&f
    , Def &&default_ = f
    , Args &&... args
)
    requires
       (... && std::invocable<F, std::integral_constant<T, Idss_>, Args...>)
    && std::invocable<Def, T, Args...>    // default case
{
    if constexpr (sizeof...(Idss_) == 0)    // no case labels
    {
        return detail::result_{ std::forward<Def>(default_) }
                .with_args(index, std::forward<Args>(args)...)
                ()
        ;
    }
    else if constexpr (sizeof...(Idss_) == 1) // one case label
    {
        using namespace boost::mp11;
        if
        (
            // index == Idss__...[0]
            []<T Idx_, T... Idss__>(integer_sequence<T, Idx_, Idss__...>, T t) constexpr noexcept 
                { return t == Idx_;  }
            (make_integer_sequence<T, Idss_...>{}, index)
        )
            return detail::result_{ f }
                    .with_args(std::integral_constant<T, Idss_...>{}, std::forward<Args>(args)...)
                    ()
                ;
        else
            return detail::result_{ default_ }
                    .with_args(index, std::forward<Args>(args)...)
                    ()
                ;
    }
    else if constexpr (sizeof...(Idss_) == 2) // two case labels - they are always sequenced
    {
        return sequenced_switch_
        (
              index
            , case_labels
            , std::forward<F>(f)
            , std::forward<Def>(default_)
            , std::forward<Args>(args)...
        );
    }
    else 
    {
        using namespace detail;
        using int_type = underlying_type_t<T>;  

        if constexpr (sequenced(std::integer_sequence<int_type, static_cast<int_type>(Idss_)...>{}))
        {
            return sequenced_switch_
            (
                  index
                , case_labels
                , std::forward<F>(f)
                , std::forward<Def>(default_)
                , std::forward<Args>(args)...
            );
        }
        else if constexpr (ordered(std::integer_sequence<int_type, static_cast<int_type>(Idss_)...>{}))
        {
            return ordered_switch_
            (
                  index
                , case_labels
                , std::forward<F>(f)
                , std::forward<Def>(default_)
                , std::forward<Args>(args)...
            );
        }
        else
        {
            return unordered_switch_
            (
                  index
                , case_labels
                , std::forward<F>(f)
                , std::forward<Def>(default_)
                , std::forward<Args>(args)...
            );
        }
    }
}

// std sequence version
template
<
      std::integral T
    , T ... Idss_
    , typename F
    , typename Def
    , typename... Args
>
constexpr auto switch_
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
    return switch_(index, boost::mp11::integer_sequence<T, Idss_...>{},
        std::forward<F>(f), std::forward<Def>(default_), std::forward<Args>(args)...);
}

}