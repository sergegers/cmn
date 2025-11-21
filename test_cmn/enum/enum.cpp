
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
    using enum cl_cmb_t;

    static_assert
    (
        group_::make<zero, one, three, two>() ==
        group_info{ int_<zero>{}, int_<one>{}, int_<two>{}, int_<three>{} }
    );

    constexpr decltype(auto) enum_info_ = enum_info_v<cl_cmb_t>;
    static_assert(std::tuple_size_v<decltype(enum_info_.m_groups)> == 2, "enum group count mismatch");

    BOOST_TEST(boost::lexical_cast<std::string>(std::get<1>(enum_info_.m_groups).m_records[1]) == "green");

    static_assert(std::get<0>(enum_info_.m_groups).get_values() == std::array { zero, one, two, three });
    static_assert(std::get<1>(enum_info_.m_groups).get_values() == std::array { red, green, blue });

    constexpr auto masks = enum_info_.m_masks;
    static_assert(masks.size() == 2, "mask size mismatch");
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

BOOST_AUTO_TEST_CASE(nullable_bitfield)
{
    using enum cmn::io::iostate_t;

    static_assert(c::strong_bitfield<cmn::io::iostate_t>);
    static_assert(c::adapted_enum<cmn::io::iostate_t>);
    static_assert(nullable_v<cmn::io::iostate_t>);
    static_assert(unique_v<cmn::io::iostate_t>);

    constexpr decltype(auto) enum_info_ = enum_info_v<cmn::io::iostate_t>;
    static_assert(std::tuple_size_v<decltype(enum_info_.m_groups)> == 4, "enum group count mismatch");
    static_assert(std::get<0>(enum_info_.m_groups).get_values() == std::array{ goodbit });
    static_assert(std::get<1>(enum_info_.m_groups).get_values() == std::array{ eofbit });
    static_assert(std::get<2>(enum_info_.m_groups).get_values() == std::array{ failbit });
    static_assert(std::get<3>(enum_info_.m_groups).get_values() == std::array{ badbit });

    constexpr auto masks = enum_info_.m_masks;
    static_assert(masks.size() == 4, "mask size mismatch");
    static_assert(std::same_as<mask_type_t<cmn::io::iostate_t>, unsigned int>);
    static_assert(std::get<0>(masks) == static_cast<unsigned int>(goodbit), "goodbit mask mismatch");
    static_assert(std::get<1>(masks) == static_cast<unsigned int>(eofbit), "eofbit mask mismatch");
    static_assert(std::get<2>(masks) == static_cast<unsigned int>(failbit), "failbit mask mismatch");
    static_assert(std::get<3>(masks) == static_cast<unsigned int>(badbit), "badbit mask mismatch");

    auto ios{ goodbit };
    BOOST_TEST(boost::lexical_cast<std::string>(ios) == "[goodbit]");
    ios = set_features(ios, eofbit, failbit);
    BOOST_TEST(ios == (eofbit | failbit));
    BOOST_TEST(boost::lexical_cast<std::string>(ios) == "[eofbit failbit]");
}

BOOST_AUTO_TEST_SUITE_END() // enum_
BOOST_AUTO_TEST_SUITE_END() // cmn
