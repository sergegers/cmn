#pragma once

#include <boost/fusion/sequence/io/out.hpp>
#include <boost/fusion/sequence/io/in.hpp>

#include <cmn/meta/concepts.h>

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

