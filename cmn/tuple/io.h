#pragma once

#include <boost/fusion/sequence/io/out.hpp>
#include <boost/fusion/sequence/io/in.hpp>

#if __has_include(<boost/mp11/concepts.hpp>) && __has_include(<boost/fusion/concepts.hpp>)
#   include <boost/mp11/concepts.hpp>
#   include <boost/fusion/concepts.hpp>
#else
#   include <cmn/meta/concepts.h>
#endif

namespace boost::fusion::sequence
{

namespace operators
{

template <typename Char, typename CharTraits, c::fus_sequence Seq>
auto operator << (std::basic_ostream<Char, CharTraits> &ostr, Seq const &seq) -> decltype(ostr)
{
    return fusion::out(ostr, seq);
}

template <typename Char, typename CharTraits, c::fus_sequence Seq>
auto operator >> (std::basic_istream<Char, CharTraits> &istr, Seq &seq) -> decltype(istr)
{
    return fusion::in(istr, seq);
}

}

}

// inject to STL streams namespace for using ADL
namespace std
{

using boost::fusion::sequence::operators::operator <<;
using boost::fusion::sequence::operators::operator >>;

}
