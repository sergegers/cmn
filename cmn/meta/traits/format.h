#pragma once


#include <cmn/meta/concepts/format.h>

namespace cmn::io
{

template <c::list_sink_format_options FmtOpt> using char_type_t = FmtOpt::char_type;
template <c::list_sink_format_options FmtOpt> using char_traits_type_t = FmtOpt::char_traits_type;

}
