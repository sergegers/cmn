#pragma once

#include <format>
#include <string>
#include <ios>
#include <concepts>

#include <boost/io/ios_state.hpp>

// ReSharper disable CppUnusedIncludeDirective
#include <boost/fusion/adapted/std_tuple.hpp>
// ReSharper restore CppUnusedIncludeDirective
#if __has_include(<boost/fusion/type_traits.hpp>)
#   include <boost/fusion/type_traits.hpp>
#else
#   include <cmn/meta/boost/fusion/type_traits.hpp>
#endif

#include <cmn/fwd.h>
#include <cmn/meta/type_traits.h>

#include <cmn/util/feature.h>
#include <cmn/util/util.h>

#include <cmn/enum/util.h>
#include <cmn/enum/detail/record_info.h>

#include "fmt_specs.h"

namespace cmn::enum_
{

namespace detail
{

// keep print operations here to avoid circular dependencies
template <typename Char, typename CharTraits, c::enum_ En>
auto operator << (std::basic_ostream<Char, CharTraits> &ostr, record_info<En> const &rec) -> decltype(ostr)
{
    using enum io::print_t;
    auto const po = io::print_manip::value(ostr);

    auto const &name = rec.name(ostr);
    static auto const scope_resolution = symbols<Char, CharTraits>::scope_resolution;

    if (has_feature(po, ns) )ostr << name.m_ns << scope_resolution;
    if (has_feature(po, class_prefix)) ostr << name.m_enum_name << scope_resolution;
    return ostr << name.m_enum_member_name;
}

}


namespace io
{

namespace detail
{

template
<
      typename Char
    , typename CharTraits
>
struct out_record_op
{
    basic_fmt_specs<Char, CharTraits> const &m_fmt_specs;
    std::basic_ostream<Char, CharTraits>    &m_ostr;
    bool                                    &m_first;

    template <c::adapted_enum E>
    constexpr auto operator ()(record_info<E> const &rec, mask_type_t<E>) const
    {
        if (!m_first) m_ostr << m_fmt_specs.separator; else m_first = false;
        m_ostr << rec;
    }
};

template <typename Char, typename CharTraits>
auto out_tail(basic_fmt_specs<Char, CharTraits> const &fmt_specs, std::basic_ostream<Char, CharTraits> &ostr, 
    bool first, std::integral auto remain) -> void
{
    if (has_feature(fmt_specs.po, print_t::tail) && !empty(remain))
    {
        boost::io:: basic_ios_all_saver CMN_ANONYMOUS_VARIABLE(){ ostr };

        if (!first) ostr << fmt_specs.separator;
        ostr << std::hex << std::showbase << std::uppercase << remain;
    }
}

template
<
      typename Char
    , typename CharTraits
    , c::adapted_enum E
>
constexpr auto out_enum(E en, basic_fmt_specs<Char, CharTraits> const &fmt_specs, 
    std::basic_ostream<Char, CharTraits> &ostr) -> decltype(ostr)
{
    namespace fus = boost::fusion;
    namespace rfus = fus::result_of;

    using mask_type = mask_type_t<E>;

    static auto groups = groups_v<E>;
    static_assert
    (
        rfus::size_v<decltype(groups)> == 1,
        "Enum must have the one and only one group"
    );

    static decltype(auto) group = std::get<0>(groups);

    ostr << fmt_specs.open;

    bool first = true;
    auto const remain = group_::find_if(group, en, out_record_op{ fmt_specs, ostr, first },
        static_cast<mask_type>(fmt_specs.mask));

    out_tail(fmt_specs, ostr, first, interop_cast(remain));

    return ostr << fmt_specs.close;
}

template
<
      typename Char
    , typename CharTraits
    , c::adapted_enum E
>
constexpr auto out_bitfield(E en, basic_fmt_specs<Char, CharTraits> const &fmt_specs,
    std::basic_ostream<Char, CharTraits> &ostr) -> decltype(ostr)
{
    using mask_type = mask_type_t<E>;

    auto const &groups = groups_v<E>;
    bool first = true;

    ostr << fmt_specs.open;

    auto const remain = fold(groups, en, out_record_op{ fmt_specs, ostr, first }, 
        static_cast<mask_type>(fmt_specs.mask));

    out_tail(fmt_specs, ostr, first, interop_cast(remain));

    return ostr << fmt_specs.close;
}

}

//-----------------------------------------------------------------------------
template
<
      typename Char
    , typename CharTraits
    , c::adapted_enum E
>
constexpr auto out(int_<kind_t::enum_>, E en, basic_fmt_specs<Char, CharTraits> const &fmt_specs, 
    std::basic_ostream<Char, CharTraits> &ostr) -> decltype(ostr)
{
    return detail::out_enum(en, fmt_specs, ostr);
}

template
<
      typename Char
    , typename CharTraits
    , c::adapted_enum E
>
constexpr auto out(int_<kind_t::bitfield>, E en, basic_fmt_specs<Char, CharTraits> const &fmt_specs,
    std::basic_ostream<Char, CharTraits> &ostr) -> decltype(ostr)
{
    return detail::out_bitfield(en, fmt_specs, ostr);
}

template
<
      typename Char
    , typename CharTraits
    , c::adapted_enum E
>
constexpr auto out(int_<kind_t::combo>, E en, basic_fmt_specs<Char, CharTraits> const &fmt_specs, 
    std::basic_ostream<Char, CharTraits> &ostr) -> decltype(ostr)
{
    return detail::out_bitfield(en, fmt_specs, ostr);
}

}

}