#pragma once

#include <tuple>
#include <array>
#include <limits>
#include <string_view>
#include <tuple>
#include <bit>
#include <algorithm>
#include <iosfwd>
#include <utility>

#include <cmn/meta/concepts.h> // kind_t, op_t, print_t
#include <cmn/meta/util.h>

#include <cmn/name/types.h>

#include <cmn/enum/util/name.h>
#include <cmn/enum/util/group.h>
#include <cmn/enum/util/groups.h>
#include <cmn/enum/util/enum.h>

namespace cmn::enum_
{

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
    return sizeof... (Groups) == 1?
        enum_:
        ((std::tuple_size_v<Groups> == 1) && ...)? bitfield: combo;
}

template <std::size_t I_> using int_ = std::integral_constant<std::size_t, I_>;

template <typename Group, typename... Groups>
consteval auto calc_masks(enum_info<Group, Groups...> const &enum_info)
    -> std::array<group_::mask_type_t<Group>, sizeof... (Groups) + 1>
{
    using group_::calc_mask;

    return
    {
          calc_mask(std::get<Group>(enum_info.m_groups))
        , calc_mask(std::get<Groups>(enum_info.m_groups))...
    };
}

///////////////////////////////////////////////////////////////////////////////
consteval auto default_ops(kind_t kind) -> unsigned int
{
    using enum kind_t;
    return 
        kind == enum_? op_comparable | op_steppable | op_io:
        kind == bitfield? op_bitwise | op_io:
        kind == combo? op_comparable | op_steppable | op_bitwise | op_io:
            op_empty
    ;
}

///////////////////////////////////////////////////////////////////////////////
//
// get smallest enum value
//
template <typename Group, typename... Groups>
consteval auto min_value(enum_info<Group, Groups...> const &enum_info) -> group_::enum_type_t<Group>
{
    using group_::get_min_enum_value;

    using enum_type = group_::enum_type_t<Group>;
    using mask_type = group_::mask_type_t<Group>;
    return static_cast<enum_type>
    (
        std::min
        (
            {
                static_cast<mask_type>(get_min_enum_value(Group{})), 
                static_cast<mask_type>(get_min_enum_value(Groups{}))...
            }
        )
    );
}

///////////////////////////////////////////////////////////////////////////////
//
// get largest enum value
//
template <typename Group, typename... Groups>
consteval auto max_value(enum_info<Group, Groups...> const &enum_info) -> group_::enum_type_t<Group>
{
    using group_::get_max_enum_value;
    using enum_type = group_::enum_type_t<Group>;
    using mask_type = group_::mask_type_t<Group>;

    return static_cast<enum_type>
    (
        std::max
        (
            {
                static_cast<mask_type>(get_max_enum_value(Group{})), 
                static_cast<mask_type>(get_max_enum_value(Groups{}))...
            }
        )
    );
}

// TODO: remove
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
    // enum value remainder
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
