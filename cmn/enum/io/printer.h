#pragma once

#include <ostream>
#include <iterator>

#include <cmn/meta/traits.h>

#include "manip.h"
#include "out.h"
#include "format_traits.h"

namespace cmn::enum_
{

namespace detail
{

// keep out operations here to avoid circular dependencies due to print_t
template 
<
      typename Char
    , typename CharTraits
    , c::adapted_enum E
>
auto operator << (std::basic_ostream<Char, CharTraits> &ostr, record_info<E> const &rec) -> decltype(ostr)
{
    using out_iterator_type = std::ostreambuf_iterator<Char, CharTraits>;

    static auto const &fmt_opt = cmn::io::sink_format_traits<E, Char, CharTraits>{};

    bool first { true };
    // TODO: clarify mask type
    io::detail::out_record(rec, fmt_opt.mask, fmt_opt, first, out_iterator_type{ ostr });

    return ostr;
}

}

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
        using delimiter_manip_type = basic_bitfield_delimiter_manip<Char, CharTraits>;
        using bitfield_mask_manip_type = bitfield_mask_manip<E>;
        using out_iterator_type = std::ostreambuf_iterator<Char, CharTraits>;

        basic_sink_format_options const fmt_opt
        {
            .options = print_manip::value(ostr),
            .open = open_manip_type::value(ostr),
            .close = close_manip_type::value(ostr),
            .delimiter = delimiter_manip_type::value(ostr),
            .mask = static_cast<std::uintptr_t>(bitfield_mask_manip_type::value(ostr))
        };

        return out(m_kind, m_val, fmt_opt, out_iterator_type{ ostr }), ostr;
    }
};

}   

}