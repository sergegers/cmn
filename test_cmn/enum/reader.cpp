
#include <string>

#include <boost/test/unit_test.hpp>

#include <cmn/enum/traits.h>
#include <cmn/enum/io/reader.h>

#include "defs.h"

BOOST_AUTO_TEST_SUITE(cmn)
BOOST_AUTO_TEST_SUITE(enum_)
BOOST_AUTO_TEST_SUITE(io)
BOOST_AUTO_TEST_SUITE(reader_)

using namespace std::literals;

BOOST_AUTO_TEST_CASE(try_parse_combo)
{
    using enum cl_cmb_2_t;
    using enum print_t;

    constexpr auto kkind = int_<kind_v<cl_cmb_2_t>>{};
    auto const name = name_v<cl_cmb_2_t>;
    auto const items = detail::prepare_enum_items<cl_cmb_2_t, char, std::char_traits<char>>(kkind);

    {
        sink_format_options fmt_opt{ .options = empty, .open = "", .close = "", .delimiter = "" };
        std::string str{"two"};
        BOOST_TEST(parse(kkind, items, name, fmt_opt, str.begin(), str.end()) == static_cast<std::ptrdiff_t>(two));
    }

    {
        sink_format_options fmt_opt{ .options = delimiter, .open = "", .close = "", .delimiter = ", " };
        std::string str{"two"};
        BOOST_TEST(parse(kkind, items, name, fmt_opt, str.begin(), str.end()) == static_cast<std::ptrdiff_t>(two));
    }
}

BOOST_AUTO_TEST_SUITE_END() // reader_
BOOST_AUTO_TEST_SUITE_END() // io
BOOST_AUTO_TEST_SUITE_END() // enum_
BOOST_AUTO_TEST_SUITE_END() // cmn
