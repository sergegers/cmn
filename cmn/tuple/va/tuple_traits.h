#pragma once

#include <tuple>

#include "concepts.h"
#include "tuple_utils.h"

namespace cmn::va
{

////////////////////////////////////////////////////////////////////////////////
// Ellipsis metafunctions
////////////////////////////////////////////////////////////////////////////////

// metafunction remove_ellipsis
template <typename VaSig> struct remove_ellipsis;

template <typename Res, typename... EArgs>
struct remove_ellipsis<Res(EArgs...)>
{
private:
    // metafunction make_sig
    template <typename Rs, typename ArgTuple> struct make_sig;

    template <typename Rs, typename... Args>
    struct make_sig < Rs, std::tuple<Args...>>
    {
        using type = auto (Args...) -> Res;
    };

    template <typename Rs, typename ArgTuple>
    using make_sig_t = typename make_sig<Rs, ArgTuple>::type;

    using pure_args_type = tuple_args_t<EArgs...>;
public:
    using type = make_sig_t<Res, pure_args_type>;
};

template <typename VaSig>
using remove_ellipsis_t = typename remove_ellipsis<VaSig>::type;

// metafunction push_front_ellipsis
template <typename VaSig> struct push_front_ellipsis;

template <typename Res, typename... EArgs>
struct push_front_ellipsis<auto (EArgs...) -> Res>
{
    using type = auto (ellipsis, EArgs...) -> Res;
};

template <typename VaSig>
using push_front_ellipsis_t = typename push_front_ellipsis<VaSig>::type;

// has ellipsis
template <typename VaSig>
struct has_ellipsis;

template <typename Res, typename... Args>
struct has_ellipsis<auto (Args...) -> Res>: std::disjunction<std::is_same<Args, ellipsis>...> {};

template <typename VaSig>
constexpr bool has_ellipsis_v = has_ellipsis<VaSig>::value;

}
