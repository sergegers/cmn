#pragma once

#include <istream>  // for std::ws  used in fusion IO, but not included

#include <boost/fusion/sequence/comparison/equal_to.hpp>
#include <boost/fusion/sequence/comparison/not_equal_to.hpp>
#include <boost/fusion/sequence/comparison/less.hpp>
#include <boost/fusion/sequence/comparison/less_equal.hpp>
#include <boost/fusion/sequence/comparison/greater.hpp>
#include <boost/fusion/sequence/comparison/greater_equal.hpp>
#include <boost/fusion/sequence/io/out.hpp>
#include <boost/fusion/sequence/hash.hpp>

#if __has_include(<boost/fusion/concepts.hpp>)
#   include <boost/fusion/concepts.hpp>
#else
#   include <cmn/meta/boost/fusion/concepts.hpp>
#endif

namespace boost::fusion::sequence
{

namespace operators
{

template <c::fus_sequence Lhs, c::fus_sequence Rhs>
constexpr auto operator == (Lhs const &lhs, Rhs const & rhs) -> bool
{
    return fusion::equal_to(lhs, rhs);
}

template <c::fus_sequence Lhs, c::fus_sequence Rhs>
constexpr auto operator != (Lhs const &lhs, Rhs const & rhs) -> bool
{
    return fusion::not_equal_to(lhs, rhs);    
}

template <c::fus_sequence Lhs, c::fus_sequence Rhs>
constexpr auto operator < (Lhs const &lhs, Rhs const & rhs) -> bool
{
    return fusion::less(lhs, rhs);        
}

template <c::fus_sequence Lhs, c::fus_sequence Rhs>
constexpr auto operator <= (Lhs const &lhs, Rhs const & rhs) -> bool
{
    return fusion::less_equal(lhs, rhs); 
}

template <c::fus_sequence Lhs, c::fus_sequence Rhs>
constexpr auto operator > (Lhs const &lhs, Rhs const & rhs) -> bool
{
    return fusion::greater(lhs, rhs);
}

template <c::fus_sequence Lhs, c::fus_sequence Rhs>
constexpr auto operator >= (Lhs const &lhs, Rhs const & rhs) -> bool
{
    return fusion::greater_equal(lhs, rhs);
}

}

namespace hashing
{

template <c::fus_sequence Seq>
auto hash_value(Seq const &seq) -> std::size_t
{
    return fusion::hashing::hash_value(seq);
}

}

}

