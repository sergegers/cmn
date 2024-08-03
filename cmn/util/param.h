#pragma once

#include <boost/core/use_default.hpp>
#include <boost/mp11.hpp>

namespace cmn
{

////////////////////////////////////////////////////////////////////////////////
//
// template parameter default placeholder
//
// Usage: 
//
// template <typename T = boost::use_default> class foo 
// {
//   def_type = decode_def_t<T, char>;
// };
//
// foo<> f0;        // def_type = char
// foo<int> f1;     // def_type = int
//
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
// Decoder function
//
template <typename Arg>
struct decode_param;

template <typename Arg>
using decode_param_t = typename decode_param<Arg>::type;

////////////////////////////////////////////////////////////////////////////////
template <typename Arg>
struct decode_param                     // default implementation remains argument type unchanged.
                                        // Useful for decoding dependent params.
{
    using type = Arg;
};

////////////////////////////////////////////////////////////////////////////////
//
// forbid default
//
struct no_default {};

template <>
struct decode_param<no_default> {};

////////////////////////////////////////////////////////////////////////////////
//
// placeholder with default value
//
template <typename Arg, typename Def = no_default> struct param {};

template <typename Arg, typename Def>
struct decode_param<param<Arg, Def>>: decode_param<Arg> {};

template <typename Def>
struct decode_param<param<boost::use_default, Def>>: decode_param<Def> {};

////////////////////////////////////////////////////////////////////////////////
//
//  lazy placeholder
//
template <typename Arg> struct lazy {};

template <typename Arg>   
struct decode_param<lazy<Arg>>: decode_param<typename Arg::type> {};

////////////////////////////////////////////////////////////////////////////////
//
//  lazy placeholders in mp11 style
//
template <template <typename...> typename F, typename... L> struct mp_lazy {};

template <template <typename...> typename F, typename... L>
struct decode_param<mp_lazy<F, L...>>:
    decode_param<F<decode_param_t<L>...>>
{};

//-----------------------------------------------------------------------------
template <typename Q, typename... L> struct mp_lazy_q {};

template <typename Q, typename... L>
struct decode_param<mp_lazy_q<Q, L...>>:
    decode_param
    <
          boost::mp11::mp_invoke_q<decode_param_t<Q>
        , decode_param_t<L>...>
    >
{};

////////////////////////////////////////////////////////////////////////////////
//
//  adapt any mp11 metafunction
//
template <template <typename...> typename F, typename... Args>
struct decode_param<F<Args...>>
{
    using type = F<decode_param_t<Args>...>;
};

////////////////////////////////////////////////////////////////////////////////
//
// Template default parameter decoder. Decode parameters and forward them
// to the template ClassT.
//
template <template <typename...> typename ClassT, typename... Params>
using decoder_t = ClassT<decode_param_t<Params>...>;

}
