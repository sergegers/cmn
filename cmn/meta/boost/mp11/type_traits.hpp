#pragma once

// boost.mp11
#include <boost/mp11.hpp>

#if __has_include(<boost/mp11/concepts.hpp>)
#   include <boost/mp11/concepts.hpp>
#else
#   include <cmn/meta/boost/mp11/concepts.hpp>
#endif

// boost.mp11 shortcuts & extensions
namespace boost::mp11
{

template <c::mp11_list L>
constexpr std::size_t mp_size_v = mp_size<L>::value;

template <c::mp11_list L>
constexpr bool mp_empty_v = mp_empty<L>::value;

template <c::mp11_list L, typename V>
constexpr std::size_t mp_find_v = mp_find<L, V>::value;

template <c::mp11_list L, template <typename...> typename P>
constexpr std::size_t mp_find_if_v = mp_find_if<L, P>::value;

template <c::mp11_list L, typename Q>
constexpr std::size_t mp_find_if_q_v = mp_find_if_q<L, Q>::value;

template <c::mp11_list L, typename V>
constexpr std::size_t mp_count_v = mp_count<L, V>::value;

template <c::mp11_list L, template <typename...> typename P>
constexpr std::size_t mp_count_if_v = mp_count_if<L, P>::value;

template <c::mp11_list L, typename Q>
constexpr std::size_t mp_count_if_q_v = mp_count_if_q<L, Q>::value;

template <c::mp11_list L, typename V>
constexpr bool mp_contains_v = mp_contains<L, V>::value;

template <c::mp11_map M, typename K>
constexpr bool mp_map_contains_v = mp_map_contains<M, K>::value;

///////////////////////////////////////////////////////////////////////////////
//
// convenient to use with boost::mpl::eval_if_c
//
template <template <typename...> typename F, typename... Args>
struct mpl_apply
{
	using type = F<Args...>;	
};

///////////////////////////////////////////////////////////////////////////////
//
// quote nontemplated class
//
template <typename S>
struct mp_quote_class
{
	template <typename T> struct fn: S {};
};

}

