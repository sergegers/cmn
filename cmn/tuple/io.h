#pragma once

#include <cmn/tuple/io/out.h>
#include <cmn/tuple/io/printer.h>
#include <cmn/tuple/io/manip.h>
#include <cmn/tuple/io/format_traits.h>
#include <cmn/tuple/io/formatter.h>

namespace cmn::io
{

using tuple_::io::operator <<;
using tuple_::io::operator >>;

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
