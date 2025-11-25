#pragma once

#include <utility>
#include <ios>
#include <algorithm>
#include <type_traits>

#include <cmn/meta/concepts.h>

#include <cmn/enum/feature.h>

#include <cmn/io/manip/slot/fwd.h>
#include <cmn/io/manip/slot/manip.h>
#include <cmn/io/manip/slot/decoder.h>
#include <cmn/io/manip/slot/forwarder.h>

namespace cmn::io
{

consteval auto get_format_options_info(auto)
{
    return std::pair{ false, 0 };
}

template <typename T>
constexpr auto format_options_info_v = get_format_options_info(T{});

template <typename T>
constexpr bool enable_luxury_io_v = std::get<0>(format_options_info_v<T>);

template <typename T>
using format_options_t = std::remove_const_t<std::tuple_element_t<1, decltype(format_options_info_v<T>)>>;

template <typename T>
constexpr format_options_t<T> default_format_options_v = std::get<1>(format_options_info_v<T>);


///////////////////////////////////////////////////////////////////////////////
namespace manip
{

// set one group at once
template <c::adapted_enum FmtOptions> 
    requires !enum_::nullable_v<FmtOptions>

struct format_options_storage
{
    using keep_type = int_keep_type;
    using tag_type = format_options_storage;

    static auto index(std::ios_base &ios) -> int
    {
        // call xalloc once to get an index at which we can store data for this
        // manipulator.
        static auto const idx = std::ios_base::xalloc();
        return idx;
        
    }

    static auto value(std::ios_base &ios) -> keep_type
    {
        return ios.iword(index(ios));
    }

    static auto value(std::ios_base &ios, keep_type value) -> void
    {
        auto const old_value = static_cast<FmtOptions>(format_options_storage::value(ios));
        auto const new_value = set_feature(old_value, static_cast<FmtOptions>(value));

        ios.iword(index(ios)) = static_cast<int>(new_value);
    }
};

//-----------------------------------------------------------------------------
template <c::adapted_enum FmtOptions> 
    requires !enum_::nullable_v<FmtOptions>

using format_options_manip = slot_manip
<
      FmtOptions
    , FmtOptions{ 0 } // here is empty values, initial value
    , FmtOptions{ 0 } // supplies by strong_typedef_fmt_traits
    , int_decoder<FmtOptions>
    , format_options_storage<FmtOptions>
>;

//-----------------------------------------------------------------------------
template <c::adapted_enum FmtOptions> 
    requires !enum_::nullable_v<FmtOptions>

using format_options_forwarder = slot_manip_forwarder<format_options_manip<FmtOptions>>;

}

///////////////////////////////////////////////////////////////////////////////

template <typename Char, typename CharTraits, typename T>
auto get_format_options(std::basic_ios<Char, CharTraits> &ios, T const &) -> format_options_t<T>
{
    using format_options_type = format_options_t<T>;
    using manip_type = manip::format_options_manip<format_options_type>;

    constexpr format_options_type empty { 0 };

    auto const value = default_format_options_v<T>; // get default type formatting options from traits
    auto const manip_value = manip_type::value(ios);

    // override default value from traits by the stream ones
    return std::ranges::fold_left
    (
        enum_::masks_v<format_options_type>, 
        empty, 
        [manip_value, value](int_fmt_t res, mask_type_t<int_fmt_t> mask)
        {
            auto const manip_masked_value = get_feature(manip_value, mask);
            return manip_masked_value != empty? 
                res | manip_masked_value: 
                res | get_feature(value, mask);
        }
    );
}

}
