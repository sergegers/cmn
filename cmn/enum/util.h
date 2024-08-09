#pragma once

#include <tuple>
#include <array>
#include <limits>
#include <string_view>
#include <tuple>
#include <bit>
#include <algorithm>
#include <iosfwd>
#include <ranges>

// boost.preprocessor
#include <boost/preprocessor/wstringize.hpp>
// boost.fusion
// ReSharper disable CppUnusedIncludeDirective
#include <boost/fusion/container/vector.hpp>
#include <boost/fusion/adapted/std_array.hpp>
#include <boost/fusion/adapted/std_tuple.hpp>
// ReSharper restore CppUnusedIncludeDirective
#include <boost/fusion/algorithm/iteration/fold.hpp>
#include <boost/fusion/view/zip_view.hpp>

#include <cmn/meta/concepts.h> // kind_t
#include <cmn/meta/util.h>

namespace cmn::enum_
{

////////////////////////////////////////////////////////////////////////////////////////////////
//
// "magic get" utilities
//
////////////////////////////////////////////////////////////////////////////////////////////////
consteval auto get_enum_name(c::enum_ auto) -> std::string_view
{
    using namespace std::string_view_literals;

    std::string_view funcsig = __FUNCSIG__;

    // <enum xxx>
    constexpr auto prefix = "<enum "sv;
    auto const l = funcsig.find_last_of('<') + prefix.size();

    constexpr auto postix= ">"sv;
    auto const r = funcsig.find_last_of(postix);

    return funcsig.substr(l, r - l);
}

consteval auto get_enum_wname(c::enum_ auto) -> std::wstring_view
{
    using namespace std::string_view_literals;

    std::wstring_view funcsig = BOOST_PP_WSTRINGIZE(__FUNCSIG__);

    // <enum xxx>
    constexpr auto prefix = L"<enum "sv;
    auto const l = funcsig.find_last_of('<') + prefix.size();

    constexpr auto postix= L">"sv;
    auto const r = funcsig.find_last_of(postix);

    return funcsig.substr(l, r - l);
}

//-----------------------------------------------------------------------------
template <c::enum_ auto En_>
consteval auto enum_member_name() -> std::string_view
{
    using namespace std::string_view_literals;

    std::string_view funcsig = __FUNCSIG__;

    // auto __cdecl enum_member_name<e0>(void)
    constexpr auto prefix = "<"sv;
    auto const l = funcsig.find_first_of(prefix) + prefix.size();

    constexpr auto postix= ">"sv;
    auto const r = funcsig.find_first_of(postix);

    return funcsig.substr(l, r - l);
}

template <c::enum_ auto En_>
consteval auto enum_member_wname() -> std::wstring_view
{
    using namespace std::string_view_literals;

    std::wstring_view funcsig = BOOST_PP_WSTRINGIZE(__FUNCSIG__);

    // auto __cdecl enum_member_wname<e0>(void)
    constexpr auto prefix = L"<"sv;
    auto const l = funcsig.find_first_of(prefix) + prefix.size();

    constexpr auto postix= L">"sv;
    auto const r = funcsig.find_first_of(postix);

    return funcsig.substr(l, r - l);
}

///////////////////////////////////////////////////////////////////////////////
template <c::enum_ auto En_>
struct record
{
    using enum_type = decltype(En_);

    static constexpr auto enum_value = En_;

    std::string_view    m_str;
    std::wstring_view   m_wstr;

    consteval record():
        m_str { enum_member_name<En_>() },
        m_wstr{ enum_member_wname<En_>() }
    {
    }

    friend auto operator << (std::ostream &ostr, record const &self) -> std::ostream &
    {
        return ostr << self.m_str;
    }

    friend auto operator << (std::wostream &ostr, record const &self) -> std::wostream &
    {
        return ostr << self.m_wstr;
    }
};

template <typename... Records> using group = std::tuple<Records...>;
template <typename... Groups> using enum_info = std::tuple<Groups...>;

////////////////////////////////////////////////////////////////////////////////

template <kind_t Kind_>
using kkind_t = std::integral_constant<kind_t, Kind_>;

////////////////////////////////////////////////////////////////////////////////////////////////
//
// enum: 1 group with n records
// bitfield: n groups with 1 record in each
// combo: n groups with m records in each
//
////////////////////////////////////////////////////////////////////////////////
template <typename... Groups> consteval auto get_kind(enum_info<Groups...> const &) -> kind_t
{
    using enum kind_t;
    using enum_info_type = enum_info<Groups...>;

    return std::tuple_size_v<enum_info_type> == 1?
        enum_:
        ((std::tuple_size_v<Groups> == 1) && ...)? bitfield: combo;
}

template <std::size_t I_> using int_ = std::integral_constant<std::size_t, I_>;

namespace record_
{

template <typename T> using enum_type_t = typename T::enum_type;
template <typename T> using mask_type_t = enum_::mask_type_t<enum_type_t<T>>;

template <c::enum_ auto En_>
consteval auto get_value(record<En_> const &) -> decltype(En_)
{
    return En_;
}

template <c::enum_ auto En_>
consteval auto get_mask(record<En_> const &) -> enum_::mask_type_t<decltype(En_)>
{
    return static_cast<enum_::mask_type_t<decltype(En_)>>(En_);
}

}

namespace group_
{

template <typename T> using enum_type_t = record_::enum_type_t<std::tuple_element_t<0, T>>;
template <typename T> using mask_type_t = mask_type_t<enum_type_t<T>>;

template <typename... Records>
consteval auto calc_mask(group<Records...> const &gr) -> mask_type_t<group<Records...>>
{
    return (record_::get_mask(std::get<Records>(gr)) | ...);
}

template <c::enum_ auto En_, decltype(En_)... Ens_>
consteval auto get_enum_values(group<record<En_>, record<Ens_>...> const &) ->
    std::array<decltype(En_), sizeof... (Ens_) + 1>
{
    return { En_, Ens_... };
}

}

template <typename Group, typename... Groups>
consteval auto calc_masks(enum_info<Group, Groups...> const &enum_info)
    -> std::array<group_::mask_type_t<Group>, sizeof... (Groups) + 1>
{
    return
    {
          group_::calc_mask(std::get<Group>(enum_info))
        , group_::calc_mask(std::get<Groups>(enum_info))...
    };
}

///////////////////////////////////////////////////////////////////////////////
//
// get smallest enum value
//
template <typename Group, typename... Groups>
consteval auto min_value(enum_info<Group, Groups...> const &enum_info) -> group_::enum_type_t<Group>
{
    return static_cast<group_::enum_type_t<Group>>(0);
//    return std::min({ std::ranges::begin(std::get<Group>(enum_info))->m_val ... });
}

///////////////////////////////////////////////////////////////////////////////
//
// get largest enum value
//
template <typename Group, typename... Groups>
consteval auto max_value(enum_info<Group, Groups...> const &enum_info) -> group_::enum_type_t<Group>
{
    return static_cast<group_::enum_type_t<Group>>(0);
    //return []<std::size_t... Indices_>
    //    (auto const &groups,std::index_sequence<Indices_...>)
    //{
    //    return std::max({ std::ranges::rbegin(std::get<Indices_>(groups))->m_val ... });
    //}
    //(groups, std::make_index_sequence<sizeof... (Groups)>{});
}

///////////////////////////////////////////////////////////////////////////////

template <c::enum_ Enum>
class utils
{
public:
    using mask_type = mask_type_t<Enum>;

    struct record_type
    {
        Enum m_val;
        std::string_view m_str;
        std::wstring_view m_wstr;

        template <typename Char, typename CharTraits = std::char_traits<Char>>
        auto get_str() const -> std::basic_string_view<Char, CharTraits>
        {
            if constexpr (std::is_same_v<Char, char>) return m_str; else return m_wstr;
        }

        template <typename Char, typename CharTraits>
        friend auto operator << 
        (
              std::basic_ostream<Char, CharTraits> &ostr
            , record_type const &rec
        )
            -> decltype(ostr)
        {
            return ostr << rec.get_str<Char, CharTraits>();
        }
    };

    template <std::size_t Size_>
    using group_t = std::array<record_type, Size_>;

    template <std::size_t... Sizes_>
    using groups_t = std::tuple<group_t<Sizes_>...>;

    static constexpr auto to_mask(Enum const &e) { return static_cast<mask_type>(e); }
    static constexpr auto to_mask(Enum &e) -> auto& {return *std::bit_cast<mask_type *>(&e); }

    static constexpr auto to_mask_val(Enum e) { return static_cast<mask_type>(e); }
private:
    template <std::size_t... Indices_> requires (sizeof... (Indices_) > 0)
    static constexpr auto calc_mask(group_t <sizeof... (Indices_)> const &group, 
        std::index_sequence<Indices_...>) -> mask_type
    {
        return (... | to_mask_val(group.at(Indices_).m_val));
    }

public:
    template <std::size_t... Sizes_>
    static constexpr auto calc_masks(groups_t<Sizes_...> const &groups)
        -> std::array<mask_type, sizeof... (Sizes_)>
    {
        return []<typename... Groups, std::size_t... GrIndices_>
        (
              std::tuple<Groups...> const &groups
            , std::index_sequence<GrIndices_...>
        )->
            std::array<mask_type, sizeof... (Groups)>
        {
            return 
            {
                calc_mask
                (
                     std::get<GrIndices_>(groups),
                     std::make_index_sequence<std::tuple_size_v<Groups>>{}
                )...
            };
        }
        (
             groups,
             std::make_index_sequence <sizeof... (Sizes_)>{}
        );
    }
private:
    template <std::size_t Idx_, std::size_t Size_>
    static constexpr auto united_mask(std::array<mask_type, Size_> const &masks)
    {
        return []<std::size_t... LeftIndices_, std::size_t... RightIndices_>
        (
              std::array<mask_type, sizeof... (LeftIndices_) + sizeof... (RightIndices_) + 1> const &masks
            , std::index_sequence<LeftIndices_...> 
            , std::index_sequence<RightIndices_...> 
        )
        {
            return (0ul | ... | masks.at(LeftIndices_)) | (0ul | ... | masks.at(RightIndices_));
        }
        (
              masks
            , std::make_index_sequence<Idx_>{}
            , make_index_sequence<Idx_ + 1, Size_>{}
        );
    }
public:
    template <std::size_t Size_> requires (Size_ > 0)
    static constexpr auto mask_overlap(std::array<mask_type, Size_> const &masks) -> mask_type
    {
        return []<std::size_t... Indices_>
        (
              std::array<mask_type, sizeof... (Indices_)> const &masks
            , std::index_sequence<Indices_...> 
        )
        {
            auto const umask = (... | masks.at(Indices_));
            return (... | (united_mask<Indices_>(masks) & masks.at(Indices_)));
        }
        (
              masks
            , std::make_index_sequence<Size_>{}
        );
    }     

    //-----------------------------------------------------------------------------
    // execute operation Op if enum value chunk belongs to group and return unprocessed
    // enum value reminder
    //-----------------------------------------------------------------------------
    template <std::size_t Size_, typename Op>
    static constexpr auto process_on_group(group_t<Size_> const &group, mask_type masked_en, Op const &op) -> mask_type
    {
        auto const it = std::ranges::lower_bound
        (
              group
            , masked_en
            , std::less{}
            , [](record_type const &rec) { return to_mask_val(rec.m_val); }
        );
        if (std::ranges::end(group) == it) return masked_en;

        auto const mask_val = to_mask_val(it->m_val);
        return mask_val > masked_en?
            masked_en :
            (op(*it), masked_en & ~mask_val)
        ;
    }

    static constexpr auto def_mask = 
        std::numeric_limits<std::make_unsigned_t<Enum>>::max();

    template <typename Op, typename... Groups>
    static auto process_on_groups
    (
        std::tuple<Groups...> const &groups, 
        std::array<mask_type, sizeof... (Groups)> const &masks,
        mask_type en, 
        Op const &op,
        mask_type mask_2 = def_mask
        
    ) -> mask_type
    {
        using boost::fusion::vector;
        using boost::fusion::zip_view;

        using sequences_type = vector<decltype(groups) &, decltype(masks) &>;
        return boost::fusion::fold
        (
            zip_view<sequences_type>{ sequences_type{ groups, masks } },
            en & mask_2,
            [&op, mask_2](mask_type val, auto const &group_mask)
            {
                using boost::fusion::at_c;

                auto const mask = at_c<1>(group_mask) & mask_2;
                return mask?
                    static_cast<mask_type>
                    (
                        val & ~mask | 
                        process_on_group(at_c<0>(group_mask), mask & val, op)
                    ): 
                    val;
            }
        );
    }

    ///////////////////////////////////////////////////////////////////////////
    //
    // get enum constant index in group, -1 - index is not found
    //
    template <std::size_t Size_>
    static constexpr auto get_index(group_t<Size_> const &group, Enum en) -> std::ptrdiff_t
    {
        return []<std::size_t... Indices_>
        (group_t<Size_> const& group, Enum en, std::index_sequence<Indices_...>)
        {
            return (-1 + ... + (group.at(Indices_).m_val == en? Indices_ : -1)) + Size_;
        }
        (group, en, std::make_index_sequence<Size_>{});
    }

    ///////////////////////////////////////////////////////////////////////////////
    //
    // inplace sort groups by enum value
    //
    template <typename... Groups>
    static constexpr auto sort_groups(std::tuple<Groups...> &groups) -> void
    {
        []<std::size_t... Indices_>
            (auto &groups,std::index_sequence<Indices_...>)
        {
            (0, ..., std::ranges::sort(std::get<Indices_>(groups), std::less{}, &record_type::m_val));
        }
        (groups, std::make_index_sequence<sizeof... (Groups)>{});
    }
};

template <typename Char, typename CharTraits, typename Enum>
auto operator << 
(
      std::basic_ostream<Char, CharTraits> &ostr
    , typename utils<Enum>::record_type const &rec
)
    -> decltype(ostr)
{
    using utils_type = utils<Enum>;
    return ostr << utils_type::str::template get<Char>(rec);
}

}   
