#pragma once

#include <ranges>

#include <boost/fusion/algorithm/iteration/fold.hpp>

#if __has_include(<boost/mp11/concepts.hpp>)
#   include <boost/mp11/concepts.hpp>
#else
#   include <cmn/meta/boost/mp11/concepts.hpp>
#endif

#if __has_include(<boost/fusion/concepts.hpp>)
#   include <boost/fusion/concepts.hpp>
#else
#   include <cmn/meta/boost/fusion/concepts.hpp>
#endif

#include <cmn/meta/concepts.h>
#include <cmn/error/exception.h>

#include "manip.h"

namespace cmn::tuple_::io
{

template <typename Char, typename CharTraits, boost::c::fus_sequence Seq>
    requires !std::ranges::range<Seq>   // skip ranges
auto operator << (std::basic_ostream<Char, CharTraits> &ostr, Seq const &seq) -> decltype(ostr)
{
    using ostream_type = std::basic_ostream<Char, CharTraits>;

    // NOTE: don't make it static
    auto const open  = basic_open_manip<Char, CharTraits>::value(ostr);
    auto const separator = basic_separator_manip<Char, CharTraits>::value(ostr);
    auto const close = basic_close_manip<Char, CharTraits>::value(ostr);

    ostr << open;

    return boost::fusion::fold
    (
        seq,
        ostr,
        [&separator, first = true](ostream_type &ostr, auto item) mutable -> decltype(ostr)
        {
            if (!first)
                ostr << separator;
            else
                first = false;

            ostr << item;

           return ostr; 
        }
    ) << close;
}

//-----------------------------------------------------------------------------
template <typename Char, typename CharTraits, boost::c::fus_sequence Seq>
    requires !std::ranges::range<Seq>   // skip ranges
auto operator >> (std::basic_istream<Char, CharTraits> &istr, Seq &seq) -> decltype(istr)
{
    throw not_implemented();
}

}

namespace boost::fusion::sequence::operators
{

using cmn::tuple_::io::operator <<;
using cmn::tuple_::io::operator >>;

}

// inject to STL streams namespace for using ADL
namespace std
{

using cmn::tuple_::io::operator <<;
using cmn::tuple_::io::operator >>;

}

namespace cmn::io
{

using tuple_::io::operator <<;
using tuple_::io::operator >>;

}
