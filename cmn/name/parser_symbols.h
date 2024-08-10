#pragma once

#include <boost/spirit/include/qi_symbols.hpp>

namespace cmn::name_
{

enum class keyword_t: char;

namespace psym
{

struct keywords_t: boost::spirit::qi::symbols<char, keyword_t>
{    
private:
    keywords_t();
    friend auto keywords() -> keywords_t const &;
};

auto keywords() -> keywords_t const &;

//-----------------------------------------------------------------------------
struct class_kwds_t : boost::spirit::qi::symbols<char, keyword_t>
{
private:
    class_kwds_t();
    friend auto class_kwds()->class_kwds_t const&;
};

auto class_kwds()->class_kwds_t const&;

}

}
