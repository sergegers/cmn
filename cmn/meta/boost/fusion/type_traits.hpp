#pragma once

#pragma once

#include <boost/core/use_default.hpp>

#include <boost/fusion/iterator/deref.hpp>
#include <boost/fusion/iterator/distance.hpp>
#include <boost/fusion/iterator/advance.hpp>
#include <boost/fusion/iterator/next.hpp>
#include <boost/fusion/iterator/equal_to.hpp>

#include <boost/fusion/sequence/intrinsic/empty.hpp>
#include <boost/fusion/sequence/intrinsic/size.hpp>
#include <boost/fusion/sequence/intrinsic/begin.hpp>
#include <boost/fusion/sequence/intrinsic/end.hpp>

#include <boost/fusion/container/vector/convert.hpp>

#include <boost/fusion/algorithm/transformation/erase.hpp>

#if __has_include(<boost/fusion/concepts.hpp>)
#   include <boost/fusion/concepts.hpp>
#else
#   include <cmn/meta/boost/fusion/concepts.hpp>
#endif

namespace boost::fusion
{

namespace result_of
{

template <typename I> using deref_t = typename deref<I>::type;
template <typename I1, typename I2> constexpr bool equal_to_v = equal_to<I1, I2>::value;
template <typename First, typename Last> constexpr auto distance_v = distance<First, Last>::value;
template <typename Iterator, typename N> constexpr auto advance_v = advance<Iterator, N>::value;

template <typename Iterator, std::ptrdiff_t N_> constexpr auto advance_c_v = advance_c<Iterator, N_>::value;
template <typename Iterator, std::ptrdiff_t N_> using advance_c_t = typename advance_c<Iterator, N_>::type;

template <typename Iterator> using next_t = typename next<Iterator>::type;

template <c::fus_sequence Seq> constexpr bool empty_v = empty<Seq>::value;
template <c::fus_sequence Seq> constexpr std::size_t size_v = size<Seq>::value;
template <c::fus_sequence Seq> using begin_t = typename begin<Seq>::type;
template <c::fus_sequence Seq>using end_t = typename end<Seq>::type;

template <c::fus_sequence Seq, typename First, typename Last = use_default>
using erase_t = typename erase<Seq, First, Last>::type;

template <c::fus_sequence Seq> using as_vector_t = typename as_vector<Seq>::type;

}

}
