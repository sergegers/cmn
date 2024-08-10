

// boost.spirit
#   include <boost/spirit/include/qi.hpp>

#include <cmn/meta/symbols.h>
#include <cmn/error/exception.h>

#include "parser_symbols.h"
#include "parser.h"

namespace cmn::name_
{

namespace qi = boost::spirit::qi;

template
<
      typename Iterator
    , typename Skipper      = qi::space_type
>
struct id_parser : qi::grammar<Iterator, Skipper, std::string()>
{
    qi::rule<Iterator, Skipper, std::string()>              id;

    id_parser()
        : id_parser::base_type { id }
    {
        using namespace qi;

        auto const& keywords = psym::keywords();

        id = ((alpha | char_('_')) >> *(alnum | char_('_'))) - keywords;

        //BOOST_SPIRIT_DEBUG_NODES
        //(
        //    (id)
        //)
    }
};

template
<
      typename Iterator
    , typename Skipper   
>
struct namespace_parser: qi::grammar<Iterator, Skipper, namespace_t()>
{
    using id_parser_type = id_parser<Iterator, Skipper>;
    using symbols_type = symbols<char>;

    id_parser_type                                              id;
    qi::rule<Iterator, Skipper, std::string()>                  fragment;
    qi::rule<Iterator, Skipper, namespace_t()>                  ns;

    namespace_parser()
        : namespace_parser::base_type { ns }
    {
        using namespace qi;

        constexpr auto sr = symbols_type::scope_resolution.c_str();

        fragment = id | string("`anonymous namespace'");
        ns          = -lit(sr) >> +(fragment >> sr);

        //BOOST_SPIRIT_DEBUG_NODES
        //(
        //    (ns)
        //)
    }
};

template
<
      typename Iterator
    , typename Skipper      = qi::space_type
>
struct enum_parser: qi::grammar<Iterator, Skipper, enum_t()>
{
    using namespace_rule_type = namespace_parser<Iterator, Skipper>;
    using id_parser_type = id_parser<Iterator, Skipper>;

    namespace_rule_type                                     ns;
    id_parser_type                                          name;
    qi::rule<Iterator, Skipper, enum_t()>                   enum_;

    enum_parser()
        : enum_parser::base_type { enum_ }
    {
        using namespace qi;

        enum_  = "enum " >>  -ns >> name;

        //BOOST_SPIRIT_DEBUG_NODES
        //(
        //    (enum_)
        //)
    }
};

template
<
      typename Iterator
    , typename Skipper      = qi::space_type
>
struct class_parser: qi::grammar<Iterator, Skipper, class_t()>
{
    using namespace_rule_type = namespace_parser<Iterator, Skipper>;
    using id_parser_type = id_parser<Iterator, Skipper>;

    namespace_rule_type                                     ns;
    id_parser_type                                          name;
    qi::rule<Iterator, Skipper, class_t()>                  class_;

    class_parser()
        : class_parser::base_type { class_ }
    {
        using namespace qi;

        auto const &cls_kwd = psym::class_kwds();

        class_   =  cls_kwd >> " " >>  -ns >> name;

        //BOOST_SPIRIT_DEBUG_NODES
        //(
        //    (class_)
        //)
    }
};

///////////////////////////////////////////////////////////////////////////////
auto parse_enum_type(std::string_view enum_tname) -> enum_t
{
    using parser = enum_parser<std::string_view::const_iterator>;

    auto begin = enum_tname.cbegin();
    auto const end = enum_tname.cend();
    enum_t res;

    if (!qi::phrase_parse(begin, end, parser{}, qi::space, qi::skip_flag::postskip, res))
         throw format_error("Couldn't parse [%1%]", enum_tname);

    if (begin != end)
        throw format_error
        (
              "[%1%] is partially parsed, [%2%] is remained"
            , enum_tname
            , std::string_view{ &*begin, static_cast<std::size_t>(std::distance(begin, end)) }
        );

    return res;
}

auto parse_class_type(std::string_view class_tname) -> class_t
{
    using parser = class_parser<std::string_view::const_iterator>;

    auto begin = class_tname.cbegin();
    auto const end = class_tname.cend();
    class_t res;

    if (!qi::phrase_parse(begin, end, parser{}, qi::space, qi::skip_flag::postskip, res))
        throw format_error("Couldn't parse [%1%]", class_tname);

    if (begin != end)
        throw format_error
        (
              "[%1%] is partially parsed, [%2%] is remained"
            , class_tname
            , std::string_view{ &*begin, static_cast<std::size_t>(std::distance(begin, end)) }
        );

    return res;
}

auto parse_type(std::string_view const &tname) -> type_t
{
    throw not_implemented{};
}

}
