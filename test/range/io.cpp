
#include <vector>
#include <ranges>

#include <boost/test/unit_test.hpp>
#include <boost/test/tools/output_test_stream.hpp>

#include <cmn/range/io.h>
#include <cmn/range/io/formatter.h>

BOOST_AUTO_TEST_SUITE(cmn)
BOOST_AUTO_TEST_SUITE(range_)
BOOST_AUTO_TEST_SUITE(io)

BOOST_AUTO_TEST_CASE(std_cnt)
{
    std::vector const v { 1, 2, 3 };
    boost::test_tools::output_test_stream ostr {};
    ostr << v;
    BOOST_CHECK(ostr.is_equal("<1, 2, 3>"));
}

BOOST_AUTO_TEST_CASE(range)
{
    boost::test_tools::output_test_stream ostr {};
    ostr << std::ranges::views::iota(1, 4);
    BOOST_CHECK(ostr.is_equal("<1, 2, 3>"));
}

BOOST_AUTO_TEST_SUITE_END() // io
BOOST_AUTO_TEST_SUITE_END() // range_
BOOST_AUTO_TEST_SUITE_END() // cmn
