
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>

#include <cmn/name/parser.h>
#include <cmn/name/generator.h>
#include <cmn/tuple/ops.h>
#include <cmn/tuple/io.h>
#include <cmn/range/io.h>

namespace boost { using cmn::range::io::operator <<; }

#include <boost/optional/optional_io.hpp>

BOOST_AUTO_TEST_SUITE(cmn)
BOOST_AUTO_TEST_SUITE(name_)

using namespace std::string_literals;

using boost::fusion::operators::operator ==;

enum en {};

namespace 
{

enum aen {};

std::vector enum_tnames
{
    boost::typeindex::type_id<en>().pretty_name()
    , boost::typeindex::type_id<aen>().pretty_name()
    , "enum en"s
};

std::vector<enum_t> enum_types
{
      { { { "cmn"s, "name_"s } }, "en"s }
    , { { { "cmn"s, "name_"s, "`anonymous namespace'"s } }, "aen"s }
    , { {}, "en"s }
};

}

BOOST_DATA_TEST_CASE(parse_enum, enum_tnames ^ enum_types, en_tname, en_type)
{
    auto const parsed_en_type = parse_enum_type(en_tname);
    BOOST_TEST(en_type == parsed_en_type);
}

BOOST_DATA_TEST_CASE(gen_enum, enum_types ^ enum_tnames, en_type, en_tname)
{
    auto const gend_en_tname = generate_enum_type_name(en_type);
    BOOST_TEST(en_tname == gend_en_tname);
}

BOOST_AUTO_TEST_SUITE_END()	// name_
BOOST_AUTO_TEST_SUITE_END()	// cmn
