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

#include <cmn/meta/concepts.h>

#include <cmn/util/feature.h>

#include <cmn/enum/print_t.h>
#include <cmn/enum/traits.h>

#include "manip.h"

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
template <typename E, typename Char, typename CharTraits>
[[nodiscard]] auto try_read_
(
    std::basic_istream<Char, CharTraits> &istr, 
    E en,
    int_<kind_t::enum_>
) noexcept
    -> boost::optional<E>
{
    using enum_item_type = boost::spirit::qi::symbols<Char, std::ptrdiff_t>;

    static_assert(std::tuple_size_v<decltype(groups_v<E>)> == 1);

    if (!detail::check_stream_state(istr)) return {};

    auto const po = print_manip::value(istr);

    static auto const item = std::ranges::fold_left
    (
        std::get<0>(groups_v<E>).m_records
      , enum_item_type{}
      , [&istr](auto &&item, auto const &rec) 
        {
            auto const lit = rec.name(istr).m_enum_member_name;
            item.add(lit, static_cast<std::ptrdiff_t>(rec.m_value));
            return item;
        }
    );

    return try_parse_enum
    (
          istr
        , item
        , enum_::name(en, istr)
        , po
    ).map([](std::ptrdiff_t res) { return static_cast<E>(res); });
}

template <typename E, typename Char, typename CharTraits>
[[nodiscard]] auto try_read_
(
    std::basic_istream<Char, CharTraits> &istr, 
    E,
    int_<kind_t::bitfield>
) noexcept
    -> boost::optional<E>
{
    using enum_item_type = boost::spirit::qi::symbols<Char, std::ptrdiff_t>;

    if (!detail::check_stream_state(istr)) return {};

    auto const po = print_manip::value(istr);

    static auto const item = boost::fusion::fold
    (
        groups_v<E>,
        enum_item_type{},
        []<typename Group>(auto && item, Group const& group)
        {
            static_assert(group_::size_v<Group> == 1);

            auto const& rec = group[0];
            auto const lit = rec.template get_str<Char, CharTraits>();
            item.add(lit, static_cast<std::ptrdiff_t>(rec.m_value));

            return item;
        }
    );

    return try_parse_combo
    (
          istr
        , item
        , name(istr)
        , std::is_scoped_enum_v<E> && has_feature(po, print_t::class_prefix)
    ).map([](std::ptrdiff_t res) { return static_cast<E>(res); });
}

template <typename E, typename Char, typename CharTraits>
[[nodiscard]] auto try_read_
(
    std::basic_istream<Char, CharTraits> &istr, 
    E,
    int_<kind_t::combo>
) noexcept
    -> boost::optional<E>
{
    using enum_item_type = boost::spirit::qi::symbols<Char, std::ptrdiff_t>;

    if (!detail::check_stream_state(istr)) return {};

    auto const po = print_manip::value(istr);

    static auto const item = boost::fusion::fold
    (
        groups_v<E>,
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
        , name(istr)
        , po
    ).map([](std::ptrdiff_t res) { return static_cast<E>(res); });
}

}

///////////////////////////////////////////////////////////////////////////////
template<c::enum_ E>
struct reader<E, kind_t::enum_>
{
    using kkind_type = int_<kind_t::enum_>;

    E                                   &m_val;
    [[no_unique_address]] kkind_type    m_kind;

    reader(E &val, kkind_type kind): m_val { val }, m_kind{ kind } {}

    template <typename Char, typename CharTraits>
    auto read(std::basic_istream<Char, CharTraits> &istr) -> decltype(istr)
    {
        using enum_item_type = boost::spirit::qi::symbols<Char, std::ptrdiff_t>;

        static_assert(std::tuple_size_v<decltype(groups_v<E>)> == 1);

        auto const po = print_manip::value(istr);

        static auto const item = std::ranges::fold_left
        (
            std::get<0>(groups_v<E>).m_records
          , enum_item_type{}
          , [&istr](auto &&item, auto const &rec) 
            {
                auto const lit = rec.name(istr).m_enum_member_name;
                item.add(lit, static_cast<std::ptrdiff_t>(rec.m_value));
                return item;
            }
        );

        this->m_val = static_cast<E>(detail::parse_enum(istr, item, name(E{}, istr), po));

        return istr;
    }
};

template<c::enum_ E>
struct reader<E, kind_t::bitfield>
{
    using kkind_type = int_<kind_t::bitfield>;

    E                                   &m_val;
    [[no_unique_address]] kkind_type    m_kind;

    reader(E &val, kkind_type kind): m_val{ val }, m_kind{ kind } {}

    template <typename Char, typename CharTraits>
    auto read(std::basic_istream<Char, CharTraits> &istr) -> decltype(istr)
    {
        using enum_item_type = boost::spirit::qi::symbols<Char, std::ptrdiff_t>;

        auto const po = print_manip::value(istr);

        static auto const item = boost::fusion::fold
        (
            groups_v<E>,
            enum_item_type{},
            [&istr]<typename Group>(auto &&item, Group const &group)
            {
                using string_type = std::basic_string<Char, CharTraits>;

                static_assert(group_::size_v<Group> == 1);

                auto const &rec = group.m_records.front();
                auto const lit = rec.name(istr).m_enum_member_name;
                item.add(lit, static_cast<std::ptrdiff_t>(rec.m_value));

                return item;
            }
        );

        this->m_val = static_cast<E>(detail::parse_combo(istr, item, name(E{}, istr), po));

        return istr;
    }
};

template<c::enum_ E>
struct reader<E, kind_t::combo>
{
    using kkind_type = int_<kind_t::combo>;

    E                                   &m_val;
    [[no_unique_address]] kkind_type    m_kind;

    reader(E &val, kkind_type kind): m_val { val }, m_kind{ kind } {}

    template <typename Char, typename CharTraits>
    auto read(std::basic_istream<Char, CharTraits> &istr) -> decltype(istr)
    {
        using enum_item_type = boost::spirit::qi::symbols<Char, std::ptrdiff_t>;

        auto const po = print_manip::value(istr);

        static auto const item = boost::fusion::fold
        (
            groups_v<E>,
            enum_item_type{},
            [&istr]<typename Item>(Item &&item, auto const &group)
            {
                return std::ranges::fold_left
                (
                    group.m_records,
                    std::forward<Item>(item),
                    [&istr](auto &&item, auto const &rec)
                    {
                        auto const lit = rec.name(istr).m_enum_member_name;
                        item.add(lit, static_cast<std::ptrdiff_t>(rec.m_value));

                        return item;
                    }
                );
            }
        );

        this->m_val = static_cast<E>(detail::parse_combo(istr, item, name(E{}, istr), po));

        return istr;
    }
};

///////////////////////////////////////////////////////////////////////////////
template <c::adapted_enum E, typename Char, typename CharTraits>
[[nodiscard]] auto try_read(std::basic_istream<Char, CharTraits> &istr) noexcept -> boost::optional<E>
{
    return detail::try_read_(istr, E{}, int_<kind_v<E>>{});
}

}
