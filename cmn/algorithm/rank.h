#pragma once

#include <boost/mp11.hpp>
#include <boost/mpl/pair.hpp>

namespace cmn
{

namespace detail
{

using namespace boost::mp11;
namespace mpl = boost::mpl;

template 
<
    template <typename...> typename Pred = mp_less,
    typename... Items
> 
struct rank
{
    using items_type = mp_list<Items...>;
    using indices_type = mp_from_sequence<std::make_index_sequence<sizeof... (Items)>>;
    using idx_map_type = mp_transform<mpl::pair, items_type, indices_type>;

    template <typename Lhs, typename Rhs>
    using pred = Pred<typename Lhs::first, typename Rhs::first>;

    using sorted_idx_map_type = mp_sort<idx_map_type, pred>;

    using type = mp_transform<mp_second, sorted_idx_map_type>;
};

} 

////////////////////////////////////////////////////////////////////////////////
// returns a sequence of indices based on predicate
////////////////////////////////////////////////////////////////////////////////
using detail::rank;

template
<
    template <typename...> typename Pred = boost::mp11::mp_less,
    typename... Items
> 
using rank_t = rank<Pred, Items...>::type;

} 
