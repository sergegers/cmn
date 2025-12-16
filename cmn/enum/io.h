#pragma once

// NOTE: to explicit instantiate input/output operators
// include the current header before direct or indirect
// - <cmn/shared/enum/enum.h>
// - <cmn/shared/enum/bitfield.h>
// - <cmn/shared/enum/combo.h>
// inclusion

#include <cmn/enum/io/printer.h>
#include <cmn/enum/io/reader.h>
#include <cmn/enum/io/manip.h>
#include <cmn/enum/io/formatter.h>

namespace cmn::io
{

using enum_::io::operator <<;
using enum_::io::operator >>;

}
