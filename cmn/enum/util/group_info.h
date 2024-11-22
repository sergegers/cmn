#pragma once

#include <cstddef>
#include <array>
#include <ranges>
#include <utility>

#include <cmn/enum/feature.h>

#include "record_info.h"
#include "concepts.h"

namespace cmn::enum_
{

// sorted by value record infos
template <util::c::record_info RecordInfo, std::size_t N_> using group_info = std::array<RecordInfo, N_>;

namespace group_
{

template <util::c::group_info T> using record_enum_type_t = record_::enum_type_t<typename T::value_type>;
template <util::c::group_info T> using mask_type_t = interop_type_t<record_enum_type_t<T>>;

template <c::enum_ auto En_, decltype(En_) ... Ens_> consteval util::c::group_info auto make() ->
    group_info<record_info<decltype(En_)>, (sizeof...(Ens_) + 1)>
{
    using enum_type = decltype(En_);
    using record_info_type = record_info<enum_type>;
    using result_type = group_info<record_info_type, (sizeof...(Ens_) + 1)>;

    result_type res { record_info_type{ int_<En_>{} }, record_info_type{ int_<Ens_>{} }... };
    std::ranges::sort(res, {}, &record_info_type::as_mask);

    return res;
}

template <util::c::record_info Record, std::size_t N_>
consteval auto calc_mask(group_info<Record, N_> const &group) -> mask_type_t<group_info<Record, N_>>
{
    using result_type = mask_type_t<group_info<Record, N_>>;
    return std::ranges::fold_left
    (
        group, 
        0, 
        [](result_type mask, Record const &rec) -> result_type
        { return mask | rec.as_mask(); }
    );
}

template <util::c::record_info Record, std::size_t N_>
consteval auto get_min_value(group_info<Record, N_> const &gr) -> record_::enum_type_t<Record>
{
    return gr[0].m_value;
}

template <util::c::record_info Record, std::size_t N_>
consteval auto get_max_value(group_info<Record, N_> const &gr) -> record_::enum_type_t<Record>
{
    return gr[N_ - 1].m_value;
}

template <util::c::record_info Record, std::size_t N_>
constexpr auto get_values(group_info<Record, N_> const &group) -> std::array<record_::enum_type_t<Record>, N_>
{
    using result_type = std::array<record_::enum_type_t<Record>, N_>;

    return
        []<std::size_t... Idss_>(group_info<Record, N_> const &group, std::index_sequence<Idss_...>) constexpr
            -> result_type
        {
            return { group[Idss_].m_value... };
        }
        (group, std::make_index_sequence<N_>{})
    ;
}

///////////////////////////////////////////////////////////////////////////////
//
// execute operation Op if enum value chunk belongs to group and return unprocessed
// enum value remainder
//
///////////////////////////////////////////////////////////////////////////////
template <typename Op, util::c::record_info Record, std::size_t N_>
constexpr auto exec
(
    group_info<Record, N_> const &group, 
    record_::mask_type_t<Record> group_mask, 
    record_::mask_type_t<Record> en, 
    Op const &op
)
    -> record_::mask_type_t<Record> // return remainder
{
    auto reminder = en;
    auto const mval = en & group_mask;

    std::ignore = std::ranges::find_if
    (
        group, 
        [&reminder, mval, &op](Record const &rec) constexpr -> bool
        {
            bool found = rec.as_mask() == mval;
            if (found)
            {
                op(rec);
                reminder &= ~mval;
            }
            return found;
        }
    );

    return reminder;
}

}

}
