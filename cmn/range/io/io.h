#pragma once
#pragma once

#include <iosfwd>
#include <ranges>
#include <algorithm>

#include <cmn/meta/concepts.h>
#include <cmn/util/symbols.h>

#include <cmn/range/io/manip.h>

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
    friend decltype(auto) operator << (std::basic_ostream<Char, CharTraits> &ostr, table_out_)
    {
        using namespace sym;

        return ostr << 
            basic_range_open<Char, CharTraits>((open_angle_bracket + endl).value<Char, CharTraits>() << 
            basic_range_close<Char, CharTraits>((endl + close_angle_bracket).value<Char, CharTraits>()) <<
            basic_range_separator<Char, CharTraits>(endl.value<Char, CharTraits>()))
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
        using namespace sym;

        return ostr <<
            basic_range_open<Char, CharTraits>(open_angle_bracket.value<Char, CharTraits>()) << 
            basic_range_close<Char, CharTraits>(close_angle_bracket.value<Char, CharTraits>()) <<
            basic_range_separator<Char, CharTraits>((comma + ws).value<Char, CharTraits>())
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
    auto const separator = basic_separator_manip<Char, CharTraits>::value(ostr);
    auto const close = basic_close_manip<Char, CharTraits>::value(ostr);

    ostr << open;

    std::ranges::for_each
    (
        std::move(rng),
        [first = true, &ostr, &separator](auto const &elem) mutable 
        {
            if (!first)
                ostr << separator;
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
