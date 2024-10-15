#pragma once

#include <tuple>
#include <utility>

#include <boost/mp11.hpp>

#include <cmn/meta/concepts.h>
#include <cmn/tuple/va/concepts.h>
#include <cmn/tuple/va/detail/util.h>

namespace cmn::va
{

////////////////////////////////////////////////////////////////////////////////
//
// va_tuple is C-like ellipsis function argument container
//
////////////////////////////////////////////////////////////////////////////////
namespace detail
{

using namespace boost::mp11;

template <c::instance_of<std::tuple> ArgTplOfTpls>
class tuple_impl;

template <typename ... NamedArgs, c::va_arg_... VaArgs>
class tuple_impl<std::tuple<std::tuple<NamedArgs...>, std::tuple<VaArgs...>>>
{
public:
    using args_type = std::tuple<NamedArgs..., VaArgs...>;
    using eargs_type = std::tuple<NamedArgs..., ellipsis, VaArgs...>;
    using named_args_type = std::tuple<NamedArgs...>;
    using keep_named_args_type = named_args_type;
    using keep_va_args_type = std::tuple<keep_type_t<VaArgs>...>;
    using keep_args_type = mp_append<keep_named_args_type, keep_va_args_type>;

    //-----------------------------------------------------------------------------
    //
    // interface for detail::get(), detail::put() functions
    //
    //-----------------------------------------------------------------------------
    static constexpr auto named_args_size = sizeof... (NamedArgs);
    static constexpr auto va_args_size = sizeof... (VaArgs);
    static constexpr auto args_size = named_args_size + va_args_size;

    using va_args_type = std::tuple<VaArgs...>;

    template <std::size_t Idx_> using arg_t = std::tuple_element_t<Idx_, args_type>;

    template <std::size_t Idx_, typename Self>
    using res_t = remove_rvalue_reference_t<decltype(std::forward_like<Self>(std::declval<arg_t<Idx_>>()))>;

    constexpr auto &named_args(this auto &self_) { return self_.m_named_args; }

    template <std::size_t Idx_, typename Self> requires (Idx_ >= named_args_size)
    constexpr decltype(auto) slot(this Self &&self_)
    {
        using keep_type = std::remove_reference_t<decltype(std::forward_like<Self>(std::declval<keep_type_<Idx_>>()))>;
        return reinterpret_cast<keep_type *>(std::forward<Self>(self_).m_va_arg_buffer + va_arg_offset_in_bytes<Idx_>());
    }

    template <std::size_t Idx_, typename Self> requires (Idx_ >= named_args_size) && c::ref_va_arg<arg_t<Idx_>>
    constexpr decltype(auto) big_arg_slot(this Self &&self_)
    {
        return std::get<big_slot_idx_v<tuple_impl, Idx_>>(std::forward<Self>(self_).m_big_arg_buffer);
    }
    //
    //-----------------------------------------------------------------------------
private:
    static_assert(sizeof... (NamedArgs) > 0, "At least one named argument is required");

    using big_arg_buffer_type = big_arg_buffer_t<tuple_impl>;

    keep_named_args_type    m_named_args;
    big_arg_buffer_type     m_big_args_buffer;

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
        return va_arg_offset_in_bytes_v<arg_t<Idss_>...>;
    }

    template <std::size_t Idx_>
    static consteval auto va_arg_offset_in_bytes() -> std::size_t
    {
        return va_arg_offset_in_bytes_impl(std::make_index_sequence<Idx_ - named_args_size>{});
    }

    static consteval auto va_args_size_in_bytes() -> std::size_t
    {
        return va_arg_offset_in_bytes<va_args_size>();
    }

    alignas(4) std::byte m_va_arg_buffer[va_args_size_in_bytes()];

    template <std::size_t... Idss_, typename... Args>
    tuple_impl(std::index_sequence<Idss_...>, Args &&... args)
    {
        (detail::put<Idss_>(*this, std::forward<Args>(args)), ... );
    }

public:
    template <typename... Args> requires (sizeof... (Args) == sizeof... (NamedArgs) + sizeof... (VaArgs))
    tuple_impl(Args &&... args):
        tuple_impl{ std::index_sequence_for<Args...>{}, std::forward<Args>(args)... }
    {}

    static constexpr auto size() { return sizeof... (NamedArgs) + sizeof... (VaArgs); }

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

    constexpr auto as_va_list() const -> std::va_list
    {
        return const_cast<std::va_list>(reinterpret_cast<char const *>(&m_va_arg_buffer));
    }
};

///////////////////////////////////////////////////////////////////////////////
//
// NamedArgs == 0
//
///////////////////////////////////////////////////////////////////////////////
template <c::va_arg_... VaArgs>
class tuple_impl<std::tuple<std::tuple<>, std::tuple<VaArgs...>>>
{
public:
    using args_type = std::tuple<VaArgs...>;
    using eargs_type = std::tuple<ellipsis, VaArgs...>;
    using named_args_type = std::tuple<>;
    using keep_named_args_type = named_args_type;
    using keep_va_args_type = std::tuple<keep_type_t<VaArgs>...>;
    using keep_args_type = mp_append<keep_named_args_type, keep_va_args_type>;

    //-----------------------------------------------------------------------------
    //
    // interface for detail::get(), detail::put() functions
    //
    //-----------------------------------------------------------------------------
    static constexpr auto named_args_size = 0;
    static constexpr auto va_args_size = sizeof... (VaArgs);
    static constexpr auto args_size = named_args_size + va_args_size;

    using va_args_type = std::tuple<VaArgs...>;

    template <std::size_t Idx_> using arg_t = std::tuple_element_t<Idx_, args_type>;

    template <std::size_t Idx_, typename Self>
    using res_t = remove_rvalue_reference_t<decltype(std::forward_like<Self>(std::declval<arg_t<Idx_>>()))>;

    constexpr auto &named_args(this auto &self_) { return self_.m_named_args; }

    template <std::size_t Idx_, typename Self> requires (Idx_ >= named_args_size)
    constexpr decltype(auto) slot(this Self &&self_)
    {
        using keep_type = std::remove_reference_t<decltype(std::forward_like<Self>(std::declval<keep_type_<Idx_>>()))>;
        return reinterpret_cast<keep_type *>(std::forward<Self>(self_).m_va_arg_buffer + va_arg_offset_in_bytes<Idx_>());
    }

    template <std::size_t Idx_, typename Self> requires (Idx_ >= named_args_size) && c::ref_va_arg<arg_t<Idx_>>
    constexpr decltype(auto) big_arg_slot(this Self &&self_)
    {
        return std::get<big_slot_idx_v<tuple_impl, Idx_>>(std::forward<Self>(self_).m_big_args_buffer);
    }
    //
    //-----------------------------------------------------------------------------
private:

    using big_arg_buffer_type = big_arg_buffer_t<tuple_impl>;

    big_arg_buffer_type     m_big_args_buffer;

    template <typename T>
    static constexpr auto idx_v = (mp_count<args_type, T>::value == 1)?
        mp_find<args_type, T>::value:
        mp_size<args_type>::value
    ;

    template <std::size_t Idx_> using keep_type_ = std::tuple_element_t<Idx_, keep_args_type>;

    template <std::size_t... Idss_>
    static consteval auto va_arg_offset_in_bytes_impl(std::index_sequence<Idss_...>) -> std::size_t
    {
        return va_arg_offset_in_bytes_v<arg_t<Idss_>...>;
    }

    template <std::size_t Idx_>
    static consteval auto va_arg_offset_in_bytes() -> std::size_t
    {
        return va_arg_offset_in_bytes_impl(std::make_index_sequence<Idx_ - named_args_size>{});
    }

    static consteval auto va_args_size_in_bytes() -> std::size_t
    {
        return va_arg_offset_in_bytes<va_args_size>();
    }

    alignas(4) std::byte m_va_arg_buffer[va_args_size_in_bytes()];

    template <std::size_t... Idss_, typename... Args>
    tuple_impl(std::index_sequence<Idss_...>, Args &&... args)
    {
        (detail::put<Idss_>(*this, std::forward<Args>(args)), ... );
    }

public:
    template <typename... Args> requires (sizeof... (Args) == sizeof... (VaArgs))
    tuple_impl(Args &&... args):
        tuple_impl{ std::index_sequence_for<Args...>{}, std::forward<Args>(args)... }
    {}

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

    constexpr auto as_va_list() const -> std::va_list
    {
        return const_cast<std::va_list>(reinterpret_cast<char const *>(&m_va_arg_buffer));
    }
};

}

template <c::function Sig>
class tuple;

template <typename... EArgs, typename Res>
class tuple<auto (EArgs...) -> Res> final:
    public detail::tuple_impl<boost::mp11::mp_split<std::tuple<EArgs...>, ellipsis>>
{
private:
    using inherited = detail::tuple_impl<boost::mp11::mp_split<std::tuple<EArgs...>, ellipsis>>;
    friend inherited;
public:
    using inherited::inherited;
};

}   
