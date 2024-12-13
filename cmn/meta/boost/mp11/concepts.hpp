#pragma once

#include <boost/mp11.hpp>

namespace boost::c
{

template <typename L> concept mp11_list = mp11::mp_is_list<L>::value;
template <typename S> concept mp11_set = mp11::mp_is_set<S>::value;
template <typename M> concept mp11_map = mp11::mp_is_map<M>::value;

}
