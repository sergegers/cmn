#pragma once

#include <type_traits>

// boost
#include <boost/optional/optional_fwd.hpp>
// boost.type_traits
#include <boost/type_traits/function_traits.hpp>
// boost.signals2
#include <boost/signals2/signal.hpp>
// boost.fusion
#include <boost/fusion/functional/invocation/invoke.hpp>

namespace cmn::va
{

namespace detail
{

// using fire_event_result_type_t 
template <typename Sig>
using fire_event_result_type_t =
    std::conditional_t
    <
        std::is_same_v <typename boost::function_traits<Sig>::result_type, void>,
        void,
        boost::optional<typename boost::function_traits<Sig>::result_type>
    >
;

template
<
    typename Sig,
    typename Combiner,
    typename Group,
    typename GroupCompare,
    typename SlotFunction,
    typename ExtendedSlotFunction,
    typename Mutex
>
auto make_empty_fire_event_result
(
    boost::signals2::signal
    <
        Sig, 
        Combiner, 
        Group, 
        GroupCompare, 
        SlotFunction, 
        ExtendedSlotFunction,
        Mutex
    > &
) noexcept -> fire_event_result_type_t<Sig>
{ 
    return {}; 
}

template
<
    typename Combiner,
    typename Group,
    typename GroupCompare,
    typename SlotFunction,
    typename ExtendedSlotFunction,
    typename Mutex,
    typename... Args
>
auto make_empty_fire_event_result
(
    boost::signals2::signal
    <
        void (Args ...),
        Combiner,
        Group,
        GroupCompare,
        SlotFunction,
        ExtendedSlotFunction,
        Mutex
    > &
) noexcept -> void {}

} 

// fire_event() for va::tuple_view
template
<
    typename Combiner,
    typename Group,
    typename GroupCompare,
    typename SlotFunction,
    typename ExtendedSlotFunction,
    typename Mutex,
    typename Res,
    typename... Args,
    typename ERes,
    typename... EArgs
>
auto fire_event
(
    boost::signals2::signal
    <
        auto (Args...) -> Res,
        Combiner, 
        Group, 
        GroupCompare, 
        SlotFunction, 
        ExtendedSlotFunction,
        Mutex
    > &sig,
    tuple_view<auto (EArgs...) -> ERes> const &tv
) -> detail::fire_event_result_type_t<auto (Args...) -> Res>
{
    // compiler couldn't properly deduce one signature for signal and tuple, so we're 
    // remaining both of them as independent templates and then check their conformance manually
    static_assert
    (
        std::same_as<auto (Args...) -> Res, remove_ellipsis_t<auto (EArgs...) -> ERes>>,
        "Mismatch between signal and tuple signatures"
    );

    if (sig.empty()) return detail::make_empty_fire_event_result(sig);  // empty signal optimization

    auto call_signal = 
        [&sig](auto &&... args)
        {
            return sig(args...);
        }
    ;

    return boost::fusion::invoke(std::move(call_signal), tv);
}

// fire_event() for std::tuple const/nonconst versions
template
<
    typename Combiner,
    typename Group,
    typename GroupCompare,
    typename SlotFunction,
    typename ExtendedSlotFunction,
    typename Mutex,
    typename Res,
    typename... Args
>
auto fire_event
(
    boost::signals2::signal
    <
        Res(Args...),
        Combiner, 
        Group, 
        GroupCompare, 
        SlotFunction, 
        ExtendedSlotFunction,
        Mutex
    > &sig,
    std::tuple<Args...> const &t
) -> detail::fire_event_result_type_t<auto (Args...) -> Res>
{
    if (sig.empty()) return detail::make_empty_fire_event_result(sig);  // empty signal optimization

    auto call_signal = 
        [&sig](auto &&... args)
        {
            return sig(std::forward<decltype(args)>(args)...);
        }
    ;

    return boost::fusion::invoke(std::move(call_signal), t);
}

template
<
    typename Combiner,
    typename Group,
    typename GroupCompare,
    typename SlotFunction,
    typename ExtendedSlotFunction,
    typename Mutex,
    typename Res,
    typename... Args
>
auto fire_event
(
    boost::signals2::signal
    <
        auto (Args...) -> Res,
        Combiner, 
        Group, 
        GroupCompare, 
        SlotFunction, 
        ExtendedSlotFunction,
        Mutex
    > &sig,
    tuple<auto (Args...) -> Res> &t
) -> detail::fire_event_result_type_t<auto (Args...) -> Res>
{
    if (sig.empty()) return detail::make_empty_fire_event_result(sig);  // empty signal optimization

    auto call_signal = 
        [&sig](auto &&... args)
        {
            return sig(std::forward<decltype(args)>(args)...);
        }
    ;

    return boost::fusion::invoke(std::move(call_signal), t);
}

}
