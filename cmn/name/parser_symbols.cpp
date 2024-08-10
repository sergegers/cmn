
#include "parser_symbols.h"
#include "types.h"

namespace cmn::name_::psym
{

keywords_t::keywords_t()
{
    using enum keyword_t;
    add
        ("enum", enum_)
        ("class", class_)
        ("struct", struct_)
    ;
}

auto keywords() -> keywords_t const&
{
    static keywords_t s_kwds;
    return s_kwds;
}

//-----------------------------------------------------------------------------
class_kwds_t::class_kwds_t()
{
    using enum keyword_t;
    add
        ("enum", enum_)
        ("class", class_)
        ("struct", struct_)
    ;
}

auto class_kwds() -> class_kwds_t const &
{
    static class_kwds_t s_kwds;
    return s_kwds;
}
}
