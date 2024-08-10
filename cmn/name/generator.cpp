
// boost.spirit
#   include <boost/spirit/include/karma.hpp>

#include <cmn/error/exception.h>
#include <cmn/meta/symbols.h>

#include "generator.h"

namespace cmn::name_
{

namespace karma = boost::spirit::karma;

template <typename OutIterator>
struct namespace_gen : karma::grammar<OutIterator, namespace_t()>
{
    using symbols_type = symbols<char>;

    karma::rule<OutIterator, namespace_t()>         ns;

    namespace_gen() :
        namespace_gen::base_type{ ns }
    {

        using namespace karma;

        constexpr auto sr = symbols_type::scope_resolution.c_str();

        ns =    *(string << sr);

        //BOOST_SPIRIT_DEBUG_NODES
        //(
        //    (ns)
        //)
    }
};

template <typename OutIterator>
struct enum_name_gen : karma::grammar<OutIterator, enum_t()>
{
    using namespace_gen_type = namespace_gen<OutIterator>;

    karma::rule<OutIterator, enum_t()>              enum_;
    namespace_gen_type                              ns;

    enum_name_gen():
        enum_name_gen::base_type { enum_ }
    {

        using namespace karma;

        enum_          =   "enum " << - ns << string;

        //BOOST_SPIRIT_DEBUG_NODES
        //(
        //    (enum_)
        //)
    }
};

auto generate_enum_type_name(enum_t const & enum_type) -> std::string
{
    using iter = std::back_insert_iterator<std::string>;
    using gen = enum_name_gen<iter>;

    std::string res;

    if (!karma::generate(std::back_inserter(res), gen{}, enum_type))
        BOOST_THROW_EXCEPTION(cmn::format_error{});

    return res;
}

auto generate_type_name(type_t const &type) -> std::string
{
    throw not_implemented{};
}

}

