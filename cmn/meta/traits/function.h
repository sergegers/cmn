#pragma once

#include <type_traits>

// boost.traits
#include <boost/type_traits/function_traits.hpp>
// boost.preprocessor
#include <boost/preprocessor/repetition/repeat.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/inc.hpp>
// boost.mp11
#include <boost/mp11.hpp>

///////////////////////////////////////////////////////////////////////////////
//
// Function Traits
//
// NOTE: add missing cases on demand
//
///////////////////////////////////////////////////////////////////////////////

namespace cmn::function
{

////////////////////////////////////////////////////////////////////////////////
// template <typename Func, std::size_t N_> struct arg_type_c
// boost function traits extension
////////////////////////////////////////////////////////////////////////////////
template <typename Func, std::size_t N_> struct arg_type_c;

#define CHT_PP_FUNC_ARG_TYPE(z, N_, _)   \
    template <typename Func>   \
    struct arg_type_c<Func, N_>  \
    {   \
        using type = typename ::boost::function_traits<Func>::   \
            BOOST_PP_CAT(arg, BOOST_PP_CAT(BOOST_PP_INC(N_), _type));    \
    };

#ifndef CHT_PP_FUN_ARG_MAX_ARITY
    #define CHT_PP_FUN_ARG_MAX_ARITY    10
#endif

BOOST_PP_REPEAT(CHT_PP_FUN_ARG_MAX_ARITY, CHT_PP_FUNC_ARG_TYPE, nil)

#undef CHT_PP_FUNC_ARG_TYPE

template <typename Func, std::size_t N_>
using arg_type_c_t = arg_type_c<Func, N_>::type;

template <typename Func, typename N> 
struct arg_type : arg_type_c<Func, N::value> {};

template <typename Func, typename N> requires std::is_function_v<Func>
using arg_type_t = arg_type<Func, N>::type;

///////////////////////////////////////////////////////////////////////////////
template <typename Func> struct is_noexcept: std::false_type {};

template <typename Res, typename... Args>
struct is_noexcept<auto (Args...) noexcept -> Res> : std::true_type {};

template <typename Func> requires std::is_function_v<Func>
constexpr bool is_noexept_v = is_noexcept<Func>::value;

//-----------------------------------------------------------------------------
template <typename Func> struct add_noexcept;

template <typename Res, typename... Args>
struct add_noexcept<auto (Args...) -> Res>
{
    using type = auto (Args...) noexcept -> Res;
};

template <typename Res, typename... Args>
struct add_noexcept<auto (Args...) noexcept -> Res>
{
    using type = auto (Args...) noexcept -> Res;
};

template <typename Func> requires std::is_function_v<Func>
using add_noexcept_t = add_noexcept<Func>::type;

//-----------------------------------------------------------------------------
template <typename Func> struct remove_noexcept;

template <typename Res, typename... Args>
struct remove_noexcept<auto (Args...) noexcept -> Res>
{
    using type = auto (Args...) -> Res;
};

template <typename Res, typename... Args>
struct remove_noexcept<auto (Args...) -> Res>
{
    using type = auto (Args...) -> Res;
};

template <typename Func> requires std::is_function_v<Func>
using remove_noexcept_t = remove_noexcept<Func>::type;

//-----------------------------------------------------------------------------
//
// NOTE: couldn't use std::is_const_v<> on function types
//
template <typename Func> struct is_const: std::false_type {};

template <typename Res, typename... Args>
struct is_const<auto (Args...) const -> Res> : std::true_type {};

template <typename Func> requires std::is_function_v<Func>
constexpr bool is_const_v = is_const<Func>::value;

//-----------------------------------------------------------------------------
template <typename Func> struct add_const;

template <typename Res, typename... Args>
struct add_const<auto (Args...) -> Res>
{
    using type = auto (Args...) const -> Res;
};

template <typename Res, typename... Args>
struct add_const<auto (Args...) const -> Res>
{
    using type = auto (Args...) const -> Res;
};

template <typename Func> requires std::is_function_v<Func>
using add_const_t = add_const<Func>::type;

//-----------------------------------------------------------------------------
template <typename Func> struct remove_const;

template <typename Res, typename... Args>
struct remove_const<auto (Args...) -> Res>
{
    using type = auto (Args...) -> Res;
};

template <typename Res, typename... Args>
struct remove_const<auto (Args..., ...) -> Res>
{
    using type = auto (Args..., ...) -> Res;
};

template <typename Res, typename... Args>
struct remove_const<auto (Args...) const -> Res>
{
    using type = auto (Args...) -> Res;
};

template <typename Res, typename... Args>
struct remove_const<auto (Args..., ...) const -> Res>
{
    using type = auto (Args..., ...) -> Res;
};

template <typename Func> requires std::is_function_v<Func>
using remove_const_t = remove_const<Func>::type;

//-----------------------------------------------------------------------------
//
// NOTE: const, ellipsis types support
//
template <typename Func> struct result;

template <typename Res, typename... Args>
struct result<auto (Args...) -> Res>
{
    using type = Res;
};

template <typename Res, typename... Args>
struct result<auto (Args...) const -> Res>
{
    using type = Res;
};

template <typename Res, typename... Args>
struct result<auto (Args..., ...) -> Res>
{
    using type = Res;
};

template <typename Res, typename... Args>
struct result<auto (Args..., ...) const -> Res>
{
    using type = Res;
};

template <typename Func> requires std::is_function_v<Func>
using result_t = result<Func>::type;

}