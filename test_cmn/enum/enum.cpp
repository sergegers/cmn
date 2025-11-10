
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

#include <cmn/enum/op.h>
#include <cmn/enum/io.h>

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
    using enum cl_cmb_t;

    static_assert
    (
        group_::make<zero, one, three, two>() ==
        group_info{ int_<zero>{}, int_<one>{}, int_<two>{}, int_<three>{} }
    );

    constexpr decltype(auto) enum_info_ = enum_info_v<cl_cmb_t>;
    static_assert(std::tuple_size_v<decltype(enum_info_.m_groups)> == 2, "Enum group count mismatch");

    BOOST_TEST(boost::lexical_cast<std::string>(std::get<1>(enum_info_.m_groups).m_records[1]) == "green");

    static_assert(std::get<0>(enum_info_.m_groups).get_values() == std::array { zero, one, two, three });
    static_assert(std::get<1>(enum_info_.m_groups).get_values() == std::array { red, green, blue });

    constexpr auto masks = enum_info_.m_masks;
    static_assert(masks.size() == 2, "size mismatch");
    static_assert(std::get<0>(masks) == digit_mask, "digit mask mismatch");
    static_assert(std::get<1>(masks) == color_mask, "color mask mismatch");
}

BOOST_AUTO_TEST_CASE(enum_class_ops)
{
    using enum cl_cmb_t;

    cl_cmb_t e{ two };
    e |= blue;
    BOOST_TEST(e == (cl_cmb_t::two | cl_cmb_t::blue));

    e &= digit_mask;
    BOOST_TEST(e == two);
}

BOOST_AUTO_TEST_SUITE_END() // enum_
BOOST_AUTO_TEST_SUITE_END() // cmn
