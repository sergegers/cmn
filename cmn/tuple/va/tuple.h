#pragma once

#include <cstdarg>
#include <tuple>
#include <new>

// boost.mp11
#include <boost/mp11.hpp>

#include <cmn/meta/type_traits.h>

#include <cmn/tuple/va/detail/tuple_get.h>
#include <cmn/tuple/va/detail/tuple_set.h>

#include "concepts.h"
#include "tuple_utils.h"

//------------------------------------------------------------------------------
// NOTE: about ellipsis functions:
//
//Parameters of functions that correspond to ... are promoted before passing 
//to your variadic function. char and short are promoted to int, 
//float is promoted to double, etc.
//6.5.2.2.7 The ellipsis notation in a function prototype declarator causes 
//argument type conversion to stop after the last declared parameter.
//The default argument promotions are performed on trailing arguments.
// https://en.cppreference.com/w/cpp/language/variadic_arguments
// http://stackoverflow.com/questions/11270588/variadic-function-va-arg-doesnt-work-with-float
//

namespace cmn::va
{

////////////////////////////////////////////////////////////////////////////////
//
// va_tuple is C-like ellipsis function argument container
//
////////////////////////////////////////////////////////////////////////////////
template <c::eva_argument... EArgs>
class tuple final: public tuple_tag
{
private:
    using itself = tuple<EArgs...>;
    using storage_type = std::aligned_storage_t<tuple_size_in_bytes<EArgs...>(), 4>;

    static constexpr auto ellipis_index = tuple_ellipsis_index<EArgs...>();

    storage_type m_buffer;

    template <std::size_t Idx_>
    static constexpr auto offset() noexcept -> std::size_t
    {
        return tuple_offset<Idx_, EArgs...>();
    }

    template <std::size_t Idx_, typename Arg>
    constexpr auto set(Arg arg) noexcept -> void
    { 
        using elem_type = tuple_element_t<Idx_, EArgs...>;
        using keep_type = tuple_keep_element_t<Idx_, EArgs...>;
        static constexpr bool is_ellipsis_arg = ellipis_index != -1 && Idx_ >= ellipis_index;        

        new
            (reinterpret_cast<char *>(&m_buffer) + offset<Idx_>())
            keep_type
            {
                detail::tuple_set
                (
                      std::bool_constant<is_ellipsis_arg>{}
                    , static_cast<elem_type>(arg)
                )
            }
        ;
    }

    //template <std::size_t Idx_>
    //static constexpr void reset(storage_type &buffer)
    //{
    //    using arg_type = std::tuple_element_t<Idx_, detail::va_tuple_::va_tuple_args_t<EArgs...>>;
    //    reinterpret_cast<arg_type *>(reinterpret_cast<char *>(&buffer) + offset<Idx_>())->~arg_type();
    //}

    template <typename ArgTuple, std::size_t... Indices_>
    explicit constexpr tuple(ArgTuple args, std::index_sequence<Indices_...>) noexcept
    {
        (set<Indices_>(std::get<Indices_>(args)), ...);
    }
public:
    using args_type = std::tuple<EArgs...>;

    template <typename... Args>
    // ReSharper disable CppNonExplicitConvertingConstructor
    constexpr tuple(Args &&... args) noexcept: 
    // ReSharper restore CppNonExplicitConvertingConstructor
        itself 
        { 
              std::tuple{ args... }
            , std::index_sequence_for<Args...>{}
        }
    {}

    static constexpr auto size_in_bytes() noexcept -> std::size_t
    { return tuple_size_in_bytes<EArgs...>(); }

    static constexpr auto size() noexcept -> std::size_t
    { return std::tuple_size_v<tuple_args_t<EArgs...>>; }

    template <std::size_t Idx_>
        requires (Idx_ < size())
    constexpr auto get() const noexcept -> tuple_element_t<Idx_, EArgs...>
    {
        using elem_type = tuple_element_t<Idx_, EArgs...>;
        using keep_type = tuple_keep_element_t<Idx_, EArgs...>;
        static constexpr bool is_ellipsis_arg = ellipis_index != -1 && Idx_ >= ellipis_index;

        auto const keep_arg_ptr = std::launder
        (
              reinterpret_cast<keep_type const *>(reinterpret_cast<char const *>(&m_buffer) 
            + offset<Idx_>())
        );
        return detail::tuple_get<elem_type>(std::bool_constant<is_ellipsis_arg>{}, *keep_arg_ptr);
    }

    template <typename T>
    constexpr auto get() const noexcept
    {
        using pure_args_type = tuple_args_t<EArgs...>;
        constexpr auto idx = boost::mp11::mp_find<pure_args_type, T>::value;
        static_assert(idx != boost::mp11::mp_size<pure_args_type>::value, "Type isn't exist");

        return get<idx>();
    }

    constexpr auto as_va_list() const noexcept
    { return const_cast<std::va_list>(reinterpret_cast<char const *>(&m_buffer)); }
};

// zero argument tuple specialization
template <>
class tuple<ellipsis>: public tuple_tag
{
private:
    using itself = tuple<ellipsis>;
public:
    using args_type = std::tuple<ellipsis>;

    static constexpr auto size() -> std::size_t { return 0; }
    constexpr auto as_va_list() const noexcept -> std::va_list { return nullptr; }
};


namespace detail
{
	
template <typename VaSig> struct make_va_tuple_impl;

template <typename Res, typename... EArgs>
struct make_va_tuple_impl<auto (EArgs...) -> Res>
{
    template <typename... Args>
    constexpr auto operator ()(Args &&... args) const noexcept -> tuple<EArgs...>
    {
        return { std::forward<Args>(args)... };
    }
};

template <typename VaSig> struct to_va_tuple_impl;

template <typename Res, typename... EArgs>
struct to_va_tuple_impl<auto (EArgs...) -> Res>
{
    template <typename... Args, std::size_t... Indices_>
    constexpr auto operator ()(std::tuple<Args...> const &tpl , std::index_sequence<Indices_...>) const noexcept
        -> tuple<EArgs...>
    {
        return { std::get<Indices_>(tpl)... };
    }
};

}

// NOTE: couldn't use perfect forwarding for arguments to decay argument types
template <c::eva_signature VaSig, c::va_argument... Args>
constexpr auto make_va_tuple(Args... args) noexcept
{
    return detail::make_va_tuple_impl<VaSig>{}(args...);
}

template <c::eva_signature VaSig, c::va_argument... Args>
constexpr auto to_va_tuple(std::tuple<Args...> const &tpl) noexcept
{
    return detail::to_va_tuple_impl<VaSig>{}(tpl, std::make_index_sequence<sizeof... (Args)>{});
}

}   
