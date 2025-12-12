#pragma once

#include <concepts>
#include <type_traits>

#include <boost/mp11.hpp>

#include <cmn/meta/concepts.h>      // c::enumerable<>
#include <cmn/meta/traits.h>   // underlying_type_t<>
#include <cmn/util/util.h>          // cmn::mp_from_sequence<>

#include <cmn/algorithm/at.h>
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
constexpr auto sequenced_switch_
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
    using namespace boost::mp11;

    using mp_list_type = cmn::mp_from_sequence<integer_sequence<T, Idss_...>>;
    using int_type = interop_type_t<T>;

    // convert to int_type to cover the case when the enum doesn't support arithmetic operations
    auto const idx = static_cast<int_type>(index);
    static constexpr auto min = static_cast<int_type>(mp_front<mp_list_type>::value);
    static constexpr auto max = static_cast<int_type>(mp_back<mp_list_type>::value);
    static constexpr auto step = static_cast<int_type>(mp_at_c<mp_list_type, 1>::value) - min;

    // index is outside case label interval or between two case labels
    if (idx < min || idx > max || (idx - min) % step != 0)  
        return detail::result_{ std::forward<Def>(default_) }.with_args(index, std::forward<Args>(args)...)();

    return at_mp11<mp_list_type>
    (
          static_cast<T>((idx - min) / step)
        , detail::result_
          {
              [&f, &args...](auto case_idx) 
              {
                    return std::forward<F>(f)(case_idx, std::forward<Args>(args)...);
              }
          }
    );
}

template
<
      std::integral T
    , T ... Idss_
    , typename F
    , typename Def
    , typename... Args
>
constexpr auto sequenced_switch_
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
    return sequenced_switch_(index, boost::mp11::integer_sequence<T, Idss_...>{},
        std::forward<F>(f), std::forward<Def>(default_), std::forward<Args>(args)...);
}

}