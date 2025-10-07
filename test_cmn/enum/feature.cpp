
#include <boost/test/unit_test.hpp>

#include <cmn/strong_typedef/io/int_fmt.h>
#include <cmn/enum/feature.h>
#include <cmn/enum/io.h>

BOOST_AUTO_TEST_SUITE(cmn)
BOOST_AUTO_TEST_SUITE(enum_)

BOOST_AUTO_TEST_CASE(int_fmt)
{
    using enum cmn::io::int_fmt_t;

    auto const fmt_opt = hex | showbase | c | short_ | lowercase | nosign;

    BOOST_TEST(set_feature(fmt_opt, dec) == (dec | showbase | c | short_ | lowercase | nosign));
    BOOST_TEST(has_all_features(fmt_opt, hex, showbase, c, short_, lowercase, nosign));

    auto const new_fmt_opt = set_features(fmt_opt,dec, asm_, short_, forcesign);

    cmn::io::int_fmt_wrapper_t x{ new_fmt_opt };
    BOOST_TEST
    (
        new_fmt_opt ==
        (dec | showbase | asm_ | short_ | lowercase | forcesign)
    );

}

BOOST_AUTO_TEST_SUITE_END() // enum_
BOOST_AUTO_TEST_SUITE_END() // cmn
