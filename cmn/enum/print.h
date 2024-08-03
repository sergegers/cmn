#pragma once

#include <iosfwd>
#include <string>

// boost.io
#include <boost/io/ios_state.hpp>

#include <cmn/meta/concepts.h>

#include "manip.h"
#include "feature.h"

#define CHT_HEX_OUT()   ::std::hex << ::std::showbase << ::std::uppercase

namespace cmn::enum_::io
{

namespace detail
{

template <typename Enum, typename Char, typename CharTraits>
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

    template <typename Record>
    auto operator ()(Record const &rec) const -> void
    {
        auto const delim = basic_bitfield_delim_manip<Char>::value(m_ostr);
        auto const po = print_manip::value(m_ostr);
        if (m_first_time) m_first_time = false; else m_ostr << delim;

        using utils_type = utils<Enum>;

        if constexpr (std::is_scoped_enum_v<Enum>)
        {
            if (has_feature(po, print_t::class_prefix))
                m_ostr
                    << traits<Enum>::template get_name<Char>()
                    << symbols<Char>::scope_resolution
                ;
        }

        m_ostr << rec.template get_str<Char, CharTraits>();
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
struct printer<Enum, kind_t::enum_>: op::fwd<Enum>
{
    using inherited = op::fwd<Enum>;
    using kkind_type = kkind_t<kind_t::enum_>;

    [[no_unique_address]] kkind_type        m_kind;

    printer(Enum val, kkind_type kind): inherited { val }, m_kind{ kind } {}

    template <typename Char, typename CharTraits>
    auto print(std::basic_ostream<Char, CharTraits> &ostr) -> decltype(ostr)
    {
        using utils_type = typename inherited::utils_type;
        using print_record_type = detail::print_record<Enum, Char, CharTraits>;

        // NOTE: don't make it static
        auto const open = basic_open_manip<Char, CharTraits>::value(ostr);
        auto const close = basic_close_manip<Char, CharTraits>::value(ostr);

        ostr << open;

        static auto groups = get_groups(Enum{});
        static_assert
        (
            std::tuple_size_v<decltype(groups)> == 1,
            "Enum must have the one and only one group"
        );

        static decltype(auto) group = std::get<0>(groups);

        // ReSharper disable CppLocalVariableMayBeConst
        bool first_time = true;
        // ReSharper restore CppLocalVariableMayBeConst
        auto const remain = utils_type::process_on_group
        (
            group,  this->to_mask_val(), print_record_type{ ostr, first_time }
        );
        detail::print_tail(remain, ostr);

        return ostr << close;
    }
};

template<c::enum_ Enum>
struct printer<Enum, kind_t::bitfield>: op::fwd<Enum>
{
    using inherited = op::fwd<Enum>;
    using kkind_type = kkind_t<kind_t::bitfield>;

    [[no_unique_address]] kkind_type        m_kind;

    printer(Enum val, kkind_type kind): inherited { val }, m_kind{ kind } {}

    template <typename Char, typename CharTraits>
    auto print(std::basic_ostream<Char, CharTraits> &ostr) -> decltype(ostr)
    {
        using utils_type = typename inherited::utils_type;
        using stream_ref_type = std::basic_ostream<Char, CharTraits> &;
        using print_record_type = detail::print_record<Enum, Char, CharTraits>;

        auto const mask_ = utils_type::to_mask_val(bitfield_mask_manip<Enum>::value(ostr));

        // NOTE: don't make it static
        auto const open = basic_open_manip<Char, CharTraits>::value(ostr);
        auto const close = basic_close_manip<Char, CharTraits>::value(ostr);

        ostr << open;

        static auto groups = get_groups(Enum{});
        bool first_time = true;
        auto const remain = boost::fusion::fold
        (
            groups,
            this->to_mask_val() & mask_,
            [&ostr, &first_time]<typename Group>(auto val, Group const &group)
            {
                static_assert
                (
                    std::tuple_size_v<Group> == 1, 
                    "Bitfield group must contain the one and only one record"
                );
                decltype(auto) rec = group[0];
                
                if (utils_type::to_mask_val(rec.m_val) & val)
                {
                    print_record_type prt { ostr, first_time };
	                prt(rec);
                }

                return val & ~utils_type::to_mask_val(rec.m_val);
            }
        );
        detail::print_tail(remain, ostr);

        return ostr << close;
    }
};

template<c::enum_ Enum>
struct printer<Enum, kind_t::combo>: op::fwd<Enum>
{
    using inherited = op::fwd<Enum>;
    using kkind_type = kkind_t<kind_t::combo>;

    [[no_unique_address]] kkind_type        m_kind;

    printer(Enum val, kkind_type kind): inherited { val }, m_kind{ kind } {}

    template <typename Char, typename CharTraits>
    auto print(std::basic_ostream<Char, CharTraits> &ostr) -> decltype(ostr)
    {
        using utils_type = typename inherited::utils_type;
        using print_record_type = detail::print_record<Enum, Char, CharTraits>;
        using mask_type = typename utils_type::mask_type;

        auto const mask_ = utils_type::to_mask_val(bitfield_mask_manip<Enum>::value(ostr));

        auto const open = basic_open_manip<Char, CharTraits>::value(ostr);
        auto const close = basic_close_manip<Char, CharTraits>::value(ostr);

        ostr << open;

        static auto groups = get_groups(Enum{});
        static auto masks = utils_type::calc_masks(groups);

        // ReSharper disable CppLocalVariableMayBeConst
        bool first_time = true;
        // ReSharper restore CppLocalVariableMayBeConst
        auto const remain = utils_type::process_on_groups
        (
            groups,
            masks,
            this->to_mask_val() & mask_,
            print_record_type{ ostr, first_time },
            mask_
        );

        detail::print_tail(remain, ostr);

        return  ostr << close;
    }
};

}   

