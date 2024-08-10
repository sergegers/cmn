#pragma once

#include <iosfwd>
#include <string>

// boost.io
#include <boost/io/ios_state.hpp>

#include <cmn/meta/concepts.h>
#include <cmn/enum/traits.h>
#include <cmn/tuple/io.h>

#include "manip.h"
#include "feature.h"
#include "util.h"

#define CHT_HEX_OUT()   ::std::hex << ::std::showbase << ::std::uppercase

namespace cmn::enum_
{

// keep print operations here to avoid circular dependencies
template <typename Char, typename CharTraits, c::enum_ auto En_>
auto operator << (std::basic_ostream<Char, CharTraits> &ostr, record_info<En_> const &rec) -> decltype(ostr)
{
    using enum io::print_t;

    auto const &name = rec.template get_name<Char, CharTraits>();
    static auto const scope_resolution = symbols<Char, CharTraits>::scope_resolution;

    auto const po = io::print_manip::value(ostr);
    if (has_feature(po, ns)) ostr << name.m_ns << scope_resolution;
    if (has_feature(po, class_prefix)) ostr << name.m_enum_name << scope_resolution;
    return ostr << name.m_enum_member_name;
}

using boost::fusion::sequence::operators::operator <<;

namespace io
{

namespace detail
{

template <typename Char, typename CharTraits>
class print_record
{
private:
    std::basic_ostream<Char, CharTraits>    &m_ostr;
    bool                                    &m_first_time;

public:
    print_record
    (
        std::basic_ostream<Char, CharTraits>& ostr,
        bool &first_time
    ) noexcept
        : m_ostr{ ostr }, m_first_time{ first_time } {}

    template <c::enum_ auto En_>
    auto operator ()(record_info<En_> const &rec) const -> void
    {
        using enum print_t;

        auto const delim = basic_bitfield_delim_manip<Char>::value(m_ostr);
        if (m_first_time) m_first_time = false; else m_ostr << delim;

        m_ostr << rec;
    }
};

template <typename Tail, typename Char, typename CharTraits>
auto print_tail(Tail tail_, std::basic_ostream<Char, CharTraits> &ostr) -> void
{
    auto const po = print_manip::value(ostr);

    if (has_feature(po, print_t::tail) && tail_)
    {
        boost::io::ios_flags_saver const ifs{ ostr };
        auto const delim = basic_bitfield_delim_manip<Char, CharTraits>::value(ostr);

        ostr << CHT_HEX_OUT() << delim << tail_;
    }
}

} 

///////////////////////////////////////////////////////////////////////////////
template<c::enum_ Enum>
struct printer<Enum, kind_t::enum_>
{
    using kkind_type = kkind_t<kind_t::enum_>;

    Enum                                    m_val;
    [[no_unique_address]] kkind_type        m_kind;

    printer(Enum val, kkind_type kind): m_val{ val }, m_kind{ kind } {}

    template <typename Char, typename CharTraits>
    auto print(std::basic_ostream<Char, CharTraits> &ostr) -> decltype(ostr)
    {
        using print_record_type = detail::print_record<Char, CharTraits>;

        // NOTE: don't make it static
        auto const open = basic_open_manip<Char, CharTraits>::value(ostr);
        auto const close = basic_close_manip<Char, CharTraits>::value(ostr);

        ostr << open;

        static auto groups = groups_v<Enum>;
        static_assert
        (
            std::tuple_size_v<decltype(groups)> == 1,
            "Enum must have the one and only one group"
        );

        static decltype(auto) group = std::get<0>(groups);

        // ReSharper disable CppLocalVariableMayBeConst
        bool first_time = true;
        // ReSharper restore CppLocalVariableMayBeConst
        auto const remain = group_::find
        (
            group,
            to_mask(m_val),
            to_mask(m_val),
            print_record_type{ ostr, first_time }
        );
        detail::print_tail(remain, ostr);

        return ostr << close;
    }
};

template<c::enum_ Enum>
struct printer<Enum, kind_t::bitfield>
{
    using kkind_type = kkind_t<kind_t::bitfield>;

    Enum                                    m_val;
    [[no_unique_address]] kkind_type        m_kind;

    printer(Enum val, kkind_type kind): m_val{ val }, m_kind{ kind } {}

    template <typename Char, typename CharTraits>
    auto print(std::basic_ostream<Char, CharTraits> &ostr) -> decltype(ostr)
    {
        using print_record_type = detail::print_record<Char, CharTraits>;

        auto const mask_ = to_mask(bitfield_mask_manip<Enum>::value(ostr));

        // NOTE: don't make it static
        auto const open = basic_open_manip<Char, CharTraits>::value(ostr);
        auto const close = basic_close_manip<Char, CharTraits>::value(ostr);

        ostr << open;

        static auto groups = groups_v<Enum>;
        bool first_time = true;
        auto const remain = boost::fusion::fold
        (
            groups,
            to_mask(m_val) & mask_,
            [&ostr, &first_time]<typename Group>(auto val, Group const &group)
            {
                static_assert
                (
                    std::tuple_size_v<Group> == 1, 
                    "Bitfield group must contain the one and only one record"
                );
                decltype(auto) rec = boost::fusion::front(group);
                
                if (rec.value_as_mask & val)
                {
                    print_record_type prt { ostr, first_time };
	                prt(rec);
                }

                return val & ~record_::get_value_as_mask(rec);
            }
        );
        detail::print_tail(remain, ostr);

        return ostr << close;
    }
};

template<c::enum_ Enum>
struct printer<Enum, kind_t::combo>
{
    using kkind_type = kkind_t<kind_t::combo>;

    Enum                                    m_val;
    [[no_unique_address]] kkind_type        m_kind;

    printer(Enum val, kkind_type kind): m_val{ val }, m_kind{ kind } {}

    template <typename Char, typename CharTraits>
    auto print(std::basic_ostream<Char, CharTraits> &ostr) -> decltype(ostr)
    {
        using print_record_type = detail::print_record<Char, CharTraits>;

        auto const mask_ = to_mask(bitfield_mask_manip<Enum>::value(ostr));

        auto const open = basic_open_manip<Char, CharTraits>::value(ostr);
        auto const close = basic_close_manip<Char, CharTraits>::value(ostr);

        ostr << open;

        static auto groups = groups_v<Enum>;
        static auto masks = masks_v<Enum>;

        // ReSharper disable CppLocalVariableMayBeConst
        bool first_time = true;
        // ReSharper restore CppLocalVariableMayBeConst
        auto const remain = groups_::fold
        (
            groups,
            masks,
            to_mask(m_val) & mask_,
            print_record_type{ ostr, first_time },
            mask_
        );

        detail::print_tail(remain, ostr);

        return  ostr << close;
    }
};

}   

}