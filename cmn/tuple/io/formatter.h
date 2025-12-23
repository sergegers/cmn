#pragma once

#include <format>

#if __has_include(<boost/fusion/concepts.hpp>)
#   include <boost/fusion/concepts.hpp>
#else
#   include <cmn/meta/boost/fusion/concepts.hpp>
#endif

#include <cmn/io/format.h>

#include "format_traits.h"

namespace std
{

template <boost::c::fus_sequence Seq, typename Char>
// TODO:
//requires formattable<std::remove_cvref_t<R>, Char>
struct formatter<cmn::io::list<Seq>, Char>: cmn::io::list_formatter<Seq, Char>
{
private:
    using inherited = cmn::io::list_formatter<Seq, Char>;
public:
    using inherited::parse;
};

}
