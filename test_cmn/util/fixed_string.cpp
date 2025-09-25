
#include <boost/test/unit_test.hpp>

#include <cmn/util/fixed_string.h>

BOOST_AUTO_TEST_SUITE(cmn)
BOOST_AUTO_TEST_SUITE(util)

BOOST_AUTO_TEST_CASE(fixed_string_)
{
    using namespace std::string_view_literals;

    static_assert("abc"_fs.size() == 3);
    static_assert("abc"sv.size() == 3);

    BOOST_TEST("abc"_fs == "abc"_fs);
    BOOST_TEST("abc"_fs == "abc"sv);

    char const abc_[] = { 'a', 'b', 'c', '\0' };
    fixed_string abc{ abc_ };
    static_assert(abc.size() == 4);

    BOOST_TEST(abc.size() == 4);
    BOOST_TEST("abc"_fs.size() == 3);
    BOOST_TEST(abc != "abc"_fs);

    BOOST_TEST(abc.compare_as_cstr("abc"_fs) == 0);

    empty_fixed_string zero{};
    static_assert(zero.size() == 0);
    BOOST_TEST(zero.size() == 0);
}

BOOST_AUTO_TEST_SUITE_END() // util
BOOST_AUTO_TEST_SUITE_END() // cmn
