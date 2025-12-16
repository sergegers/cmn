#pragma once

#include <cmn/meta/concepts.h>
#include <cmn/meta/traits.h>
#include <cmn/util/feature.h>

#include <cmn/enum/detail/record_info.h>

#include "print.h"

namespace cmn::enum_
{

namespace detail
{

// keep out operations here to avoid circular dependencies due to print_t
template <c::adapted_enum E, typename Char, typename CharTraits, typename OutIt>
constexpr auto out(record_info<E> const &rec, c::list_source_options<Char, CharTraits> auto const& options) -> void
{
    using enum io::print_t;

    io::print_t po = options.options;
    if (has_feature(po, ns))
    {
        
    }
}

}

namespace io::detail
{

///////////////////////////////////////////////////////////////////////////////
///
/// redirect to appropriate out_...() function
///
///////////////////////////////////////////////////////////////////////////////

template <c::adapted_enum E, typename Char, typename CharTraits, typename OutIt>
constexpr auto out(int_<kind_t::enum_>, E en, c::list_source_options<Char, CharTraits> const &options, OutIt out_it) -> void
{}

template <c::adapted_enum E, typename Char, typename CharTraits, typename OutIt>
constexpr auto out(int_<kind_t::bitfield>, E en, c::list_source_options<Char, CharTraits> const &options, OutIt out_it) -> void
{}

template <c::adapted_enum E, typename Char, typename CharTraits, typename OutIt>
constexpr auto out(int_<kind_t::combo>, E en, c::list_source_options<Char, CharTraits> const &options, OutIt out_it) -> void
{}


}


}
