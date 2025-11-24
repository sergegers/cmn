
#include <concepts>
#include <array>
#include <tuple>
#include <format>

#include <boost/lexical_cast.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/tools/output_test_stream.hpp>

#include <cmn/meta/concepts.h>
#include <cmn/error/exception.h>
#include <cmn/util/lexical_cast.h>
#include <cmn/io/debug.h>

#include <cmn/enum/op.h>
#include <cmn/enum/io.h>
#include <cmn/enum/traits.h>
#include <cmn/enum/feature.h>

#include "defs.h"

BOOST_AUTO_TEST_SUITE(cmn)
BOOST_AUTO_TEST_SUITE(enum_)

static_assert(next_step(0b0111) == 0b1000);
static_assert(next_step(0b0111) != 0b1001);
static_assert(next_step(0b0100000100) == 0b1000000000);

//-----------------------------------------------------------------------------
using boost::test_tools::output_test_stream;
using boost::test_tools::per_element;

using namespace io;

BOOST_AUTO_TEST_CASE(masks)
{
    using enum zero_cl_cmb_t;

    static_assert
    (
        group_::make<zero, one, three, two>() ==
        group_info{ int_<zero>{}, int_<one>{}, int_<two>{}, int_<three>{} }
    );
    static_assert(c::adapted_enum<zero_cl_cmb_t>);
    static_assert(std::tuple_size_v<decltype(groups_v<zero_cl_cmb_t>)> == 2, "enum group count mismatch");

    BOOST_TEST(boost::lexical_cast<std::string>(record_v<zero_cl_cmb_t, 1, 1>.m_value) == "green");

    static_assert(group_v<zero_cl_cmb_t, 0>.get_values() == std::array { zero, one, two, three });
    static_assert(group_v<zero_cl_cmb_t, 1>.get_values() == std::array{ red, green, blue });

    constexpr auto &masks = masks_v<zero_cl_cmb_t>;
    static_assert(masks.size() == 2, "mask size mismatch");
    static_assert(masks[0] == digit_mask, "digit mask mismatch");
    static_assert(masks[1] == color_mask, "color mask mismatch");
}

BOOST_AUTO_TEST_CASE(enum_class_ops)
{
    using enum zero_cl_cmb_t;

    zero_cl_cmb_t e{ two };
    e |= blue;
    BOOST_TEST(e == (zero_cl_cmb_t::two | zero_cl_cmb_t::blue));

    e &= digit_mask;
    BOOST_TEST(e == two);
}

BOOST_AUTO_TEST_CASE(nullable_bitfield)
{
    using iostate_t = cmn::io::iostate_t;
    using enum iostate_t;

    static_assert(c::strong_bitfield<cmn::io::iostate_t>);
    static_assert(c::adapted_enum<cmn::io::iostate_t>);
    static_assert(nullable_v<cmn::io::iostate_t>);
    static_assert(unique_v<cmn::io::iostate_t>);

    static_assert(std::tuple_size_v<decltype(groups_v<iostate_t>)> == 4, "enum group count mismatch");
    static_assert(record_v<iostate_t, 0, 0>.m_value == goodbit);
    static_assert(record_v<iostate_t, 1, 0>.m_value == eofbit);
    static_assert(record_v<iostate_t, 2, 0>.m_value == failbit);
    static_assert(record_v<iostate_t, 3, 0>.m_value == badbit);

    constexpr auto &masks = masks_v<iostate_t>;
    static_assert(masks.size() == 4, "mask size mismatch");
    static_assert(std::same_as<mask_type_t<cmn::io::iostate_t>, unsigned int>);
    static_assert(masks[0] == static_cast<unsigned int>(goodbit), "goodbit mask mismatch");
    static_assert(masks[1] == static_cast<unsigned int>(eofbit), "eofbit mask mismatch");
    static_assert(masks[2] == static_cast<unsigned int>(failbit), "failbit mask mismatch");
    static_assert(masks[3] == static_cast<unsigned int>(badbit), "badbit mask mismatch");

    auto ios{ goodbit };
    BOOST_TEST(boost::lexical_cast<std::string>(ios) == "[goodbit]");
    ios = set_features(ios, eofbit, failbit);
    BOOST_TEST(ios == (eofbit | failbit));
    BOOST_TEST(boost::lexical_cast<std::string>(ios) == "[eofbit failbit]");
}

BOOST_AUTO_TEST_SUITE_END() // enum_
BOOST_AUTO_TEST_SUITE_END() // cmn
