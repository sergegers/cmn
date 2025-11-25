
#include <cmn/enum/io.h>

#include "int_fmt.h"

namespace cmn::enum_::op
{

template auto operator << (std::ostream &, cmn::io::int_fmt_t) -> std::ostream &;
template auto operator << (std::wostream &, cmn::io::int_fmt_t) -> std::wostream &;

}
