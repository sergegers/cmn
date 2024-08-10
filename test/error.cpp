
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

//BOOST_AUTO_TEST_CASE(get_error_description_)
//{
//    try
//    {
//        BOOST_THROW_EXCEPTION
//        ((
//            io_error{ "Text is not parsed" }
//            << error::errinfo_msg
//              {
//                {
//                        .m_msg = "My text",
//                        .m_open = 5
//                    }
//                }
//        ));
//    }
//    catch (boost::exception const &ex)
//    {
//        BOOST_TEST(get_error_description(ex) == "Text is not parsed");
//    }
//}

BOOST_AUTO_TEST_SUITE_END() // error
BOOST_AUTO_TEST_SUITE_END() // cmn

