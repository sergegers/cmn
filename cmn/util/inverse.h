#pragma once

#include <boost/spirit/include/qi_symbols.hpp>
#include <boost/spirit/include/karma_symbols.hpp>

namespace boost::spirit
{

////////////////////////////////////////////////////////////////////////////////
//
// http://stackoverflow.com/questions/14861476/boost-c-searching-spirit-symbol-table
//
////////////////////////////////////////////////////////////////////////////////

template <typename Char, typename T, typename Lookup, typename Filter>
auto inverse(qi::symbols<Char, T, Lookup, Filter> const& parser)->
    karma::symbols<T, std::basic_string<Char> > 
{
    struct adder_t 
    {
        karma::symbols<T, std::basic_string<Char> > result;

        auto operator()(std::basic_string<Char> const &symbol, T v) const -> void
        { 
            result.add(v, symbol); 
        }

    } adder;

    parser.for_each(adder);
    return adder.result;
}

} 
