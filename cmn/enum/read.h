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

#include "feature.h"
#include "traits.h"
#include "manip.h"

namespace cmn::enum_::io
{

namespace detail
{

template <typename Char, typename CharTraits>
auto check_stream_state(std::basic_ios<Char, CharTraits> const &istr) noexcept -> bool;

template <typename Char, typename CharTraits>
auto parse_enum
(
      std::basic_istream<Char, CharTraits>& istr
    , boost::spirit::qi::symbols<Char, std::ptrdiff_t> const& item
    , std::basic_string_view<Char, CharTraits> enum_name
    , bool is_scoped
)
    ->std::ptrdiff_t;

template <typename Char, typename CharTraits>
auto parse_combo
(
      std::basic_istream<Char, CharTraits>& istr
    , boost::spirit::qi::symbols<Char, std::ptrdiff_t> const& item
    , std::basic_string_view<Char, CharTraits> enum_name
    , bool is_scoped
)
    ->std::ptrdiff_t;

//-----------------------------------------------------------------------------
template <typename Char, typename CharTraits>
[[nodiscard]] auto try_parse_enum
(
    std::basic_istream<Char, CharTraits>& istr
    , boost::spirit::qi::symbols<Char, std::ptrdiff_t> const& item
    , std::basic_string_view<Char, CharTraits> enum_name
    , bool is_scoped
) noexcept
 -> boost::optional<std::ptrdiff_t>;

template <typename Char, typename CharTraits>
auto try_parse_combo
(
    std::basic_istream<Char, CharTraits>& istr
    , boost::spirit::qi::symbols<Char, std::ptrdiff_t> const& item
    , std::basic_string_view<Char, CharTraits> enum_name
    , bool is_scoped
) noexcept
 -> boost::optional<std::ptrdiff_t>;

//-----------------------------------------------------------------------------
template <typename Enum, typename Char, typename CharTraits>
[[nodiscard]] auto try_read_
(
    std::basic_istream<Char, CharTraits> &istr, 
    op::fwd<Enum>,
    kkind_t<kind_t::enum_>
) noexcept
    -> boost::optional<Enum>
{
    using utils_type = utils<Enum>;
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
            auto const val = rec.m_val;
            item.add(lit, static_cast<std::ptrdiff_t>(val));
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
    op::fwd<Enum>,
    kkind_t<kind_t::bitfield>
) noexcept
    -> boost::optional<Enum>
{
    using utils_type = utils<Enum>;
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
            auto const val = rec.m_val;
            item.add(lit, static_cast<std::ptrdiff_t>(val));

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
    op::fwd<Enum>,
    kkind_t<kind_t::combo>
) noexcept
    -> boost::optional<Enum>
{
    using utils_type = utils<Enum>;
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
struct reader<Enum, kind_t::enum_>: op::fwd<Enum &>
{
    using inherited = op::fwd<Enum &>;
    using kkind_type = kkind_t<kind_t::enum_>;

    [[no_unique_address]] kkind_type        m_kind;

    reader(Enum &val, kkind_type kind): inherited { val }, m_kind{ kind } {}

    template <typename Char, typename CharTraits>
    auto read(std::basic_istream<Char, CharTraits> &istr) -> decltype(istr)
    {
        using utils_type = typename inherited::utils_type;
        using enum_item_type = boost::spirit::qi::symbols<Char, std::ptrdiff_t>;

        static_assert(std::tuple_size_v<decltype(traits<Enum>::groups)> == 1);

        auto const po = print_manip::value(istr);

        static auto const item = std::ranges::fold_left
        (
            std::get<0>(traits<Enum>::groups)
          , enum_item_type{}
          , []<typename Item>(Item &&item, auto const &rec) 
            {
                auto const lit = rec.template get_str<Char, CharTraits>();
                auto const val = rec.m_val;
                item.add(lit, static_cast<std::ptrdiff_t>(val));
                return std::forward<Item>(item);
            }
        );

        this->m_val = static_cast<Enum>
        (
            detail::parse_enum
            (
                  istr
                , item
                , traits<Enum>::template get_name<Char>()
                , std::is_scoped_enum_v<Enum> && has_feature(po, print_t::class_prefix)
            )
        );

        return istr;
    }
};

template<c::enum_ Enum>
struct reader<Enum, kind_t::bitfield>: op::fwd<Enum &>
{
    using inherited = op::fwd<Enum &>;
    using kkind_type = kkind_t<kind_t::bitfield>;

    [[no_unique_address]] kkind_type        m_kind;

    reader(Enum &val, kkind_type kind): inherited { val }, m_kind{ kind } {}

    template <typename Char, typename CharTraits>
    auto read(std::basic_istream<Char, CharTraits> &istr) -> decltype(istr)
    {
        using utils_type = typename inherited::utils_type;
        using enum_item_type = boost::spirit::qi::symbols<Char, std::ptrdiff_t>;

        auto const po = print_manip::value(istr);

        static auto const item = boost::fusion::fold
        (
            traits<Enum>::groups,
            enum_item_type{},
            []<typename Item, typename Group>(Item &&item, Group const &group)
            {
                static_assert(std::tuple_size_v<Group> == 1);

                auto const& rec = group[0];
                auto const lit = rec.template get_str<Char, CharTraits>();
                auto const val = rec.m_val;
                item.add(lit, static_cast<std::ptrdiff_t>(val));

                return std::forward<Item>(item);
            }
        );

        this->m_val = static_cast<Enum>
        (
            detail::parse_combo
            (
                  istr
                , item
                , traits<Enum>::template get_name<Char>()
                , std::is_scoped_enum_v<Enum> && has_feature(po, print_t::class_prefix)
            )
        );

        return istr;
    }
};

template<c::enum_ Enum>
struct reader<Enum, kind_t::combo>: op::fwd<Enum &>
{
    using inherited = op::fwd<Enum &>;
    using kkind_type = kkind_t<kind_t::combo>;

    [[no_unique_address]] kkind_type        m_kind;

    reader(Enum &val, kkind_type kind): inherited { val }, m_kind{ kind } {}

    template <typename Char, typename CharTraits>
    auto read(std::basic_istream<Char, CharTraits> &istr) -> decltype(istr)
    {
        using utils_type = typename inherited::utils_type;
        using enum_item_type = boost::spirit::qi::symbols<Char, std::ptrdiff_t>;

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

        this->m_val = static_cast<Enum>
        (
            detail::parse_combo
            (
                  istr
                , item
                , traits<Enum>::template get_name<Char>()
                , std::is_scoped_enum_v<Enum> && has_feature(po, print_t::class_prefix)
            )
        );

        return istr;
    }
};

///////////////////////////////////////////////////////////////////////////////
template <c::ext_any_enum Enum, typename Char, typename CharTraits>
[[nodiscard]] auto try_read(std::basic_istream<Char, CharTraits> &istr) noexcept -> boost::optional<Enum>
{
    return detail::try_read_(istr, op::fwd<Enum>{ Enum{} }, kkind_t<kind_v<Enum>>{});
}

}
