
#include <boost/test/unit_test.hpp>

#include <cmn/error/exception.h>

BOOST_AUTO_TEST_SUITE(cmn)
BOOST_AUTO_TEST_SUITE(error)

BOOST_AUTO_TEST_CASE(ctor)
{
    {
        io_error const err{};
        BOOST_TEST(err.what() == "Unknown exception");
    }

    {
        io_error const err{ "error message"};
        BOOST_TEST(err.what() == "error message");
    }

    {
        io_error const err{ "error message with args: [{0} {1}]", 0, "and one"};
        BOOST_TEST(err.what() == "error message with args: [0 and one]");
    }
}

BOOST_AUTO_TEST_SUITE_END() // error
BOOST_AUTO_TEST_SUITE_END() // cmn

