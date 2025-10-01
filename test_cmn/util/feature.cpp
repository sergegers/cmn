
#include <boost/test/unit_test.hpp>

#include <cmn/meta/concepts.h>

#include <cmn/util/feature.h>

#include <cmn/enum/bitfield.h>
#include <cmn/enum/io.h>

BOOST_AUTO_TEST_SUITE(cmn)
BOOST_AUTO_TEST_SUITE(util)

enum class strong_feature_t
{
    f1 = 0x1,
    f2  = 0x2,
    f4  = 0x4,
    f8  = 0x8,
    m3  = f1 | f2,
    mB  = f4 | f8
};

consteval auto adapt_enum_info(strong_feature_t)
{
    using namespace enum_;
    using enum strong_feature_t;

    return adapt_bitfield_info_helper<f1, f2, f4, f8>();
}

CMN_ENUM_INJECT_OPS()

static_assert(cmn::c::strong_bitfield<strong_feature_t>);

using enum strong_feature_t;

auto const sbf_pol = f1 | f4;

static_assert(get_feature(sbf_pol, m3) == f1);
static_assert(get_feature(sbf_pol, mB) == f4);

static_assert(set_feature(sbf_pol, f2, m3) == (f2 | f4));
static_assert(set_feature(sbf_pol, f8, mB) == (f1 | f8));

static_assert(has_feature(sbf_pol, f1));
static_assert(has_feature(sbf_pol, f4));
static_assert(has_all_features(sbf_pol, f1, f4));

//-----------------------------------------------------------------------------

enum feature_t
{
    f_1 = 0x1,
    f_2  = 0x2,
    f_4  = 0x4,
    f_8  = 0x8,
    f_m3  = f_1 | f_2,
    f_mB  = f_4 | f_8
};

auto const bf_pol = f_1 | f_4;

static_assert(get_feature(bf_pol, f_m3) == f_1);
static_assert(get_feature(bf_pol, f_mB) == f_4);

static_assert(set_feature(bf_pol, f_2, f_m3) == (f_2 | f_4));
static_assert(set_feature(bf_pol, f_8, f_mB) == (f_1 | f_8));

static_assert(has_feature(bf_pol, f_1));
static_assert(has_feature(bf_pol, f_4));

//-----------------------------------------------------------------------------


BOOST_AUTO_TEST_SUITE_END() // util
BOOST_AUTO_TEST_SUITE_END() // cmn
