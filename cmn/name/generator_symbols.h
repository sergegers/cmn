#pragma once

#include <boost/spirit/include/karma_symbols.hpp>

namespace cmn::name_
{

enum class keyword_t: char;

namespace gsym
{

using keywords_t = boost::spirit::karma::symbols<keyword_t, std::string>;
auto keywords() -> keywords_t const &;

using class_kwds_t = boost::spirit::karma::symbols<keyword_t, std::string>;
auto class_kwds()->keywords_t const&;

}

}
