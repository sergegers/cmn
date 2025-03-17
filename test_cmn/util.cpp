
#include <string_view>

#include <boost/test/unit_test.hpp>

#include <cmn/meta/concepts.h>
#include <cmn/util/util.h>
#include <cmn/util/fixed_string.h>

BOOST_AUTO_TEST_SUITE(cmn)
BOOST_AUTO_TEST_SUITE(util)

using namespace literals;

static_assert(log10(1ul) == 1, "log10(1)");
static_assert(pow10(0ul) == 1, "pow10(0)");

static_assert("1"_fs == itoa<1>(), "1");
static_assert("123456"_fs == itoa<123456>(), "123456");
static_assert("-123456"_fs == itoa< -123456>(), "-123456");

static_assert(3 == bsf(0b1000u), "1000B");
static_assert(4 == bsf(0b1001'0000u), "10010000B");
static_assert(5 == bsf(0b10'0000u), "100000B");

static_assert(3 == bsr(0b1010u));
static_assert(7 == bsr(0b1001'0000u));
static_assert(5 == bsr(0b10'0000u));

static_assert
(
    std::is_same_v
    <
        index_swap_t<std::index_sequence<1, 2, 3, 4, 5>, 2, 4>,
        std::index_sequence<1, 4, 3, 2, 5>
    >,
    "swap_index_seq_t failed" 
);

static_assert(c::string<fixed_string<10>>);

static_assert(std::same_as<copy_const_t<int, char>, char>);
static_assert(std::same_as<copy_const_t<int const, char>, char const>);
static_assert(std::same_as<copy_const_t<int, char const>, char>);
static_assert(std::same_as<copy_const_t<int const, char const>, char const>);

static_assert(std::same_as<copy_reference_t<int, char>, char>);
static_assert(std::same_as<copy_reference_t<int &, char>, char &>);
static_assert(std::same_as<copy_reference_t<int &, char const>, char const &>);
static_assert(std::same_as<copy_reference_t<int &, char &>, char &>);
static_assert(std::same_as<copy_reference_t<int const &, char const>, char const &>);
static_assert(std::same_as<copy_reference_t<int &&, char>, char &&>);
static_assert(std::same_as<copy_reference_t<int, char &&>, char>);

static_assert(std::same_as<copy_cvr_t<int &, char>, char &>);
static_assert(std::same_as<copy_cvr_t<int const &, char const>, char const &>);

BOOST_AUTO_TEST_CASE(fixed_string_)
{
    using namespace std::string_view_literals;

    BOOST_TEST("abc"_fs == "abc"_fs);
    BOOST_TEST("abc"_fs == "abc"sv);

    char const abc_[] = { 'a', 'b', 'c' };
    fixed_string abc{ abc_ };

    BOOST_TEST(abc.size() == 3);
    BOOST_TEST("abc"_fs.size() == 4);

    BOOST_TEST(abc != "abc"_fs);

    BOOST_TEST(abc.compare_as_cstr("abc"_fs) == 0);
}

BOOST_AUTO_TEST_SUITE_END() // util
BOOST_AUTO_TEST_SUITE_END() // cmn
