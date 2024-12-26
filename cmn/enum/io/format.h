#pragma once

#include <iterator>
#include <algorithm>
#include <format>

#include <cmn/fwd.h>
#include <cmn/meta/type_traits.h>

#include <cmn/util/feature.h>

#include <cmn/enum/traits.h>
#include <cmn/enum/detail/record_info.h>
#include <cmn/enum/detail/macro.h>

#include "fmt_specs.h"

namespace cmn::enum_
{

namespace detail
{

// keep print operations here to avoid circular dependencies
template <typename Char, typename CharTraits, c::enum_ En>
constexpr auto operator << (std::basic_ostream<Char, CharTraits> &ostr, record_info<En> const &rec) -> decltype(ostr)
{
    using enum io::print_t;

    auto const &name = rec.name(ostr);
    static auto const scope_resolution = symbols<Char, CharTraits>::scope_resolution;

    auto const po = io::print_manip::value(ostr);
    if (has_feature(po, ns)) ostr << name.m_ns << scope_resolution;
    if (has_feature(po, class_prefix)) ostr << name.m_enum_name << scope_resolution;
    return ostr << name.m_enum_member_name;
}

}


namespace io
{

namespace detail
{

template <typename Char, typename CharTraits = std::char_traits<Char>>
constexpr Char const *_1_arg_fmt = nullptr;

template <> constexpr char const *_1_arg_fmt<char> = "{}";
template <> constexpr wchar_t const *_1_arg_fmt<wchar_t> = L"{}";

//-----------------------------------------------------------------------------
template <typename Char, typename CharTraits = std::char_traits<Char>>
constexpr Char const *_2_arg_fmt = nullptr;

template <> constexpr char const *_2_arg_fmt<char> = "{}{}";
template <> constexpr wchar_t const *_2_arg_fmt<wchar_t> = L"{}{}";

//-----------------------------------------------------------------------------
template
<
      typename Char
    , typename CharTraits
    , std::output_iterator<Char> OutIt
>
class print_record_2
{
public:
    using iterator_type = OutIt;
    using fmt_specs_type = basic_fmt_specs<Char, CharTraits>;

    iterator_type           &m_out;
    fmt_specs_type const    &m_fmt_specs;
    bool                    &m_first_time;

    template <c::adapted_enum E>
    constexpr auto operator ()(record_info<E> const &rec) const -> void
    {
        if (m_first_time) 
        {
            m_first_time = false;
            //std::format_to(out,  m_out << rec;
        }
        else
        {
            m_out << m_fmt_specs.separator << rec;
        }

    }
};

template
<
      typename Char
    , typename CharTraits
    , std::output_iterator<Char> OutIt
>
constexpr auto print_tail_2(auto tail_, basic_fmt_specs<Char, CharTraits> const &fmt_specs, bool &first_time, OutIt &out) -> void
{
    if (has_feature(fmt_specs.po, print_t::tail) && !empty(tail_))
    {
        if (first_time)
        {
            out = std::format_to(out, _2_arg_fmt<Char, CharTraits>, fmt_specs.separator, tail_);
            first_time = false;
        }
        else
        {
            out = std::format_to(out, _1_arg_fmt<Char, CharTraits>, tail_);
        }
    }
}

} 

template
<
      typename Char
    , typename CharTraits
    , c::adapted_enum E
    , typename OutIt
>
constexpr auto format(int_<kind_t::enum_>, E en, basic_fmt_specs<Char, CharTraits> const &fmt_specs, OutIt out) -> void
{
    using print_record_type = detail::print_record_2<Char, CharTraits, OutIt>;

//    std::format_to() ranges::copy(fmt_specs.open, out);

    static auto groups = groups_v<E>;
    static_assert
    (
        std::tuple_size_v<decltype(groups)> == 1,
        "Enum must have the one and only one group"
    );

    static decltype(auto) group = std::get<0>(groups);

    bool first_time = true;
    auto const remain = group.exec(en, print_record_type{ out, first_time });
    detail::print_tail_2(remain, fmt_specs, first_time, out);

    //return ostr << close;
}

template
<
      typename Char
    , typename CharTraits
    , c::adapted_enum E
>
constexpr auto format(int_<kind_t::bitfield>, E en, basic_fmt_specs<Char, CharTraits> const &fmt_specs, 
    std::ostream_iterator<Char, Char, CharTraits> out) -> void
{
        //using print_record_type = detail::print_record_2<Char, CharTraits, E>;

        //ostr << fmt_specs.open;

        //static auto groups = groups_v<Enum>;
        //bool first_time = true;
        //auto const remain = boost::fusion::fold
        //(
        //    groups,
        //    to_interop(m_val) & mask_,
        //    [&ostr, &first_time]<typename Group>(auto val, Group const &group)
        //    {
        //        static_assert
        //        (
        //            group_::size_v<Group> == 1, 
        //            "Bitfield group must contain the one and only one record"
        //        );
        //        decltype(auto) rec = group.m_records.front();
        //        
        //        if (rec.as_interop() & val)
        //        {
        //            print_record_type prt { ostr, first_time };
	       //         prt(rec);
        //        }

        //        return val & ~rec.as_interop();
        //    }
        //);
        //detail::print_tail(remain, ostr);

        //return ostr << close;
}

template
<
      typename Char
    , typename CharTraits
    , c::adapted_enum E
>
constexpr auto format(int_<kind_t::combo>, E en, basic_fmt_specs<Char, CharTraits> const &fmt_specs, 
    std::ostream_iterator<Char, Char, CharTraits> out) -> void
{
    //using print_record_type = detail::print_record<Char, CharTraits>;

    //auto const mask_ = to_mask(bitfield_mask_manip<Enum>::value(ostr));

    //auto const open = basic_open_manip<Char, CharTraits>::value(ostr);
    //auto const close = basic_close_manip<Char, CharTraits>::value(ostr);

    //ostr << open;

    //static auto enum_info = enum_info_v<Enum>;
    //static auto masks = masks_v<Enum>;

    //// ReSharper disable CppLocalVariableMayBeConst
    //bool first_time = true;
    //// ReSharper restore CppLocalVariableMayBeConst
    //auto const remain = enum_info.exec(m_val, print_record_type{ ostr, first_time }, mask_);

    //detail::print_tail(remain, ostr);

    //return  ostr << close;
}

}

}