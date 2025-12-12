#pragma once

#include <tuple>

#include <boost/mp11.hpp>

#include <cmn/meta/concepts.h>
#include <cmn/meta/traits.h>
#include <cmn/tuple/va/concepts.h>

namespace cmn::va
{

// forward declarations
template <c::function Sig> class tuple_view;

//-----------------------------------------------------------------------------
//
// make signature from tuple of arguments
//
template <c::instance_of<std::tuple> VaArgTpl, typename Res>
struct make_signature;

template <c::eva_arg... EvaArgs, typename Res>
struct make_signature<std::tuple<EvaArgs...>, Res>
{
    using type = auto (EvaArgs...) -> Res;
};

template <c::instance_of<std::tuple> VaArgTpl, typename Res>
using make_signature_t = typename make_signature<VaArgTpl, Res>::type;

//-----------------------------------------------------------------------------
//
// arguments w/o ellipsis
//
template <c::function Sig>
struct remove_ellipsis;

template <c::eva_arg... EvaArgs, typename Res>
struct remove_ellipsis<auto (EvaArgs...) -> Res>
{
    using type = make_signature_t<boost::mp11::mp_remove<std::tuple<EvaArgs...>, ellipsis>, Res>;
};

template <c::function Sig>
using remove_ellipsis_t = typename remove_ellipsis<Sig>::type;

////////////////////////////////////////////////////////////////////////////////
//
// tuple_ellipsis_index_v
//
// returns ellipsis index or sizeof... (EvaArgs) if the ellipsis is absent
//
////////////////////////////////////////////////////////////////////////////////
template <c::eva_arg... EArgs>
constexpr std::size_t ellipsis_index_v = boost::mp11::mp_find<boost::mp11::mp_list<EArgs...>, ellipsis>::value;

///////////////////////////////////////////////////////////////////////////////
template <c::function Sig>
constexpr bool has_ellipsis_v = false;

template <c::eva_arg... EArgs, typename Res>
constexpr bool has_ellipsis_v<auto (EArgs...) -> Res> = (ellipsis_index_v<EArgs...> < (sizeof... (EArgs)));

///////////////////////////////////////////////////////////////////////////////
template <std::size_t Idx_, c::instance_of<tuple_view> TplView>
using tuple_keep_element_t = std::tuple_element_t<Idx_, typename TplView::keep_args_type>;

}