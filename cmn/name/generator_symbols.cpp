
#include <cmn/util/inverse.h>

#include "generator_symbols.h"
#include "parser_symbols.h"
#include "types.h"

namespace cmn::name_::gsym
{

auto keywords() -> keywords_t const &
{
    namespace karma = boost::spirit::karma;

    static karma::symbols<keyword_t, std::string> const s_kwds =
        boost::spirit::inverse(psym::keywords());

    return s_kwds;
}

auto class_kwds() -> keywords_t const &
{
    namespace karma = boost::spirit::karma;

    static karma::symbols<keyword_t, std::string> const s_kwds =
        boost::spirit::inverse(psym::class_kwds());

    return s_kwds;
}
}
