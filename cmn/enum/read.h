#pragma once

#include <iosfwd>
#include <string>
#include <type_traits>
#include <cstddef>
#include <ranges>

#include <boost/fusion/algorithm/iteration/fold.hpp>
#include <boost/spirit/include/qi_symbols.hpp>
// ReSharper disable once CppUnusedIncludeDirective
#include <boost/optional/optional_fwd.hpp>
#include <boost/lexical_cast.hpp>

#include <cmn/meta/concepts.h>

#include "feature.h"
#include "traits.h"
#include "manip.h"
#include "util.h"

namespace cmn::enum_::io
{

namespace detail
{

template <typename Char, typename CharTraits>
auto check_stream_state(std::basic_ios<Char, CharTraits> const &istr) noexcept -> bool;

// parse kind_t::enum
template <typename Char, typename CharTraits>
auto parse_enum
(
      std::basic_istream<Char, CharTraits>& istr
    , boost::spirit::qi::symbols<Char, std::ptrdiff_t> const& item
    , basic_qualified_name<Char, CharTraits> enum_name
    , print_t po
)
    ->std::ptrdiff_t;

// parse kind_t::bitfield, kind_t::combo
template <typename Char, typename CharTraits>
auto parse_combo
(
      std::basic_istream<Char, CharTraits>& istr
    , boost::spirit::qi::symbols<Char, std::ptrdiff_t> const& item
    , basic_qualified_name<Char, CharTraits> enum_name
    , print_t po
)
    ->std::ptrdiff_t;

//-----------------------------------------------------------------------------
// try parse kind_t::enum
template <typename Char, typename CharTraits>
[[nodiscard]] auto try_parse_enum
(
    std::basic_istream<Char, CharTraits>& istr
    , boost::spirit::qi::symbols<Char, std::ptrdiff_t> const& item
    , basic_qualified_name<Char, CharTraits> enum_name
    , print_t po
) noexcept
 -> boost::optional<std::ptrdiff_t>;

// try parse kind_t::bitfield, kind_t::combo
template <typename Char, typename CharTraits>
auto try_parse_combo
(
    std::basic_istream<Char, CharTraits>& istr
    , boost::spirit::qi::symbols<Char, std::ptrdiff_t> const& item
    , basic_qualified_name<Char, CharTraits> enum_name
    , print_t po
) noexcept
 -> boost::optional<std::ptrdiff_t>;

//-----------------------------------------------------------------------------
template <typename Enum, typename Char, typename CharTraits>
[[nodiscard]] auto try_read_
(
    std::basic_istream<Char, CharTraits> &istr, 
    Enum,
    kkind_t<kind_t::enum_>
) noexcept
    -> boost::optional<Enum>
{
    using enum_item_type = boost::spirit::qi::symbols<Char, std::ptrdiff_t>;

    static_assert(std::tuple_size_v<decltype(traits<Enum>::groups)> == 1);

    if (!detail::check_stream_state(istr)) return {};

    auto const po = print_manip::value(istr);

    static auto const item = std::ranges::fold_left
    (
        std::get<0>(traits<Enum>::groups)
      , enum_item_type{}
      , []<typename Item>(Item &&item, auto const &rec) 
        {
            auto const lit = rec.template get_str<Char, CharTraits>();
            item.add(lit, static_cast<std::ptrdiff_t>(rec.m_value));
            return std::forward<Item>(item);
        }
    );

    return try_parse_enum
    (
          istr
        , item
        , traits<Enum>::template get_name<Char>()
        , std::is_scoped_enum_v<Enum> && has_feature(po, print_t::class_prefix)
    ).map([](std::ptrdiff_t res) { return static_cast<Enum>(res); });
}

template <typename Enum, typename Char, typename CharTraits>
[[nodiscard]] auto try_read_
(
    std::basic_istream<Char, CharTraits> &istr, 
    Enum,
    kkind_t<kind_t::bitfield>
) noexcept
    -> boost::optional<Enum>
{
    using enum_item_type = boost::spirit::qi::symbols<Char, std::ptrdiff_t>;

    if (!detail::check_stream_state(istr)) return {};

    auto const po = print_manip::value(istr);

    static auto const item = boost::fusion::fold
    (
        traits<Enum>::groups,
        enum_item_type{},
        []<typename Item, typename Group>(Item && item, Group const& group)
        {
            static_assert(std::tuple_size_v<Group> == 1);

            auto const& rec = group[0];
            auto const lit = rec.template get_str<Char, CharTraits>();
            item.add(lit, static_cast<std::ptrdiff_t>(rec.m_value));

            return std::forward<Item>(item);
        }
    );

    return try_parse_combo
    (
          istr
        , item
        , traits<Enum>::template get_name<Char>()
        , std::is_scoped_enum_v<Enum> && has_feature(po, print_t::class_prefix)
    ).map([](std::ptrdiff_t res) { return static_cast<Enum>(res); });
}

template <typename Enum, typename Char, typename CharTraits>
[[nodiscard]] auto try_read_
(
    std::basic_istream<Char, CharTraits> &istr, 
    Enum,
    kkind_t<kind_t::combo>
) noexcept
    -> boost::optional<Enum>
{
    using enum_item_type = boost::spirit::qi::symbols<Char, std::ptrdiff_t>;

    if (!detail::check_stream_state(istr)) return {};

    auto const po = print_manip::value(istr);

    static auto const item = boost::fusion::fold
    (
        traits<Enum>::groups,
        enum_item_type{},
        []<typename Item>(Item && item, auto const& group)
        {
            for (auto const& rec : group)
            {
                auto const lit = rec.template get_str<Char, CharTraits>();
                auto const val = rec.m_val;
                item.add(lit, static_cast<std::ptrdiff_t>(val));
            }

            return std::forward<Item>(item);
        }
    );

    return try_parse_combo
    (
          istr
        , item
        , traits<Enum>::template get_name<Char>()
        , std::is_scoped_enum_v<Enum> && has_feature(po, print_t::class_prefix)
    ).map([](std::ptrdiff_t res) { return static_cast<Enum>(res); });
}

}

///////////////////////////////////////////////////////////////////////////////
template<c::enum_ Enum>
struct reader<Enum, kind_t::enum_>
{
    using kkind_type = kkind_t<kind_t::enum_>;

    Enum                                   &m_val;
    [[no_unique_address]] kkind_type        m_kind;

    reader(Enum &val, kkind_type kind): m_val { val }, m_kind{ kind } {}

    template <typename Char, typename CharTraits>
    auto read(std::basic_istream<Char, CharTraits> &istr) -> decltype(istr)
    {
        using enum_item_type = boost::spirit::qi::symbols<Char, std::ptrdiff_t>;
        using string_type = std::basic_string<Char, CharTraits>;

        static_assert(std::tuple_size_v<decltype(groups_v<Enum>)> == 1);

        auto const po = print_manip::value(istr);

        static auto const item = std::ranges::fold_left
        (
            std::get<0>(groups_v<Enum>).m_records
          , enum_item_type{}
          , [](auto &&item, auto const &rec) 
            {
                auto const lit = boost::lexical_cast<string_type>(rec);
                item.add(lit, static_cast<std::ptrdiff_t>(rec.m_value));
                return item;
            }
        );

        this->m_val = static_cast<Enum>(detail::parse_enum(istr, item, name(Enum{}, istr), po));

        return istr;
    }
};

template<c::enum_ Enum>
struct reader<Enum, kind_t::bitfield>
{
    using kkind_type = kkind_t<kind_t::bitfield>;

    Enum                                   &m_val;
    [[no_unique_address]] kkind_type        m_kind;

    reader(Enum &val, kkind_type kind): m_val{ val }, m_kind{ kind } {}

    template <typename Char, typename CharTraits>
    auto read(std::basic_istream<Char, CharTraits> &istr) -> decltype(istr)
    {
        using enum_item_type = boost::spirit::qi::symbols<Char, std::ptrdiff_t>;

        auto const po = print_manip::value(istr);

        static auto const item = boost::fusion::fold
        (
            groups_v<Enum>,
            enum_item_type{},
            []<typename Group>(auto &&item, Group const &group)
            {
                using string_type = std::basic_string<Char, CharTraits>;

                static_assert(group_::size_v<Group> == 1);

                auto const &rec = group.m_records.front();
                auto const lit = boost::lexical_cast<string_type>(rec);
                item.add(lit, static_cast<std::ptrdiff_t>(rec.m_value));

                return item;
            }
        );

        this->m_val = static_cast<Enum>(detail::parse_combo(istr, item, name(Enum{}, istr), po));

        return istr;
    }
};

template<c::enum_ Enum>
struct reader<Enum, kind_t::combo>
{
    using kkind_type = kkind_t<kind_t::combo>;

    Enum                                   &m_val;
    [[no_unique_address]] kkind_type        m_kind;

    reader(Enum &val, kkind_type kind): m_val { val }, m_kind{ kind } {}

    template <typename Char, typename CharTraits>
    auto read(std::basic_istream<Char, CharTraits> &istr) -> decltype(istr)
    {
        using enum_item_type = boost::spirit::qi::symbols<Char, std::ptrdiff_t>;

        auto const po = print_manip::value(istr);

        static auto const item = boost::fusion::fold
        (
            groups_v<Enum>,
            enum_item_type{},
            []<typename Item>(Item &&item, auto const &group)
            {
                return std::ranges::fold_left
                (
                    group.m_records,
                    std::forward<Item>(item),
                    [](auto &&item, auto const &rec)
                    {
                        using string_type = std::basic_string<Char, CharTraits>;

                        auto const lit = boost::lexical_cast<string_type>(rec);
                        item.add(lit, static_cast<std::ptrdiff_t>(rec.m_value));

                        return item;
                    }
                );
            }
        );

        this->m_val = static_cast<Enum>(detail::parse_combo(istr, item, name(Enum{}, istr), po));

        return istr;
    }
};

///////////////////////////////////////////////////////////////////////////////
template <c::e_any_enum Enum, typename Char, typename CharTraits>
[[nodiscard]] auto try_read(std::basic_istream<Char, CharTraits> &istr) noexcept -> boost::optional<Enum>
{
    return detail::try_read_(istr, Enum{}, kkind_t<kind_v<Enum>>{});
}

}
