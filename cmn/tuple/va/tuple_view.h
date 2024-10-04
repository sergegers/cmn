#pragma once

#include <cstdarg>
#include <tuple>
#include <new>

#include <boost/mp11/algorithm.hpp>

#include <cmn/meta/type_traits.h>

#include <cmn/tuple/va/detail/tuple_get.h>
#include <cmn/tuple/va/detail/tuple_set.h>
#include "concepts.h"
#include "tuple_utils.h"

namespace cmn::va
{

// NOTE: va_tuple can hold ordinary args and va_list args

////////////////////////////////////////////////////////////////////////////////
//
// va_tuple_view is wrapper for C-like ellipsis function arguments
// wrapper is read only
//
////////////////////////////////////////////////////////////////////////////////
template <c::eva_argument... EArgs>
class tuple_view final: public tuple_tag
{
private:
    using itself = tuple_view<EArgs...>;

    static constexpr auto ellipis_index = tuple_ellipsis_index<EArgs...>();

    alignas(4) std::va_list m_va;

    template <std::size_t Idx_>
    static constexpr auto offset() -> std::size_t
    {
        return tuple_offset<Idx_, EArgs...>();
    }
public:
    using args_type = std::tuple<EArgs...>;

    constexpr tuple_view(from_va_list_t, std::va_list va) : m_va { va } {}

    template <std::size_t Idx_ = 0>
    explicit constexpr tuple_view(tuple_element_t<Idx_, EArgs...> &arg):
        m_va{ reinterpret_cast<std::va_list>(std::addressof(arg)) }
    {}

    static constexpr auto size_in_bytes() -> std::size_t
    { return tuple_size_in_bytes<EArgs...>(); }

    static constexpr auto size() -> std::size_t
    { return std::tuple_size_v<tuple_args_t<EArgs...>>; }

    template <std::size_t Idx_>
        requires (Idx_ < size())
    constexpr auto get() const
        -> tuple_element_t<Idx_, EArgs...>
    {
        using elem_type = tuple_element_t<Idx_, EArgs...>;
        using keep_type = tuple_keep_element_t<Idx_, EArgs...>;

        static constexpr bool is_ellipsis_arg = ellipis_index != -1 && Idx_ >= ellipis_index;        

        auto const keep_arg_ptr = std::launder(reinterpret_cast<keep_type *>(m_va + offset<Idx_>()));
        return detail::tuple_get<elem_type>(std::bool_constant<is_ellipsis_arg>{}, *keep_arg_ptr);
    }

    template <typename T>
    constexpr auto get() const
    {
        using namespace boost::mp11;
        using pure_args_type = tuple_args_t<EArgs...>;

        constexpr auto idx = mp_find<pure_args_type, T>::value;
        static_assert(idx != mp_size<pure_args_type>::value, "Type isn't exist");

        return get<idx>();
    }
};

template <>
class tuple_view<ellipsis>: public tuple_tag
{
    using itself = tuple_view<ellipsis>;
public:
    using args_type = std::tuple<ellipsis>;

    static constexpr auto size_in_bytes() -> std::size_t { return 0; }
    static constexpr auto size() -> std::size_t { return 0; }

    constexpr tuple_view(from_va_list_t, std::va_list /*va*/) {}
};

namespace detail
{

// struct arg_type
template <std::size_t Idx_, typename VaSig>
struct arg_type;

template <std::size_t Idx_, typename Res, typename... EArgs>
struct arg_type<Idx_, auto (EArgs...) -> Res>
{
    using type = tuple_element_t<Idx_, EArgs...>;
};

template <std::size_t Idx_, typename VaSig> 
using arg_type_t = typename arg_type<Idx_, VaSig>::type;

// struct make_va_tuple_view_impl
template <c::eva_signature VaSig>
struct make_va_tuple_view_impl;

template <typename Res, c::va_argument... EArgs>
struct make_va_tuple_view_impl<auto (EArgs...) -> Res>
{
private:
    template <c::eva_argument... EEArgs>
    static constexpr auto create_from_eargs(from_va_list_t fva, std::va_list va, std::tuple<EEArgs...> const &)
    {
        return tuple_view<EEArgs...> { fva, va };
    }
public:
    constexpr auto operator ()(from_va_list_t fva, std::va_list va) const
    {
        static_assert(tuple_ellipsis_index<EArgs...>() != -1, "There is no ellipsis");
        // there is for the variadic parameters only, 
        // so let's cut the ordinary parameters head, 
        // but remain the ellipsis
        return 
            create_from_eargs
            (
                fva, va, 
                 boost::mp11::mp_erase_c<std::tuple<EArgs...>, 0, tuple_ellipsis_index<EArgs...>()>{}
            )
        ;
    }

    template <std::size_t Idx_>
    constexpr auto operator () 
    (
          std::integral_constant<std::size_t, Idx_>
        , arg_type_t<Idx_, Res(EArgs...)> &arg
    ) const noexcept
    {
        // Could be used with ordinary (w/o ellipsis) functions
        return tuple_view<EArgs...>{ arg };
    }
};

} 

// variadic arguments only
template <c::eva_signature EVaSig>
constexpr auto make_va_tuple_view(from_va_list_t fva, std::va_list va) noexcept
{
    return detail::make_va_tuple_view_impl<EVaSig>{}(fva, va);
}

template <c::eva_signature EVaSig, std::size_t Idx_>
constexpr auto make_va_tuple_view(detail::arg_type_t<Idx_, EVaSig> &arg) noexcept
{
    return detail::make_va_tuple_view_impl<EVaSig>{}
    (
          std::integral_constant<std::size_t, Idx_>{}
        , arg
    );
}

// last nonvariadic argument and variadics
template <c::eva_signature EVaSig>
constexpr auto make_va_tuple_view(detail::arg_type_t<0, EVaSig> &arg) noexcept
{
    return make_va_tuple_view<EVaSig, 0>(arg);
}

}
