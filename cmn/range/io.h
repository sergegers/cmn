#pragma once

#include <cmn/range/io/format_traits.h>
#include <cmn/range/io/out.h>
#include <cmn/range/io/formatter.h>
#include <cmn/range/io/manip.h>
#include <cmn/range/io/printer.h>

namespace cmn::io
{

using range_::io::operator <<;

}

// inject to STL streams namespace for using ADL
namespace std
{

using cmn::range_::io::operator <<;

}
