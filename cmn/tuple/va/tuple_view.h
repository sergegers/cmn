#pragma once

#include <tuple>
#include <type_traits>
#include <cstdint>
#include <cstddef>
#include <new>
#include <string>
#include <cstdarg>

#include <boost/mp11.hpp>

#include <boost/fusion/sequence/intrinsic/back.hpp>
#include <boost/fusion/adapted/std_tuple.hpp>

#include <cmn/meta/concepts.h>
#include <cmn/meta/traits.h>

#include <cmn/tuple/va/detail/util.h>

#include "concepts.h"

namespace cmn::va
{

namespace detail
{

using namespace boost::mp11;
namespace fus = boost::fusion;

//------------------------------------------------------------------------------
//
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
//-----------------------------------------------------------------------------
template <c::instance_of<std::tuple> ArgTplOfTpls>
class tuple_view_impl;

template <typename ... NamedArgs, c::va_arg_... VaArgs>
class tuple_view_impl<std::tuple<std::tuple<NamedArgs...>, std::tuple<VaArgs...>>>
{
public:
    using args_type = std::tuple<NamedArgs..., VaArgs...>;
    using eargs_type = std::tuple<NamedArgs..., ellipsis, VaArgs...>;
    using named_args_type = std::tuple<NamedArgs...>;
    using keep_named_args_type = std::tuple<std::add_lvalue_reference_t<NamedArgs>...>;
    using keep_va_args_type = std::tuple<keep_type_t<VaArgs>...>;
    using keep_args_type = mp_append<keep_named_args_type, keep_va_args_type>;

    //-----------------------------------------------------------------------------
    //
    // interface for detail::get() functions
    //
    //-----------------------------------------------------------------------------
    static constexpr auto named_args_size = sizeof... (NamedArgs);

    template <std::size_t Idx_> using arg_t = std::tuple_element_t<Idx_, args_type>;

    template <std::size_t Idx_, typename Self>
    using res_t = remove_rvalue_reference_t<decltype(std::forward_like<Self>(std::declval<arg_t<Idx_>>()))>;

    constexpr auto &named_args(this auto &self_) { return self_.m_named_args; }

    template <std::size_t Idx_, typename Self> requires (Idx_ >= named_args_size)
    constexpr decltype(auto) slot(this Self &&self_)
    {
        using keep_type = std::remove_reference_t<decltype(std::forward_like<Self>(std::declval<keep_type_<Idx_>>()))>;
        return reinterpret_cast<keep_type *>(std::forward<Self>(self_).va_arg_buffer() + va_arg_offset_in_bytes<Idx_>());
    }
    //
    //-----------------------------------------------------------------------------
private:
    static_assert(sizeof... (NamedArgs) > 0, "At least one named argument is required");

    keep_named_args_type    m_named_args;

    template <typename T>
    static constexpr auto idx_v = (mp_count<args_type, T>::value == 1)?
        mp_find<args_type, T>::value:
        mp_size<args_type>::value
    ;

    // The behavior of the va_start macro is undefined if the last parameter before the ellipsis has reference type,
    // or has type that is not compatible with the type that results from default argument promotions
    // https://en.cppreference.com/w/cpp/language/variadic_arguments Default conversions
    static_assert(c::va_arg_<mp_back<named_args_type>>);

    template <std::size_t Idx_> using keep_type_ = std::tuple_element_t<Idx_, keep_args_type>;

    template <std::size_t... Idss_>
    static consteval auto va_arg_offset_in_bytes_impl(std::index_sequence<Idss_...>) -> std::size_t
    {
        return (va_slot_size_in_bytes_v<arg_t<Idss_>> + ...);
    }

    template <std::size_t Idx_>
    static consteval auto va_arg_offset_in_bytes() -> std::size_t
    {
        return va_arg_offset_in_bytes_impl(std::make_index_sequence<Idx_ - named_args_size + 1>{});
    }

    template <typename Self>
    constexpr auto *va_arg_buffer(this Self &&self_) // std::byte (const) *
    {
        using buffer_type = std::remove_reference_t<decltype(std::forward_like<Self>(std::byte{}))>;

        auto &last_arg = fus::back(std::forward<Self>(self_).m_named_args);
        return reinterpret_cast<buffer_type *>(&last_arg);
    }
public:
    template <typename... Args> requires (std::convertible_to<Args &&, NamedArgs> && ...)
    tuple_view_impl(Args &&... args): m_named_args{ std::forward<Args>(args)... } {}

    static constexpr auto size() { return sizeof... (NamedArgs) + sizeof... (VaArgs); }

    static constexpr auto va_args_size = sizeof... (VaArgs);
    static constexpr auto args_size = named_args_size + va_args_size;

    // BUG: https://developercommunity.visualstudio.com/t/Structured-Binding-With-Deducing-This-ge/10773966
    // https://developercommunity.visualstudio.com/t/VS-2022-17112:-Internal-Compiler-Error/10736199
    template <std::size_t Idx_> requires (Idx_ < size())
    constexpr auto get() noexcept //-> decltype(detail::get<Idx_>(*this))
    {
        return detail::get<Idx_>(*this);
    }

    template <std::size_t Idx_> requires (Idx_ < size())
    constexpr auto get() const noexcept //-> decltype(detail::get<Idx_>(*this))
    {
        return detail::get<Idx_>(*this);
    }

    //-----------------------------------------------------------------------------
    template <typename T> requires (idx_v<T> < size())
    constexpr auto get() noexcept //-> decltype(detail::get<idx_v<T>>(*this))
    {
        return detail::get<idx_v<T>>(*this);
    }

    template <typename T> requires (idx_v<T> < size())
    constexpr auto get() const noexcept //-> decltype(detail::get<idx_v<T>>(*this))
    {
        return detail::get<idx_v<T>>(*this);
    }
};

///////////////////////////////////////////////////////////////////////////////
//
// NamedArgs == 0
//
///////////////////////////////////////////////////////////////////////////////
template <c::va_arg_... VaArgs>
class tuple_view_impl<std::tuple<std::tuple<>, std::tuple<VaArgs...>>>
{
public:
    using args_type = std::tuple<VaArgs...>;
    using eargs_type = std::tuple<ellipsis, VaArgs...>;
    using named_args_type = std::tuple<>;
    using keep_named_args_type = std::tuple<>;
    using keep_va_args_type = std::tuple<keep_type_t<VaArgs>...>;
    using keep_args_type = mp_append<keep_named_args_type, keep_va_args_type>;

    template <std::size_t Idx_> using arg_type_t = std::tuple_element_t<Idx_, args_type>;

    //-----------------------------------------------------------------------------
    //
    // interface for detail::get() functions
    //
    //-----------------------------------------------------------------------------
    static constexpr auto named_args_size = 0;

    template <std::size_t Idx_> using arg_t = std::tuple_element_t<Idx_, args_type>;

    template <std::size_t Idx_, typename Self>
    using res_t = remove_rvalue_reference_t<decltype(std::forward_like<Self>(std::declval<arg_t<Idx_>>()))>;

    constexpr auto &named_args(this auto &self_) { return self_.m_named_args; }

    template <std::size_t Idx_, typename Self> requires (Idx_ >= named_args_size)
    constexpr decltype(auto) slot(this Self &&self_)
    {
        using keep_type = std::remove_reference_t<decltype(std::forward_like<Self>(std::declval<keep_type_<Idx_>>()))>;
        return reinterpret_cast<keep_type *>(std::forward<Self>(self_).va_arg_buffer() + va_arg_offset_in_bytes<Idx_>());
    }
    //
    //-----------------------------------------------------------------------------
private:
    template <std::size_t Idx_> using keep_type_ = std::tuple_element_t<Idx_, keep_args_type>;

    template <typename T>
    static constexpr auto idx_v = (mp_count<args_type, T>::value == 1)?
        mp_find<args_type, T>::value:
        mp_size<args_type>::value
    ;

    static constexpr auto va_args_size = sizeof... (VaArgs);
    static constexpr auto args_size = va_args_size;

    std::va_list m_va;

    template <std::size_t... Idss_>
    static consteval auto va_arg_offset_in_bytes_impl(std::index_sequence<Idss_...>) -> std::size_t
    {
        return va_arg_offset_in_bytes_v<arg_type_t<Idss_>...>;
    }

    template <std::size_t Idx_>
    static consteval auto va_arg_offset_in_bytes() -> std::size_t
    {
        return va_arg_offset_in_bytes_impl(std::make_index_sequence<Idx_>{});
    }

    template <typename Self>
    constexpr auto *va_arg_buffer(this Self &&self_) // std::byte (const) *
    {
        using buffer_type = std::remove_reference_t<decltype(std::forward_like<Self>(std::byte{}))>;
        return self_.m_va;
    }

    template <std::size_t Idx_, typename Self>
    constexpr decltype(auto) slot(this Self &&self_)
    {
        using keep_type = std::remove_reference_t<decltype(std::forward_like<Self>(std::declval<keep_type_<Idx_>>()))>;
        return reinterpret_cast<keep_type *>(std::forward<Self>(self_).va_arg_buffer() + va_arg_offset_in_bytes<Idx_>());
    }

public:
    tuple_view_impl(std::va_list va): m_va{ va } {}

    static constexpr auto size() { return sizeof... (VaArgs); }

    // BUG: https://developercommunity.visualstudio.com/t/Structured-Binding-With-Deducing-This-ge/10773966
    // https://developercommunity.visualstudio.com/t/VS-2022-17112:-Internal-Compiler-Error/10736199
    template <std::size_t Idx_> requires (Idx_ < size())
    constexpr auto get() noexcept //-> decltype(detail::get<Idx_>(*this))
    {
        return detail::get<Idx_>(*this);
    }

    template <std::size_t Idx_> requires (Idx_ < size())
    constexpr auto get() const noexcept //-> decltype(detail::get<Idx_>(*this))
    {
        return detail::get<Idx_>(*this);
    }

    //-----------------------------------------------------------------------------
    template <typename T> requires (idx_v<T> < size())
    constexpr auto get() noexcept //-> decltype(detail::get<idx_v<T>>(*this))
    {
        return detail::get<idx_v<T>>(*this);
    }

    template <typename T> requires (idx_v<T> < size())
    constexpr auto get() const noexcept //-> decltype(detail::get<idx_v<T>>(*this))
    {
        return detail::get<idx_v<T>>(*this);
    }
};

}

template <c::function Sig>
class tuple_view;

template <typename... EArgs, typename Res>
class tuple_view<auto (EArgs...) -> Res> final:
    public detail::tuple_view_impl<boost::mp11::mp_split<std::tuple<EArgs...>, ellipsis>>
{
private:
    using inherited = detail::tuple_view_impl<boost::mp11::mp_split<std::tuple<EArgs...>, ellipsis>>;
    friend inherited;
public:
    using inherited::inherited;
};

}
