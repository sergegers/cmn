#pragma once

#include <boost/fusion/support/is_sequence.hpp>
#include <boost/fusion/support/category_of.hpp>

namespace boost::c
{
template <typename S>
concept fus_sequence = fusion::traits::is_sequence<S>::value;

template <typename S>
concept random_access_fus_sequence =
       fus_sequence<S> && fusion::traits::is_random_access<S>::value;

}
