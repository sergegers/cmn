#pragma once

#include <cstddef>
#include <iosfwd>
#include <string>
#include <string_view>
#include <type_traits>
#include <ranges>
#include <algorithm>

#include <boost/fusion/algorithm/iteration/fold.hpp>
#include <boost/spirit/include/qi_symbols.hpp>
#include <boost/spirit/include/support_istream_iterator.hpp>
// ReSharper disable once CppUnusedIncludeDirective
#include <boost/optional/optional_fwd.hpp>

#include <cmn/meta/concepts.h>

#include <cmn/util/feature.h>

#include <cmn/enum/traits.h>
#include <cmn/enum/io/format_traits.h>

#include "manip.h"
#include "parser.h"
#include "print.h"

namespace cmn::enum_::io
{

namespace detail
{

template <c::adapted_enum E, typename Char, typename CharTraits>
constexpr auto prepare_enum_items(int_<kind_t::enum_>) -> boost::spirit::qi::symbols<Char, std::ptrdiff_t>
{
    using string_type = std::basic_string<Char, CharTraits>;
    using enum_item_type = boost::spirit::qi::symbols<Char, std::ptrdiff_t>;

    static_assert(std::tuple_size_v<decltype(groups_v<E>)> == 1);

    return std::ranges::fold_left
    (
        std::get<0>(groups_v<E>).m_records
      , enum_item_type{}
      , [](auto &&items, auto const &rec) 
        {
            auto const enum_member_name = rec.template name<Char, CharTraits>().m_enum_member_name;
            items.add(string_type{ enum_member_name }, rec.as_interop());
            return items;
        }
    );
}

template <c::adapted_enum E, typename Char, typename CharTraits>
constexpr auto prepare_enum_items(int_<kind_t::bitfield>) -> boost::spirit::qi::symbols<Char, std::ptrdiff_t>
{
    using string_type = std::basic_string<Char, CharTraits>;
    using enum_item_type = boost::spirit::qi::symbols<Char, std::ptrdiff_t>;

    return boost::fusion::fold
    (
        groups_v<E>,
        enum_item_type{},
        []<typename Group>(auto &&items, Group const& group)
        {
            static_assert(group_::size_v<Group> == 1, "Bitfield group must contain the one and only one record");

            auto const &rec = group.m_records[0];
            auto const enum_member_name = rec.template name<Char, CharTraits>().m_enum_member_name;
            items.add(string_type{ enum_member_name }, rec.as_interop());

            return items;
        }
    );
}

template <c::adapted_enum E, typename Char, typename CharTraits>
constexpr auto prepare_enum_items(int_<kind_t::combo>) -> boost::spirit::qi::symbols<Char, std::ptrdiff_t>
{
    using string_type = std::basic_string<Char, CharTraits>;
    using enum_item_type = boost::spirit::qi::symbols<Char, std::ptrdiff_t>;

    return boost::fusion::fold
    (
        groups_v<E>,
        enum_item_type{},
        [](auto &&items, auto const& group)
        {
            for (auto const &rec : group.m_records)
            {
                auto const enum_member_name = rec.template name<Char, CharTraits>().m_enum_member_name;
                items.add(string_type{ enum_member_name }, rec.as_interop());
            }

            return items;
        }
    );
}

//-----------------------------------------------------------------------------
template <typename E, typename Char, typename CharTraits>
[[nodiscard]] auto try_read_
(
    std::basic_istream<Char, CharTraits> &istr, 
    E en,
    int_<kind_t::enum_> kind
) noexcept
    -> boost::optional<E>
{
    using enum_item_type = boost::spirit::qi::symbols<Char, std::ptrdiff_t>;

    static_assert(std::tuple_size_v<decltype(groups_v<E>)> == 1);

    if (istr.rdstate() != std::ios_base::goodbit) return {};

    using open_manip_type = basic_open_manip<Char, CharTraits>;
    using close_manip_type = basic_close_manip<Char, CharTraits>;
    using separator_manip_type = basic_bitfield_delimiter_manip<Char, CharTraits>;
    using istream_iterator_type = boost::spirit::basic_istream_iterator<Char, CharTraits>;

    static auto const items = detail::prepare_enum_items<E, Char, CharTraits>(kind);

    sink_format_options const fmt_specs
    {
        .options = print_manip::value(istr),
        .open = open_manip_type::value(istr),
        .separator = separator_manip_type::value(istr),
        .close = close_manip_type::value(istr),
    };

    return try_parse
    (
          kind
        , items
        , name(en, istr)
        , fmt_specs
        , istream_iterator_type{ istr }
        , istream_iterator_type{}
    ).map([](std::ptrdiff_t res) { return static_cast<E>(res); });
}

template <typename E, typename Char, typename CharTraits>
[[nodiscard]] auto try_read_
(
    std::basic_istream<Char, CharTraits> &istr, 
    E,
    int_<kind_t::bitfield> kind
) noexcept
    -> boost::optional<E>
{
    using enum_item_type = boost::spirit::qi::symbols<Char, std::ptrdiff_t>;

    if (istr.rdstate() != std::ios_base::goodbit) return {};

    auto const po = print_manip::value(istr);

    static auto const items = prepare_enum_items<E, Char, CharTraits>(kind);

    return try_parse_combo
    (
          istr
        , items
        , name(istr)
        , std::is_scoped_enum_v<E> && has_feature(po, print_t::class_prefix)
    ).map([](std::ptrdiff_t res) { return static_cast<E>(res); });
}

template <typename E, typename Char, typename CharTraits>
[[nodiscard]] auto try_read_
(
    std::basic_istream<Char, CharTraits> &istr, 
    E,
    int_<kind_t::combo> kind
) noexcept
    -> boost::optional<E>
{
    if (istr.rdstate() != std::ios_base::goodbit) return {};

    auto const po = print_manip::value(istr);

    static auto const items = prepare_enum_items<E, Char, CharTraits>(kind);

    return try_parse_combo
    (
          istr
        , items
        , name(istr)
        , po
    ).map([](std::ptrdiff_t res) { return static_cast<E>(res); });
}

}


///////////////////////////////////////////////////////////////////////////////
template<c::adapted_enum E, kind_t Kind_>
struct reader<E, Kind_>
{
    using kkind_type = int_<Kind_>;

    E                                   &m_val;
    [[no_unique_address]] kkind_type    m_kind;

    reader(E &val, kkind_type kind): m_val{ val }, m_kind{ kind } {}

    template <typename Char, typename CharTraits>
    auto read(std::basic_istream<Char, CharTraits> &istr) -> decltype(istr)
    {
        using open_manip_type = basic_open_manip<Char, CharTraits>;
        using close_manip_type = basic_close_manip<Char, CharTraits>;
        using delimiter_manip_type = basic_bitfield_delimiter_manip<Char, CharTraits>;
        using istream_iterator_type = boost::spirit::basic_istream_iterator<Char, CharTraits>;

        static auto const items = detail::prepare_enum_items<E, Char, CharTraits>(m_kind);

        sink_format_options const fmt_opt
        {
            .options = print_manip::value(istr),
            .open = open_manip_type::value(istr),
            .close = close_manip_type::value(istr),
            .delimiter = delimiter_manip_type::value(istr),
        };

        this->m_val = static_cast<E>
        (
            parse
            (
                  m_kind
                , items
                , name(E{}, istr)
                , fmt_opt
                , istream_iterator_type{ istr }
                , istream_iterator_type{}
            )
        );

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
