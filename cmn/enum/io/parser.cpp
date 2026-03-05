
#include <string>
#include <string_view>
#include <iterator>

#include <boost/preprocessor/cat.hpp>

#include <boost/exception/all.hpp>
#include <boost/optional.hpp>
#include <boost/none.hpp>
#include <boost/lexical_cast.hpp>

#include <boost/spirit/include/qi.hpp>
#include <boost/phoenix.hpp>

#if __has_include(<boost/spirit/ext.hpp>)
#   include <boost/spirit/ext.hpp>
#else
#   include <cmn/meta/boost/spirit/ext.hpp>
#endif

#include <cmn/meta/macro.h>
#include <cmn/error/exception.h>
#include <cmn/util/lexical_cast.h>

#include "parser.h"

namespace cmn::enum_::io
{

namespace
{

namespace qi = boost::spirit::qi;

//-----------------------------------------------------------------------------
// NOTE: transfer istream iterators by reference
// because they don't meet iterator requirements

template </*std::input_iterator*/typename It>
struct result
{
    bool            m_processed;
    It const        &m_begin;
    It              m_last;
    It const        &m_end;
    std::ptrdiff_t  m_items;

    result(It const &begin, It const &end):
          m_processed{ false }
        , m_begin{ begin }
        , m_last{ begin }
        , m_end{ end }
        , m_items{ 0 }
    {}

    constexpr operator bool() const noexcept { return m_processed; }
};

template </*std::input_iterator*/typename It>
constexpr auto items_or_empty(result<It> const &res) noexcept -> boost::optional<std::ptrdiff_t>
{
    return res.m_processed && res.m_last == res.m_end? 
        boost::optional<std::ptrdiff_t>{ res.m_items }:
        boost::none
    ;
}

//-----------------------------------------------------------------------------
template
<
      typename Char
    , typename CharTraits
    , /*std::input_iterator*/ typename It
>
auto items_or_throw(result<It> const &res)
{
    using string_type = std::basic_string<Char, CharTraits>;

    constexpr auto to_string = overloaded
    {
        [](std::string const &str) { return str; },
        [](std::wstring const &wstr) { return boost::lexical_cast<std::string>(wstr); }
    };

    if (!res)
    {
        // begin, end aren't contiguous iterators, so they couldn't be used directly
        string_type text;
        std::copy(res.m_begin, res.m_end, std::back_inserter(text));

        string_type unparsed;
        std::copy(res.m_last, res.m_end, std::back_inserter(unparsed));

        BOOST_THROW_EXCEPTION
        (
            (
                io_error
                { 
                    "Parsing failed for \xB2{0}\xB1\nPartial parse result: {1}\nStopped at: \xB2{2}\xB1", 
                    to_string(text), 
                    res.m_items,
                    to_string(unparsed)
                }
            )
        );
    }

    if (res.m_last != res.m_end)
    {
        // begin, end aren't contiguous iterators, so they couldn't be used directly
        string_type text;
        std::copy(res.m_begin, res.m_end, std::back_inserter(text));

        BOOST_THROW_EXCEPTION
        ((
            io_error{ "Text is not parsed" }
            << error_::errinfo_msg
              {
                {
                        .m_msg = to_string(text),
                        .m_open = static_cast<std::size_t>(std::distance(res.m_begin, res.m_last))
                    }
                }
        ));
    }

    return res.m_items;
}

///////////////////////////////////////////////////////////////////////////////
template
<
      typename Char
    , typename CharTraits
    , /*std::input_iterator*/ typename It
>
auto try_parse_enum
(
      qi::symbols<Char, std::ptrdiff_t> const &item
    , basic_qualified_name<Char, CharTraits> const &enum_name
    , basic_sink_format_options<Char, CharTraits> const &fmt_opt
    , It const &begin
    , It const &end
) noexcept -> result<It>
{
    result res { begin, end };

    if (has_feature(fmt_opt.options, print_t::class_prefix))
    {
        res.m_processed = qi::phrase_parse
        (
          res.m_last
            ,  res.m_end
            , qi::lit(fmt_opt.open) >> qi::lit(enum_name.class_prefix()) >> item >> qi::lit(fmt_opt.close)
            , qi::space
            , res.m_items
        );
    }
    else
    {
        res.m_processed = qi::phrase_parse
        (
              res.m_last
                    , res.m_end
                    , qi::lit(fmt_opt.open) >> item >> qi::lit(fmt_opt.close)
                    , qi::space
                    , res.m_items
        );
    }

    return res;
}

//-----------------------------------------------------------------------------
template
<
      typename Char
    , typename CharTraits
    , /*std::input_iterator*/ typename It
>
auto try_parse_bitfield
(
      qi::symbols<Char, std::ptrdiff_t> const &items_
    , basic_qualified_name<Char, CharTraits> const &enum_name
    , basic_sink_format_options<Char, CharTraits> const &fmt_opt
    , It const &begin
    , It const &end
) noexcept -> result<It>
{
    result res { begin, end };

    using string_type = std::basic_string<Char, CharTraits>;
    using enum_item_type = qi::symbols<Char, std::ptrdiff_t>;
    using iterator_type = It;

    if (has_feature(fmt_opt.options, print_t::class_prefix))
    {
        struct parser : qi::grammar<iterator_type, std::ptrdiff_t(), qi::space_type>
        {
            qi::rule<iterator_type, std::ptrdiff_t(), qi::space_type>     value;
            qi::rule<iterator_type, std::ptrdiff_t(), qi::space_type>     items;

            parser
            (
                enum_item_type const &item
                , string_type const &open
                , string_type const &close
                , string_type const &delimiter
                , string_type const &pfx
            ) : parser::base_type{ items }
            {
                using namespace qi;

                value = lexeme[(lit(pfx) >> item[_val |= _1/*, std::cout << _val*/]) % lit(delimiter)];
                items = lit(open) >> value >> lit(close);

                //BOOST_SPIRIT_DEBUG_NODES
                //(
                //    (value)
                //)
            }
        }

        const parser
        {
            items_,
            fmt_opt.open,
            fmt_opt.close,
            fmt_opt.delimiter,
            enum_name.class_prefix()
        };
        res.m_processed = qi::phrase_parse(res.m_last, res.m_end, parser, qi::space, res.m_items);
    }
    else
    {
            struct parser : qi::grammar<iterator_type, std::ptrdiff_t(), qi::space_type>
            {
                qi::rule<iterator_type, std::ptrdiff_t(), qi::space_type>     value;
                qi::rule<iterator_type, std::ptrdiff_t(), qi::space_type>     items;

                parser
                (
                    enum_item_type const &item
                    , string_type const &open
                    , string_type const &close
                    , string_type const &delimiter
                ) : parser::base_type{ items }
                {
                    using namespace qi;

                    value = lexeme[item[_val |= _1/*, std::cout << _val << "\n"*/] % lit(delimiter)];
                    items = lit(open) >> value >> lit(close);

                    //BOOST_SPIRIT_DEBUG_NODES
                    //(
                    //    (value)
                    //    (items)
                    //)
                }
            }

            const parser{ items_, fmt_opt.open, fmt_opt.close, fmt_opt.delimiter };
            res.m_processed = qi::phrase_parse(res.m_last, res.m_end, parser, qi::space, res.m_items);
    }

    return res;
}

}

///////////////////////////////////////////////////////////////////////////////

// parse kind_t::enum
template
<
      typename Char
    , typename CharTraits
    , /*std::input_iterator*/ typename It
>
auto try_parse
(
      int_<kind_t::enum_>
    , qi::symbols<Char, std::ptrdiff_t> const &items
    , basic_qualified_name<Char, CharTraits> const &enum_name
    , basic_sink_format_options<Char, CharTraits> const &fmt_opt
    , It const &begin
    , It const &end
)
  noexcept -> boost::optional<std::ptrdiff_t>
{
    return items_or_empty(try_parse_enum(items, enum_name, fmt_opt, begin, end));
}
// parse kind_t::bitfield
template
<
      typename Char
    , typename CharTraits
    , typename /*std::input_iterator*/ It
>
auto try_parse
(
      int_<kind_t::bitfield>
    , qi::symbols<Char, std::ptrdiff_t> const &items
    , basic_qualified_name<Char, CharTraits> const &enum_name
    , basic_sink_format_options<Char, CharTraits> const &fmt_opt
    , It const &begin
    , It const &end
)
    noexcept -> boost::optional<std::ptrdiff_t>
{
    return items_or_empty(try_parse_bitfield(items, enum_name, fmt_opt, begin, end));
}

// parse kind_t::combo
template
<
      typename Char
    , typename CharTraits
    , typename /*std::input_iterator*/ It
>
auto try_parse
(
      int_<kind_t::combo>
    , qi::symbols<Char, std::ptrdiff_t> const &items
    , basic_qualified_name<Char, CharTraits> const &enum_name
    , basic_sink_format_options<Char, CharTraits> const &fmt_opt
    , It const &begin
    , It const &end
)
    noexcept -> boost::optional<std::ptrdiff_t>
{
    return items_or_empty(try_parse_bitfield(items, enum_name, fmt_opt, begin, end));
}

#define CMN_INSTANTIATE_TRY_PARSE(kind_, char_, iterator_) \
    template auto try_parse    \
    (   \
          int_<BOOST_PP_CAT(kind_t::, kind_)>   \
        , boost::spirit::qi::symbols<char_, std::ptrdiff_t> const &  \
        , basic_qualified_name<char_> const &    \
        , basic_sink_format_options<char_> const & \
        , iterator_ const &   \
        , iterator_ const &  \
    )   \
        noexcept -> boost::optional<std::ptrdiff_t>;

CMN_INSTANTIATE_TRY_PARSE(enum_, char, boost::spirit::istream_iterator)
CMN_INSTANTIATE_TRY_PARSE(bitfield, char, boost::spirit::istream_iterator)
CMN_INSTANTIATE_TRY_PARSE(combo, char, boost::spirit::istream_iterator)
CMN_INSTANTIATE_TRY_PARSE(combo, char, std::string::iterator)

CMN_INSTANTIATE_TRY_PARSE(enum_, wchar_t, boost::spirit::wistream_iterator)
CMN_INSTANTIATE_TRY_PARSE(bitfield, wchar_t, boost::spirit::wistream_iterator)
CMN_INSTANTIATE_TRY_PARSE(combo, wchar_t, boost::spirit::wistream_iterator)
CMN_INSTANTIATE_TRY_PARSE(combo, char, std::wstring::iterator)

#undef CMN_INSTANTIATE_TRY_PARSE

///////////////////////////////////////////////////////////////////////////////

// parse kind_t::enum_
template
<
      typename Char
    , typename CharTraits
    , /*std::input_iterator*/ typename It
>
auto parse
(
      int_<kind_t::enum_>
    , qi::symbols<Char, std::ptrdiff_t> const &items
    , basic_qualified_name<Char, CharTraits> const &enum_name
    , basic_sink_format_options<Char, CharTraits> const &fmt_opt
    , It const &begin
    , It const &end
)
    -> std::ptrdiff_t
{
    return items_or_throw<Char, CharTraits>(try_parse_enum(items, enum_name, fmt_opt, begin, end));
}

// parse kind_t::bitfield
template
<
      typename Char
    , typename CharTraits
    , /*std::input_iterator*/ typename It
>
auto parse
(
      int_<kind_t::bitfield>
    , qi::symbols<Char, std::ptrdiff_t> const &items
    , basic_qualified_name<Char, CharTraits> const &enum_name
    , basic_sink_format_options<Char, CharTraits> const &fmt_opt
    , It const &begin
    , It const &end
)
    ->std::ptrdiff_t
{
    return items_or_throw<Char, CharTraits>(try_parse_bitfield(items, enum_name, fmt_opt, begin, end));
}

// parse kind_t::combo
template
<
      typename Char
    , typename CharTraits
    , /*std::input_iterator*/ typename It
>
auto parse
(
      int_<kind_t::combo>
    , qi::symbols<Char, std::ptrdiff_t> const& items
    , basic_qualified_name<Char, CharTraits> const &enum_name
    , basic_sink_format_options<Char, CharTraits> const &fmt_opt
    , It const &begin
    , It const &end
)
    ->std::ptrdiff_t
{
    return items_or_throw<Char, CharTraits>(try_parse_bitfield(items, enum_name, fmt_opt, begin, end));
}

//-----------------------------------------------------------------------------
#define CMN_INSTANTIATE_PARSE(kind_, char_, iterator_) \
    template auto parse    \
    (   \
          int_<BOOST_PP_CAT(kind_t::, kind_)>   \
        , boost::spirit::qi::symbols<char_, std::ptrdiff_t> const &  \
        , basic_qualified_name<char_> const &    \
        , basic_sink_format_options<char_> const & \
        , iterator_ const &   \
        , iterator_ const &  \
    )   \
    -> std::ptrdiff_t;

CMN_INSTANTIATE_PARSE(enum_, char, boost::spirit::istream_iterator)
CMN_INSTANTIATE_PARSE(bitfield, char, boost::spirit::istream_iterator)
CMN_INSTANTIATE_PARSE(combo, char, boost::spirit::istream_iterator)
CMN_INSTANTIATE_PARSE(combo, char, std::string::iterator)

CMN_INSTANTIATE_PARSE(enum_, wchar_t, boost::spirit::wistream_iterator)
CMN_INSTANTIATE_PARSE(bitfield, wchar_t, boost::spirit::wistream_iterator)
CMN_INSTANTIATE_PARSE(combo, wchar_t, boost::spirit::wistream_iterator)
CMN_INSTANTIATE_PARSE(combo, wchar_t, std::wstring::iterator)

#undef CMN_INSTANTIATE_PARSE

}
