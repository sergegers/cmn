
#include <string>

#include <boost/test/unit_test.hpp>

#include <cmn/meta/traits/type.h>
#include <cmn/enum/traits.h>
#include <cmn/enum/io/printer.h>

#include "defs.h"

BOOST_AUTO_TEST_SUITE(cmn)
BOOST_AUTO_TEST_SUITE(enum_)
BOOST_AUTO_TEST_SUITE(io)
BOOST_AUTO_TEST_SUITE(printer_)

BOOST_AUTO_TEST_CASE(out_record_)
{
    using enum cl_cmb_2_t;
    using enum print_t;

    sink_format_options fmt_opt
    {
        .options = empty
    };
    record_info rec { int_<two>{} };
    std::string buffer;
    bool first = true;

    detail::out_record(rec, no_mask<cl_cmb_2_t>, fmt_opt, first, std::back_inserter(buffer));
    BOOST_TEST(buffer == "two");

    first = true;
    buffer.clear();
    fmt_opt.options = class_prefix;
    detail::out_record(rec, no_mask<cl_cmb_2_t>, fmt_opt, first, std::back_inserter(buffer));
    BOOST_TEST(buffer == "cl_cmb_2_t::two");

    first = true;
    buffer.clear();
    fmt_opt.options = class_prefix | ns;
    detail::out_record(rec, no_mask<cl_cmb_2_t>, fmt_opt, first, std::back_inserter(buffer));
    BOOST_TEST(buffer == "cmn::enum_::cl_cmb_2_t::two");
}

BOOST_AUTO_TEST_SUITE_END() // printer_
BOOST_AUTO_TEST_SUITE_END() // io
BOOST_AUTO_TEST_SUITE_END() // enum_
BOOST_AUTO_TEST_SUITE_END() // cmn
