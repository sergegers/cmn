#pragma once

#include <cmn/enum/io/format_traits.h>

namespace cmn::enum_::io
{

namespace detail
{

///////////////////////////////////////////////////////////////////////////////
///
/// redirect to appropriate out_...() function
///
template <typename Char, typename CharTraits, c::adapted_enum E, typename OutIt>
constexpr auto out(int_<kind_t::enum_>, E en, vsource_options<Char, CharTraits> const & options,
    OutIt out_it) -> void
{}

template <typename Char, typename CharTraits, c::adapted_enum E, typename OutIt>
constexpr auto out(int_<kind_t::bitfield>, E en, vsource_options<Char, CharTraits> const& options,
    OutIt out_it) -> void
{}

template <typename Char, typename CharTraits, c::adapted_enum E, typename OutIt>
constexpr auto out(int_<kind_t::combo>, E en, vsource_options<Char, CharTraits> const &options,
    OutIt out_it) -> void
{}


}


}
