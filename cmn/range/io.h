#pragma once

#include <iosfwd>
#include <ranges>
#include <algorithm>
// boost.io
#include <boost/io/ios_state.hpp>

#include <cmn/meta/concepts.h>

#include "manip.h"

namespace cmn::range_::io
{

//-----------------------------------------------------------------------------
//
// <
//      item_0
//      item_1
//      ...
//      item_n
// >
//
struct table_out_
{
    template <typename Char, typename CharTraits>
    friend decltype(auto) operator <<(std::basic_ostream<Char, CharTraits> &ostr, table_out_)
    {
        using symbols = symbols<Char, CharTraits>;
        return ostr << 
            basic_ropen<Char, CharTraits>(symbols::open_angle_bracket + symbols::endl) << 
            basic_rclose<Char, CharTraits>(symbols::close_angle_bracket + symbols::endl) <<
            basic_rdelim<Char, CharTraits>(symbols::endl)
        ;
    }
};

inline constexpr table_out_ table_out{};

//-----------------------------------------------------------------------------
//
//  <item_0, item_1, ... item_n>
//
struct compact_table_out_
{
    template <typename Char, typename CharTraits>
    friend decltype(auto) operator << (std::basic_ostream<Char, CharTraits> &ostr, compact_table_out_)
    {
        using symbols = symbols<Char, CharTraits>;
        return ostr << 
            basic_ropen<Char, CharTraits>(symbols::open_angle_bracket) << 
            basic_rclose<Char, CharTraits>(symbols::close_angle_bracket) <<
            basic_rdelim<Char, CharTraits>(symbols::comma + symbols::whitespace)
        ;
    }
};

inline constexpr compact_table_out_ compact_table_out{};

///////////////////////////////////////////////////////////////////////////////
template <typename Char, typename CharTraits, std::ranges::range Range>
auto operator << (std::basic_ostream<Char, CharTraits> &ostr, Range rng) -> std::basic_ostream<Char, CharTraits> &
    requires
        c::printable<std::ranges::range_value_t<Range>, Char, CharTraits>
     && !c::c_array_of<Range, Char>   // skip string arrays
{
    // NOTE: don't make it static
    auto const open  = basic_open_manip<Char, CharTraits>::value(ostr);
    auto const delim = basic_delim_manip<Char, CharTraits>::value(ostr);
    auto const close = basic_close_manip<Char, CharTraits>::value(ostr);

    ostr << open;

    std::ranges::for_each
    (
        std::move(rng),
        [first = true, &ostr, &delim](auto const &elem) mutable 
        {
            if (!first)
                ostr << delim;
            else
                first = false;

            ostr << elem;
        }
    );

    ostr << close;

    return ostr;
}

}   

// inject to STL streams namespace for using ADL
namespace std
{

using cmn::range_::io::operator <<;

}
