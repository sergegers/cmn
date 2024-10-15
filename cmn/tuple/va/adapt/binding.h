#pragma once

#include <cmn/meta/concepts.h>
#include <cmn/tuple/va/tuple_view.h>

namespace cmn::va
{

////////////////////////////////////////////////////////////////////////////////
//
// Structure bindings support
//
///////////////////////////////////////////////////////////////////////////////
template <std::size_t Idx_, c::function Sig>
constexpr decltype(auto) get(tuple_view<Sig> const &t) noexcept
{
    return t.template get<Idx_>();
}

//template <typename T, c::function Sig>
//constexpr decltype(auto) get(tuple_view<Sig> const &t) noexcept
//{
//    return t.template get<T>();
//}

//template <std::size_t Idx_, typename... EArgs>
//constexpr auto get(tuple<EArgs...> const &t) noexcept -> decltype(auto)
//{
//    return t.template get<Idx_>();
//}
//
//template <typename T, typename... EArgs>
//constexpr auto get(tuple<EArgs...> const &t) noexcept -> decltype(auto)
//{
//    return t.template get<T>();
//}

} 
