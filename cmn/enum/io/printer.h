#pragma once

#include <iosfwd>
#include <string>
#include <iterator>

#include <cmn/meta/concepts.h>
#include <cmn/meta/type_traits.h>   // int_<>

#include <cmn/enum/traits.h>
#include <cmn/enum/detail/macro.h>
#include <cmn/enum/detail/record_info.h>

#include <cmn/tuple/io.h>
#include <cmn/util/feature.h>

#include "manip.h"
#include "fmt_specs.h"
#include "out.h"

namespace cmn::enum_
{

namespace io
{

template<c::adapted_enum E, kind_t Kind_>
struct printer<E, Kind_>
{
    using kkind_type = int_<Kind_>;

    E                                   m_val;
    [[no_unique_address]] kkind_type    m_kind;

    constexpr printer(E val, kkind_type kind): m_val{ val }, m_kind{ kind } {}

    template <typename Char, typename CharTraits>
    constexpr auto print(std::basic_ostream<Char, CharTraits> &ostr) -> decltype(ostr)
    {
        using open_manip_type = basic_open_manip<Char, CharTraits>;
        using close_manip_type = basic_close_manip<Char, CharTraits>;
        using separator_manip_type = basic_bitfield_separator_manip<Char, CharTraits>;
        using bitfield_mask_manip_type = bitfield_mask_manip<E>;
        using fmt_specs_type = basic_fmt_specs<Char, CharTraits>;

        fmt_specs_type const fmt_specs
        {
            .open = open_manip_type::value(ostr),
            .separator = separator_manip_type::value(ostr),
            .close = close_manip_type::value(ostr),
            .po = print_manip::value(ostr),
            .mask = static_cast<long>(bitfield_mask_manip_type::value(ostr))
        };

        return io::out(m_kind, m_val, fmt_specs, ostr);
    }
};

}   

}