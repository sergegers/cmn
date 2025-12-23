
#include <tuple>
#include <type_traits>
#include <format>

#include <boost/test/unit_test.hpp>
#include <boost/test/tools/output_test_stream.hpp>

#include <boost/fusion/adapted/std_tuple.hpp>

#include <cmn/tuple/io.h>

BOOST_AUTO_TEST_SUITE(cmn)
BOOST_AUTO_TEST_SUITE(tuple_)
BOOST_AUTO_TEST_SUITE(io)

BOOST_AUTO_TEST_CASE(std_cnt)
{
    std::tuple const v { 1, 2, 3 };
    static_assert(boost::c::fus_sequence<std::remove_cvref_t<decltype(v)>>);

    boost::test_tools::output_test_stream ostr {};
    ostr << v;
    BOOST_CHECK(ostr.is_equal("(1, 2, 3)"));
}

BOOST_AUTO_TEST_CASE(format)
{
    using cmn::io::list;

    BOOST_TEST(std::format("{}", std::tuple{ 1, 'a', 2.f }) == "(1, 'a', 2)");
    BOOST_TEST(std::format("{:n}", std::tuple{ 1, 'a', 2.f }) == "1, 'a', 2");

    // TODO: enable
    //BOOST_TEST(std::format("{:>>> : , : <<<}", list{ std::tuple{ 1, 'a', 2.f } }) == ">>> 1 , 'a' , 2 <<<");
    //BOOST_TEST(std::format("{:::}", list{ std::tuple{ 1, 'a', 2.f } }) == "1'a'2");
    //BOOST_TEST(std::format("{:{{:, :}}}", list{ std::tuple{ 1, 'a', 2.f } }) == "{1, 'a', 2}");
}

BOOST_AUTO_TEST_SUITE_END() // io
BOOST_AUTO_TEST_SUITE_END() // tuple_
BOOST_AUTO_TEST_SUITE_END() // cmn
