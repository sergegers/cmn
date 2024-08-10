
#define BOOST_TEST_MODULE test
// ReSharper disable once CppUnusedIncludeDirective
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_monitor.hpp>

#include <cmn/error/exception.h>

namespace 
{

auto boost_exception_translator(boost::exception const &e) -> void
{
    BOOST_ERROR(cmn::error::get_error_description(e));
}

}

struct global_fixture
{
    global_fixture();
};

global_fixture::global_fixture()
{
    boost::unit_test::unit_test_monitor.register_exception_translator<
        boost::exception>(&boost_exception_translator);
}

BOOST_TEST_GLOBAL_FIXTURE(global_fixture);