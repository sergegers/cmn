
#include <iterator>

#include <boost/spirit/include/qi.hpp>
#include <boost/phoenix.hpp>

#include <boost/lexical_cast.hpp>

#include <cmn/error/exception.h>
#include <cmn/util/util.h>   // overloaded
#include <cmn/util/lexical_cast.h>
#include <cmn/util/feature.h>

#include "read.h"
#include "manip.h"

#pragma warning (push)
#pragma warning(disable: 4459)

namespace cmn::enum_::io::detail
{

namespace qi = boost::spirit::qi;  // NOLINT(misc-unused-alias-decls)

template <typename Char, typename CharTraits>
auto check_stream_state(std::basic_ios<Char, CharTraits> const &istr) noexcept -> bool
{
    return istr.rdstate() == std::ios_base::goodbit;
}

template auto check_stream_state(std::ios const &) noexcept -> bool;
template auto check_stream_state(std::wios const &) noexcept -> bool;

template <typename Char, typename CharTraits>
auto get_print_options(std::basic_istream<Char, CharTraits> &istr) -> print_t
{
    
}

template <typename Char, typename CharTraits>
auto get_open(std::basic_istream<Char, CharTraits> &istr)
{
    return basic_open_manip<Char, CharTraits>::value(istr);
}

template <typename Char, typename CharTraits>
auto get_close(std::basic_istream<Char, CharTraits> &istr)
{
    return basic_close_manip<Char, CharTraits>::value(istr);
}

template <typename Char, typename CharTraits>
auto get_separator(std::basic_istream<Char, CharTraits> &istr)
{
    return basic_bitfield_separator_manip<Char, CharTraits>::value(istr);
}


// NOTE: transfer istream iterators by reference
// because they don't meet iterator requirements
template <typename Char, typename CharTraits>
constexpr auto check_errors
(
      bool parse_result
    , boost::spirit::basic_istream_iterator<Char, CharTraits> const &begin
    , boost::spirit::basic_istream_iterator<Char, CharTraits> const &last
    , boost::spirit::basic_istream_iterator<Char, CharTraits> const &end
)noexcept -> bool
{
    return parse_result && last == end;
}

// NOTE: transfer istream iterators by reference
// because they don't meet iterator requirements
template <typename Char, typename CharTraits>
auto check_result
(
      bool parse_result
    , boost::spirit::basic_istream_iterator<Char, CharTraits> const &begin
    , boost::spirit::basic_istream_iterator<Char, CharTraits> const &last
    , boost::spirit::basic_istream_iterator<Char, CharTraits> const &end
)
{
    using string_type = std::basic_string<Char, CharTraits>;

    constexpr auto to_string = overloaded
    {
        [](std::string const &str) { return str; },
        [](std::wstring const &wstr) { return boost::lexical_cast<std::string>(wstr); }
    };

    if (!parse_result)
    {
        // begin, end aren't contiguous iterators, so they couldn't be used directly
        string_type text;
        std::copy(begin, end, std::back_inserter(text));

        BOOST_THROW_EXCEPTION((io_error{ "Parsing failed for \xB2{0}\xB1", to_string(text) }));
    }

    if (last != end)
    {
        // begin, end aren't contiguous iterators, so they couldn't be used directly
        string_type text;
        std::copy(begin, end, std::back_inserter(text));

        BOOST_THROW_EXCEPTION
        ((
            io_error{ "Text is not parsed" }
            << error_::errinfo_msg
              {
                {
                        .m_msg = to_string(text),
                        .m_open = static_cast<std::size_t>(std::distance(begin, last))
                    }
                }
        ));
    }
}

///////////////////////////////////////////////////////////////////////////////
template <typename Char, typename CharTraits> auto parse_enum
(
      std::basic_istream<Char, CharTraits> &istr
    , qi::symbols<Char, std::ptrdiff_t> const &item
    , basic_qualified_name<Char, CharTraits> enum_name
    , print_t po
)
    -> std::ptrdiff_t
{
    using namespace qi;
    using iterator_type = boost::spirit::basic_istream_iterator<Char, CharTraits>;

    auto const open = get_open(istr);
    auto const close = get_close(istr);

    std::ptrdiff_t items = 0;
    auto const begin = iterator_type{ istr };
    auto last = begin;
    auto const end = iterator_type{};

    bool parse_result;
    if (has_feature(po, print_t::class_prefix))
    {
        parse_result = phrase_parse
        (
              last
            , end
            , lit(open) >> lit(enum_name.class_prefix()) >> item >> lit(close)
            , space
            , items
        );
    }
    else
    {
        parse_result = phrase_parse
        (
              last
            , end
            , lit(open) >> item >> lit(close)
            , space
            , items
        );
    }

    check_result(parse_result, begin, last, end);

    return items;
}

template auto parse_enum(std::istream &, qi::symbols<char, std::ptrdiff_t> const &, qualified_name, print_t) -> std::ptrdiff_t;
template auto parse_enum(std::wistream &, qi::symbols<wchar_t, std::ptrdiff_t> const &, wqualified_name, print_t) -> std::ptrdiff_t;

//-----------------------------------------------------------------------------
template <typename Char, typename CharTraits>
auto try_parse_enum
(
    std::basic_istream<Char, CharTraits>& istr
    , qi::symbols<Char, std::ptrdiff_t> const& item
    , basic_qualified_name<Char, CharTraits> enum_name
    , print_t po
) noexcept
 -> boost::optional<std::ptrdiff_t>
{
    using namespace qi;
    using iterator_type = boost::spirit::basic_istream_iterator<Char, CharTraits>;

    auto const open = get_open(istr);
    auto const close = get_close(istr);

    std::ptrdiff_t items = 0;
    auto const begin = iterator_type{ istr };
    auto last = begin;
    auto const end = iterator_type{};

    if (has_feature(po, print_t::class_prefix))
    {
        if
        ( 
            check_errors
            (
                phrase_parse
                (
                      last
                    , end
                    , lit(open) >> lit(enum_name.class_prefix()) >> item >> lit(close)
                    , space
                    , items
                ),
                begin, last, end
            )
        )
            return items;
        else
            return {};
    }
    else
    {
        if
        (
            check_errors
            (
                phrase_parse
                (
                      last
                    , end
                    , lit(open) >> item >> lit(close)
                    , space
                    , items
                ),
                begin, last, end
            )
        )
            return items;
        else
            return {};
    }
}

template auto try_parse_enum(std::istream &, qi::symbols<char, std::ptrdiff_t> const &, 
    qualified_name, print_t) noexcept -> boost::optional<std::ptrdiff_t>;
template auto try_parse_enum(std::wistream &, qi::symbols<wchar_t, std::ptrdiff_t> const &, 
    wqualified_name, print_t) noexcept -> boost::optional<std::ptrdiff_t>;

///////////////////////////////////////////////////////////////////////////////
template <typename Char, typename CharTraits>
auto parse_combo
(
      std::basic_istream<Char, CharTraits>& istr
    , qi::symbols<Char, std::ptrdiff_t> const &item
    , basic_qualified_name<Char, CharTraits> enum_name
    , print_t po
)
    ->std::ptrdiff_t
{
    using string_type = std::basic_string<Char, CharTraits>;
    using string_view_type = std::basic_string_view<Char, CharTraits>;
    using enum_item_type = qi::symbols<Char, std::ptrdiff_t>;
    using iterator_type = boost::spirit::basic_istream_iterator<Char, CharTraits>;

    auto const open = get_open(istr);
    auto const close = get_close(istr);
    auto const separator = basic_bitfield_separator_manip<Char, CharTraits>::value(istr);

    std::ptrdiff_t items = 0;
    auto const begin = iterator_type{ istr };
    auto last = begin;
    auto const end = iterator_type{};
    if (has_feature(po, print_t::class_prefix))
    {
        struct parser: qi::grammar<iterator_type, std::ptrdiff_t(), qi::space_type>
        {
            qi::rule<iterator_type, std::ptrdiff_t(), qi::space_type>     value;
            qi::rule<iterator_type, std::ptrdiff_t(), qi::space_type>     items;

            parser
            (
                  enum_item_type const &item 
                , string_type const &open
                , string_type const &close
                , string_type const &separator
                , string_type const &pfx 
            ): parser::base_type { items }
            {
                using namespace qi;

                value   = lexeme[(lit(pfx) >> item[_val |= _1/*, std::cout << _val*/]) % lit(separator)];
                items    = lit(open) >> value >> lit(close);

                //BOOST_SPIRIT_DEBUG_NODES
                //(
                //    (value)
                //)
            }
        }

        const parser{ item, open, close, separator, enum_name.class_prefix() };
        check_result
        (
            qi::phrase_parse(last, end, parser, qi::space, items),
            begin, last, end
        );
    }
    else
    {
        struct parser: qi::grammar<iterator_type, std::ptrdiff_t(), qi::space_type>
        {
            qi::rule<iterator_type, std::ptrdiff_t(), qi::space_type>     value;
            qi::rule<iterator_type, std::ptrdiff_t(), qi::space_type>     items;

            parser
            (
                  enum_item_type const &item 
                , string_type const & open
                , string_type const & close
                , string_type const & separator
            ): parser::base_type { items }
            {
                using namespace qi;

                value       = lexeme[item[_val |= _1/*, std::cout << _val << "\n"*/] % lit(separator)];
                items       = lit(open) >> value >> lit(close);

                //BOOST_SPIRIT_DEBUG_NODES
                //(
                //    (value)
                //    (items)
                //)
            }
        }

        const parser { item, open, close, separator };
        check_result
        (
            qi::phrase_parse(last, end, parser, qi::space, items),
            begin, last, end
        );
    }

    return items;
}

template auto parse_combo(std::istream &, qi::symbols<char, std::ptrdiff_t> const &, qualified_name, print_t) -> std::ptrdiff_t;
template auto parse_combo(std::wistream &, qi::symbols<wchar_t, std::ptrdiff_t> const &, wqualified_name, print_t) -> std::ptrdiff_t;

//-----------------------------------------------------------------------------
template <typename Char, typename CharTraits>
auto try_parse_combo
(
    std::basic_istream<Char, CharTraits>& istr
    , qi::symbols<Char, std::ptrdiff_t> const& item
    , basic_qualified_name<Char, CharTraits> enum_name
    , print_t po
) noexcept
 -> boost::optional<std::ptrdiff_t>
{
    using string_type = std::basic_string<Char, CharTraits>;
    using string_view_type = std::basic_string_view<Char, CharTraits>;
    using enum_item_type = qi::symbols<Char, std::ptrdiff_t>;
    using iterator_type = boost::spirit::basic_istream_iterator<Char, CharTraits>;

    auto const open = get_open(istr);
    auto const close = get_close(istr);
    auto const separator = basic_bitfield_separator_manip<Char, CharTraits>::value(istr);

    std::ptrdiff_t items = 0;
    auto const begin = iterator_type{ istr };
    auto last = begin;
    auto const end = iterator_type{};
    if (has_feature(po, print_t::class_prefix))
    {
        struct parser: qi::grammar<iterator_type, std::ptrdiff_t(), qi::space_type>
        {
            qi::rule<iterator_type, std::ptrdiff_t(), qi::space_type>     value;
            qi::rule<iterator_type, std::ptrdiff_t(), qi::space_type>     items;

            parser
            (
                  enum_item_type const &item 
                , string_type const &open
                , string_type const &close
                , string_type const &separator
                , string_type const &pfx 
            ): parser::base_type { items }
            {
                using namespace qi;

                value   = lexeme[(lit(pfx) >> item[_val |= _1/*, std::cout << _val*/]) % lit(separator)];
                items    = lit(open) >> value >> lit(close);

                //BOOST_SPIRIT_DEBUG_NODES
                //(
                //    (value)
                //)
            }
        }

        const parser{ item, open, close, separator, enum_name.class_prefix() };
        if
        (
            check_errors
            (
                qi::phrase_parse(last, end, parser, qi::space, items),
                begin, last, end
            )
        )
            return items;
        else
            return {};
    }
    else
    {
        struct parser: qi::grammar<iterator_type, std::ptrdiff_t(), qi::space_type>
        {
            qi::rule<iterator_type, std::ptrdiff_t(), qi::space_type>     value;
            qi::rule<iterator_type, std::ptrdiff_t(), qi::space_type>     items;

            parser
            (
                  enum_item_type const &item 
                , string_type const & open
                , string_type const & close
                , string_type const & separator
            ): parser::base_type { items }
            {
                using namespace qi;

                value       = lexeme[item[_val |= _1/*, std::cout << _val << "\n"*/] % lit(separator)];
                items       = lit(open) >> value >> lit(close);

                //BOOST_SPIRIT_DEBUG_NODES
                //(
                //    (value)
                //    (items)
                //)
            }
        }

        const parser { item, open, close, separator };
        if
        (
            check_errors
            (
                qi::phrase_parse(last, end, parser, qi::space, items),
                begin, last, end
            )
        )
            return items;
        else
            return {};
    }
}

template auto try_parse_combo(std::istream &, qi::symbols<char, std::ptrdiff_t> const &,
    qualified_name, print_t) noexcept -> boost::optional<std::ptrdiff_t>;
template auto try_parse_combo(std::wistream &, qi::symbols<wchar_t, std::ptrdiff_t> const &,
    wqualified_name, print_t) noexcept -> boost::optional<std::ptrdiff_t>;

}

#pragma warning(pop)