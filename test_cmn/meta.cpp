
#include <boost/test/unit_test.hpp>

#include <type_traits>
#include <string>
#include <string_view>

#include <boost/fusion/container/map.hpp>

#if __has_include(<boost/fusion/container/imap.hpp>)
#   include <boost/fusion/container/imap.hpp>
#else
#   include <cmn/meta/boost/fusion/container/imap.hpp>
#endif

#if __has_include(<boost/fusion/container/generation/make_imap.hpp>)
#   include <cboost/fusion/container/generation/make_imap.hpp>
#else
#   include <cmn/meta/boost/fusion/container/generation/make_imap.hpp>
#endif

#include <cmn/meta/concepts.h>
#include <cmn/meta/traits.h>

#include <cmn/util/fixed_string.h>
#include <cmn/enum/io/format_traits.h>


BOOST_AUTO_TEST_SUITE(cmn)

namespace utf = boost::unit_test;

#ifdef CMN_STATIC_TEST

// TODO: enable

// type_traits
static_assert(std::is_same_v<copy_const_t<int, char>, char>, "copy_const failed 0");
static_assert(std::is_same_v<copy_const_t<int const, char>, char const>, "copy_const failed 1");
static_assert(!std::is_same_v<copy_const_t<int const, int &>, int const &>, "copy_const failed 2");
static_assert(!std::is_same_v<copy_const_t<int const, int *>, int const *>, "copy_const failed 3");
static_assert(std::is_same_v<copy_const_t<int const, int *>, int *const>, "copy_const failed 4");
static_assert(std::is_same_v<copy_const_t<int const, char const &>, char const &>, "copy_const failed 5");
static_assert(std::is_same_v<copy_const_t<int const, char const *>, char const *const>, "copy_const failed 6");

static_assert(std::is_same_v<deep_copy_const_t<int, char>, char>, "copy_const failed 0");
static_assert(std::is_same_v<deep_copy_const_t<int const, char>, char const>, "copy_const failed 1");
static_assert(std::is_same_v<deep_copy_const_t<int const, long>, long const>, "copy_const failed 2");
static_assert(std::is_same_v<deep_copy_const_t<int const, int &>, int const &>, "copy_const failed 3");
// static_assert(std::is_same_v<deep_copy_const_t<int const, int *>, int const * const>, "copy_const failed 4");
// static_assert(!std::is_same_v<deep_copy_const_t<int const, int *>, int * const>, "copy_const failed 6");
// static_assert(std::is_same_v<deep_copy_const_t<int const, char const &>, char const &>, "copy_const failed 7");
// static_assert(std::is_same_v<deep_copy_const_t<int const, char const *>, char const * const>, "copy_const failed 8");
//
// static_assert(std::is_same_v<add_volatile_on_t<int, char>, char>, "add_volatile_on failed 0");
// static_assert(std::is_same_v<add_volatile_on_t<int volatile, char>, char volatile>, "add_volatile_on failed 1");
//
static_assert(std::is_same_v<copy_rvalue_reference_t<int, char>, char>, "copy_rvalue_reference failed 0");
static_assert(std::is_same_v<copy_rvalue_reference_t<int &&, char>, char &&>, "copy_rvalue_reference failed 1");

static_assert(std::is_same_v<copy_lvalue_reference_t<int, char>, char>, "copy_lvalue_reference failed 0");
static_assert(std::is_same_v<copy_lvalue_reference_t<int &, char>, char &>, "copy_lvalue_reference failed 1");
static_assert(std::is_same_v<copy_lvalue_reference_t<int &, char const>, char const &>,
              "copy_lvalue_reference failed 2");

// static_assert(std::is_same_v<add_cvref_on_t<int, float>, float>, "add_cvr_on_t 0 failed");
// static_assert(std::is_same_v<add_cvref_on_t<int const, float>, float const>, "add_cvr_on_t 1 failed");
// static_assert(std::is_same_v<add_cvref_on_t<int volatile, float>, float volatile>, "add_cvr_on_t 2 failed");
// static_assert(std::is_same_v<add_cvref_on_t<int &, float>, float &>, "add_cvr_on_t 3 failed");
// static_assert(std::is_same_v<add_cvref_on_t<int &&, float>, float &&>, "add_cvr_on_t 4 failed");
// static_assert(std::is_same_v<add_cvref_on_t<int const &, float>, float const &>, "add_cvr_on_t 5 failed");
// static_assert(std::is_same_v<add_cvref_on_t<int volatile &&, float>, float volatile &&>, "add_cvr_on_t 6 failed");
// static_assert(std::is_same_v<add_cvref_on_t<int const volatile &, float>, float const volatile &>, "add_cvr_on_t 7
// failed"); static_assert(std::is_same_v<add_cvref_on_t<int volatile &&, float>, float volatile &&>, "add_cvr_on_t 8
// failed");
//
static_assert(std::is_same_v<remove_rvalue_reference_t<int &&>, int>, "remove_rvalue_reference_t 0 failed");
static_assert(std::is_same_v<remove_rvalue_reference_t<int &>, int &>, "remove_rvalue_reference_t 1 failed");
static_assert(std::is_same_v<remove_rvalue_reference_t<int const &&>, int const>, "remove_rvalue_reference_t 2 failed");
static_assert(std::is_same_v<remove_rvalue_reference_t<int const &>, int const &>,
              "remove_rvalue_reference_t 3 failed");

static_assert(std::is_same_v<remove_lvalue_reference_t<int &&>, int &&>, "remove_lvalue_reference_t 0 failed");
static_assert(std::is_same_v<remove_lvalue_reference_t<int &>, int>, "remove_rvalue_reference_t 1 failed");
static_assert(std::is_same_v<remove_lvalue_reference_t<int const &&>, int const &&>,
              "remove_lvalue_reference_t 2 failed");
static_assert(std::is_same_v<remove_lvalue_reference_t<int const &>, int const>, "remove_lvalue_reference_t 3 failed");

// static_assert(std::is_same_v<function::arg_type_c_t<void(int, char *, std::string const &), 0>, int>,
//     "arg_type_c - argument 0 failed");
// static_assert(std::is_same_v<function::arg_type_c_t<int(int, char *, std::string const &), 1>, char *>,
//     "arg_type_c - argument 1 failed");
// static_assert(std::is_same_v<function::arg_type_c_t<void(int, char *, std::string const &), 2>, std::string const &>,
//     "arg_type_c - argument 2 failed");
//
//// NOTE: migrate can_call -> std::is_invocable
// static_assert(std::is_invocable_v<decltype(::_itoa), int, char *, int>, "can_call -> std::is_invocable ->
// std::is_invocable 0 failed"); static_assert(!std::is_invocable_v<decltype(::_itoa), int, char *>, "can_call ->
// std::is_invocable 1 failed"); static_assert(std::is_invocable_v<decltype(::_itoa), int, char *, long>, "can_call ->
// std::is_invocable 2 failed");
//
// int g_i[2] { 1, 3 };
//
// int fn0(int const &, int const &, int const &);
// int fn1();
//
// static_assert(std::is_invocable_v<decltype(fn0), int, int, int>, "can_call -> std::is_invocable 3 failed");
// static_assert(!std::is_invocable_v<decltype(fn0)>, "can_call -> std::is_invocable 4 failed");
// static_assert(std::is_invocable_v<decltype(fn1)>, "can_call -> std::is_invocable 5 failed");
//
// struct ss
//{
//     static int fn0(int, char *, float) noexcept { return 0; }
//     static int fn1(int &, char *, float) noexcept { return 0; }
//     static auto fn2(int const &, char *, double) noexcept { return std::pair{ 1, "ab" }; }
// };
//
// static_assert(std::is_invocable_v<decltype(ss::fn0), int, char *, float>, "can_call -> std::is_invocable 6 failed");
// static_assert(!std::is_invocable_v<decltype(ss::fn0), char *, float, float>, "can_call -> std::is_invocable 7
// failed"); static_assert(std::is_invocable_v<decltype(ss::fn1), int &, char *, float>, "can_call -> std::is_invocable
// 8 failed"); static_assert(!std::is_invocable_v<decltype(ss::fn1), int, char *, float>, "can_call -> std::is_invocable
// 9 failed"); static_assert(!std::is_invocable_v<decltype(ss::fn1), char *, char *, float>, "can_call ->
// std::is_invocable 10 failed"); static_assert(std::is_invocable_v<decltype(ss::fn2), int &, char *, double>, "can_call
// -> std::is_invocable 11 failed"); static_assert(!std::is_invocable_v<decltype(ss::fn2), int, float, int(&)[2]>,
// "can_call -> std::is_invocable 12 failed");
//
// static_assert(carray_size_v<int[6]> == 6, "array_size failed 0");
// static_assert(carray_size_v<int[]> == 0, "array_size failed 1");
// static_assert(carray_size_v<int> == std::numeric_limits<std::size_t>::max(), "array_size failed 2");
// static_assert(carray_size_v<char> == std::numeric_limits<std::size_t>::max(), "array_size failed 3");
//
static_assert(c::dereferenceable<int *>, "dereferenceable test 0 failed");
static_assert(c::dereferenceable<int const *>, "dereferenceable test 1 failed");
static_assert(c::dereferenceable<struct test_ *>, "dereferenceable test 2 failed");
static_assert(c::dereferenceable<std::shared_ptr<int>>, "dereferenceable test 3 failed");
static_assert(c::dereferenceable<std::shared_ptr<int const>>, "dereferenceable test 4 failed");
static_assert(c::dereferenceable<std::unique_ptr<float>>, "dereferenceable test 5 failed");
static_assert(c::dereferenceable<std::vector<long>::iterator>, "dereferenceable test 6 failed");
static_assert(c::dereferenceable<std::vector<int>::const_iterator>, "dereferenceable test 7 failed");
static_assert(!c::dereferenceable<int>, "dereferenceable test 8 failed");
static_assert(!c::dereferenceable<std::vector<int>>, "dereferenceable test 9 failed");

// static_assert(c::instance_of_any_integral<std::false_type>);
// static_assert(c::instance_of_bool<std::true_type>);

static_assert(c::instance_of_bool<std::bool_constant<false>>);

enum class my_enum
{
    e0,
    e1
};
static_assert(c::instance_of_enumerable<std::integral_constant<my_enum, my_enum::e1>, my_enum>);

// string concepts
static_assert(c::const_string<std::string>);
static_assert(c::const_string<std::wstring>);
static_assert(c::const_string<std::string_view>);
static_assert(c::const_string_of<std::string, char, std::char_traits<char>>);
static_assert(c::const_string_of<std::wstring, wchar_t, std::char_traits<wchar_t>>);
static_assert(c::const_string_of<std::string_view, char, std::char_traits<char>>);
static_assert(c::string<std::string>);
static_assert(c::string<std::wstring>);
static_assert(!c::string<std::string_view>);
static_assert(c::string_of<std::string, char, std::char_traits<char>>);
static_assert(c::string_of<std::wstring, wchar_t, std::char_traits<wchar_t>>);
static_assert(!c::string_of<std::string_view, char, std::char_traits<char>>);
static_assert(c::string<fixed_string<10>>);
// tuple
static_assert(c::const_tuple_of<std::tuple<int, long, char> const, int, long, char>);
static_assert(c::tuple_of<std::tuple<int, long, char>, int, long, char>);
static_assert(!c::tuple_of<std::tuple<int, long, char> const, int, long, char>);

static_assert(c::list_sink_format_options_of<io::sink_format_traits<enum_::io::tag, char>, char, std::char_traits<char>>);
static_assert(c::list_sink_format_options_of<enum_::io::sink_format_options<char>, char, std::char_traits<char>>);
static_assert(c::list_sink_formatted<enum_::io::tag, char, std::char_traits<char>>);

#endif

BOOST_AUTO_TEST_SUITE(meta)

BOOST_AUTO_TEST_CASE(imap_, *utf::tolerance(0.00000001f))
{
    namespace fus = boost::fusion;
    namespace rfus = fus::result_of;

#ifdef CMN_STATIC_TEST
    using sparce_tuple_type = fus::map<fus::ipair<3, int>, fus::ipair<7, char>, fus::ipair<17, float>>;

    static_assert(std::tuple_size_v<sparce_tuple_type> == 3);
    static_assert(std::same_as<std::tuple_element_t<3, sparce_tuple_type>, int>);
    static_assert(std::same_as<std::tuple_element_t<7, sparce_tuple_type>, char>);
    static_assert(std::same_as<std::tuple_element_t<17, sparce_tuple_type>, float>);
#endif

    auto const sparce_tpl = fus::make_imap<3, 7, 17>(3, '7', 17.f);
#ifdef CMN_STATIC_TEST_type
    static_assert(std::same_as<decltype(sparce_tpl)>, sparce_tuple_type>);
#endif

    BOOST_TEST(std::get<3>(sparce_tpl) == 3);
    BOOST_TEST(std::get<7>(sparce_tpl) == '7');
    BOOST_TEST(std::get<17>(sparce_tpl) == 17.f);
}

BOOST_AUTO_TEST_SUITE_END() // meta
BOOST_AUTO_TEST_SUITE_END() // cmn
