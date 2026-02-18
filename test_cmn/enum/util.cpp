
#include <vector>

#include <boost/test/unit_test.hpp>

#include <cmn/enum/traits.h>
#include <cmn/enum/io.h>

#include "defs.h"

BOOST_AUTO_TEST_SUITE(cmn)
BOOST_AUTO_TEST_SUITE(enum_)
BOOST_AUTO_TEST_SUITE(util)

namespace tt = boost::test_tools;
using cl_cmb_2_items_t = std::vector<cl_cmb_2_t>;

struct cl_cmb_2_processed
{
    cl_cmb_2_items_t &m_items;

    cl_cmb_2_processed(cl_cmb_2_items_t &items): m_items{ items }
    {
        m_items.clear();
    }

    constexpr auto operator ()(record_info<cl_cmb_2_t> const &rec, mask_type_t<cl_cmb_2_t> mask) const
    {
        m_items.push_back(rec.m_value);
    }
};

BOOST_AUTO_TEST_SUITE(group)

BOOST_AUTO_TEST_CASE(find_if_)
{
    using enum cl_cmb_2_t;

    constexpr auto in = two | green;

    constexpr auto digit_group = group_v<cl_cmb_2_t, 0>;
    cl_cmb_2_items_t items{};
    BOOST_TEST(green == group_::find_if(digit_group, in, cl_cmb_2_processed{ items }));
    BOOST_TEST(items == cl_cmb_2_items_t{ two }, tt::per_element());

    constexpr auto color_group = group_v<cl_cmb_2_t, 1>;
    BOOST_TEST(two == group_::find_if(color_group, in, cl_cmb_2_processed{ items }));
    BOOST_TEST(items == cl_cmb_2_items_t{ green }, tt::per_element());
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_CASE(fold_)
{
    using enum cl_cmb_2_t;

    constexpr auto in = two | green;

    cl_cmb_2_items_t items{};
    BOOST_TEST(0 == fold(groups_v<cl_cmb_2_t>, in, cl_cmb_2_processed{ items }));
    BOOST_TEST(items == (cl_cmb_2_items_t{ two, green }), tt::per_element());
}

BOOST_AUTO_TEST_SUITE_END() // io
BOOST_AUTO_TEST_SUITE_END() // enum_
BOOST_AUTO_TEST_SUITE_END() // cmn



