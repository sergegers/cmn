#pragma once

#include <tuple>
#include <type_traits>

// boost.mp11
#include <boost/mp11.hpp>

#include <cmn/enum/feature.h>
#include <cmn/algorithm/find.h>

#include "record_info.h"
#include "concepts.h"

namespace cmn::enum_
{

// sorted by value record infos
template <util::c::record_info... RecordInfos> requires (sizeof... (RecordInfos) > 0)
using group_info = std::tuple<RecordInfos...>;

namespace group_
{

template <util::c::group_info T> using record_enum_type_t = record_::enum_type_t<std::tuple_element_t<0, T>>;
template <util::c::group_info T> using mask_type_t = interop_type_t<record_enum_type_t<T>>;

template <typename Lhs, typename Rhs> using pred_t = std::bool_constant<(Lhs::enum_value < Rhs::enum_value)>;
template <c::enum_ auto ... Ens_> using make_t = boost::mp11::mp_sort<group_info<record_info<Ens_>...>, pred_t>;

template <c::enum_ auto ... Ens_> consteval util::c::group_info auto make() { return make_t<Ens_...>{}; }

template <util::c::record_info... Records>
consteval auto calc_mask(group_info<Records...> const &gr) -> mask_type_t<group_info<Records...>>
{
    return (record_::get_value_as_mask(std::get<Records>(gr)) | ...);
}

template <c::enum_ auto En_, decltype(En_)... Ens_>
consteval auto get_enum_values(group_info<record_info<En_>, record_info<Ens_>...> const &) ->
    std::array<decltype(En_), sizeof... (Ens_) + 1>
{
    return { En_, Ens_... };
}

template <c::enum_ auto En_, decltype(En_)... Ens_>
consteval auto get_min_enum_value(group_info<record_info<En_>, record_info<Ens_>...> const &) -> decltype(En_) { return En_; }

template <util::c::record_info Record, util::c::record_info... Records>
consteval auto get_max_enum_value(group_info<Record, Records...> const &) -> record_::enum_type_t<Record>
{
    using namespace boost::mp11;
    return record_::get_value(mp_back<group_info<Record, Records...>>{});
}

///////////////////////////////////////////////////////////////////////////////
//
// execute operation Op if enum value chunk belongs to group and return unprocessed
// enum value remainder
//
///////////////////////////////////////////////////////////////////////////////
template <typename Op, util::c::record_info Record, util::c::record_info... Records>
constexpr auto find
(
    group_info<Record, Records...> const &group, 
    record_::mask_type_t<Record> group_mask, 
    record_::mask_type_t<Record> en, 
    Op const &op
)
    -> record_::mask_type_t<Record> // return remainder
{
    auto reminder = en;
    auto const mval = en & group_mask;

    find_if_fus
    (
        group,
        [mval]<c::enum_ auto En_>(record_info<En_> const &rec) { return rec.value_as_mask == mval; },
        [&op, &reminder, mval]<c::enum_ auto En_>(record_info<En_> const &rec) 
        {
            op(rec);
            reminder &= ~mval;
        }
    );
    return reminder;
}

}

}
