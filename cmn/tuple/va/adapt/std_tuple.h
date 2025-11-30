#pragma once

#include <cstddef>
#include <tuple>
#include <type_traits>

#include <cmn/meta/concepts.h>
#include <cmn/tuple/va/tuple_view.h>

////////////////////////////////////////////////////////////////////////////////
//
// There is no appropriate extension point, so we are intrude into std
// namespace. So, there is no guarantee that all std tuple functions & metafunctions works.
// Sorry about that.
// NOTE: use boost::fustion::invoke() instead of std::apply()
//
///////////////////////////////////////////////////////////////////////////////
namespace std
{

////////////////////////////////////////////////////////////////////////////////
//
// adapt va_tuple_view
//
////////////////////////////////////////////////////////////////////////////////
template <cmn::c::function Sig>
struct tuple_size<cmn::va::tuple_view<Sig>> :
    integral_constant<size_t, cmn::va::tuple_view<Sig>::size()>
{};

template <size_t Idx_, cmn::c::function Sig>
struct tuple_element < Idx_, cmn::va::tuple_view<Sig>>
{
    using tuple_view_type = cmn::va::tuple_view<Sig>;
    using type = std::tuple_element_t<Idx_, typename tuple_view_type::args_type>;
};

//-----------------------------------------------------------------------------
template <size_t Idx_, cmn::c::function Sig>
constexpr decltype(auto) get(cmn::va::tuple_view<Sig> &t) noexcept
{
    return t.template get<Idx_>();
}

template <size_t Idx_, cmn::c::function Sig>
constexpr decltype(auto) get(cmn::va::tuple_view<Sig> const &t) noexcept
{
    return t.template get<Idx_>();
}

template <size_t Idx_, cmn::c::function Sig>
constexpr decltype(auto) get(cmn::va::tuple_view<Sig> &&t) noexcept
{
    return move(t).template get<Idx_>();
}

//-----------------------------------------------------------------------------
template <typename T, cmn::c::function Sig>
constexpr decltype(auto) get(cmn::va::tuple_view<Sig> &t) noexcept
{
    return t.template get<T>();
}

template <typename T, cmn::c::function Sig>
constexpr decltype(auto) get(cmn::va::tuple_view<Sig> const &t) noexcept
{
    return t.template get<T>();
}

template <typename T, cmn::c::function Sig>
constexpr decltype(auto) get(cmn::va::tuple_view<Sig> &&t) noexcept
{
    return std::move(t).template get<T>();
}

// tuple_cut support
//template <typename... EArgs>
//struct _View_as_tuple<cmn::va::tuple_view<EArgs...>>
//{
//    using type = cmn::va::tuple_args_t<EArgs...>;
//};

////////////////////////////////////////////////////////////////////////////////
//
// adapt va_tuple
//
////////////////////////////////////////////////////////////////////////////////
template <cmn::c::function Sig>
struct tuple_size<cmn::va::tuple<Sig>> :
    integral_constant<size_t, cmn::va::tuple<Sig>::size()>
{};

template <std::size_t Idx_, cmn::c::function Sig>
struct tuple_element < Idx_, cmn::va::tuple<Sig>>
{
    using tuple_type = cmn::va::tuple<Sig>;
    using type = std::tuple_element_t<Idx_, typename tuple_type::args_type>;
};

//-----------------------------------------------------------------------------
template <std::size_t Idx_, cmn::c::function Sig>
constexpr decltype(auto) get(cmn::va::tuple<Sig> &t) noexcept
{
    return t.template get<Idx_>();
}

template <std::size_t Idx_, cmn::c::function Sig>
constexpr decltype(auto) get(cmn::va::tuple<Sig> const &t) noexcept
{
    return t.template get<Idx_>();
}

template <std::size_t Idx_, cmn::c::function Sig>
constexpr decltype(auto) get(cmn::va::tuple<Sig> &&t) noexcept
{
    return std::move(t).template get<Idx_>();
}

//-----------------------------------------------------------------------------
template <typename T, cmn::c::function Sig>
constexpr decltype(auto) get(cmn::va::tuple<Sig> &t) noexcept
{
    return t.template get<T>();
}

template <typename T, cmn::c::function Sig>
constexpr decltype(auto) get(cmn::va::tuple<Sig> const &t) noexcept
{
    return t.template get<T>();
}

template <typename T, cmn::c::function Sig>
constexpr decltype(auto) get(cmn::va::tuple<Sig> &&t) noexcept
{
    return std::move(t).template get<T>();
}

// tuple_cut support
//template <typename... EArgs>
//struct _View_as_tuple<cmn::va::tuple<EArgs...>>
//{
//    using type = cmn::va::tuple_args_t<EArgs...>;
//};

}   
