
#include <concepts>
#include <cstdarg>

#include <boost/test/unit_test.hpp>

#include <boost/fusion/functional/invocation/invoke.hpp>
#include <boost/signals2.hpp>

#include <cmn/tuple/tuple.h>
#include <cmn/tuple/va/tuple.h>
#include <cmn/tuple/va/tuple_view.h>
#include <cmn/tuple/va/adapt.h>
#include <cmn/tuple/va/traits.h>
#include <cmn/tuple/va/event.h>
#include <cmn/tuple/va/detail/util.h>
#include <cmn/util/strong_typedef.h>

BOOST_AUTO_TEST_SUITE(cmn)
BOOST_AUTO_TEST_SUITE(va)

using namespace std::string_literals;

// tuple
static_assert(std::same_as<remove_ellipsis_t<auto (int, ellipsis, int, int) -> void>, auto (int, int, int) -> void>);

static_assert(std::same_as<detail::keep_type_t<float>, double>);
static_assert(sizeof(std::intptr_t) == detail::va_slot_size_in_bytes_v<float>);
static_assert(sizeof(std::intptr_t) == detail::va_slot_size_in_bytes_v<char>);

static_assert(ellipsis_index_v<int, ellipsis, int, char> == 1ul);
static_assert(ellipsis_index_v<int, int, char> == 3ul);

static_assert(detail::va_arg_offset_in_bytes_v<> == 0ul);
static_assert(detail::va_arg_offset_in_bytes_v<int> == sizeof(std::intptr_t));
static_assert(detail::va_arg_offset_in_bytes_v<int, char> == sizeof(std::intptr_t) * 2);

static_assert(std::same_as<std::tuple_element_t<0, tuple_view<auto (int, ellipsis, char, long) -> int>>, int>, "tuple_element_t test 0 failed");
static_assert(std::same_as<std::tuple_element_t<1, tuple_view<auto (int, ellipsis, char, long) -> int>>, char>, "tuple_element_t test 1 failed");
static_assert(std::same_as<std::tuple_element_t<2, tuple_view<auto (int, ellipsis, char, long) -> int>>, long>, "va_tuple_element_t test 2 failed");

static_assert(std::same_as<tuple_keep_element_t<0, tuple_view<auto (int, ellipsis, float, double) -> void>>, int &>,
    "tuple_view_keep_element_t test 0 failed");
static_assert(std::same_as<tuple_keep_element_t<1, tuple_view<auto (int, ellipsis, float, double) -> void>>, double>,
    "tuple_view_keep_element_t test 1 failed");
static_assert(std::same_as<tuple_keep_element_t<2, tuple_view<auto (int, ellipsis, float, double) -> void>>, double>,
    "tuple_view_keep_element_t test 2 failed");
// NOTE: reference isn't allowed
static_assert(std::same_as<tuple_keep_element_t<1, tuple_view<auto (int, ellipsis, float *, double) -> void>>, float *>,
    "tuple_view_keep_element_t 3 failed");
static_assert(std::same_as<tuple_keep_element_t<2, tuple_view<auto (int, ellipsis, float, double) -> void>>, double>,
    "tuple_view_keep_element_t 4 failed");
static_assert(std::same_as<tuple_keep_element_t<2, tuple_view<auto (int, ellipsis, float, double *) -> void>>, double *>,
    "tuple_view_keep_element_t test 5 failed");

static_assert(has_ellipsis_v<auto (int, ellipsis, char) -> int>, "has_ellipsis 0 failed");
static_assert(!has_ellipsis_v<auto (int, float, char) -> int>, "has_ellipsis 1 failed");

// event
static_assert
(
    std::same_as
    <
        boost::optional<int>, 
        decltype
        (
            fire_event
            (
                std::declval<boost::signals2::signal<auto (float, int) -> int> &>(),
                std::declval<tuple_view<auto (float, ellipsis, int) -> void> &>()
            )
        )
    >,
    "fire_event() 0 failed"
);


static_assert
(
    std::same_as
    <
        void, 
        decltype
        (
            fire_event
            (
                std::declval<boost::signals2::signal<auto (float, int) -> void> &>(),
                std::declval<tuple_view<auto (float, ellipsis, int) -> void> &>()
            )
        )
    >,
    "fire_event() 1 failed"
);

struct xx
{
    int x0;
    int x1;
    int x2;

    friend constexpr auto operator <=> (xx const &, xx const &) = default;

    friend auto operator << (std::ostream &ostr, xx const &xx_) -> std::ostream &
    {
        return ostr << "{ " << xx_.x0 << ", " << xx_.x1 << ", " << xx_.x2 << " }";
    }
};

using test_eva_t = auto (int, ellipsis, int, char, char const *, xx, float, long) -> void;

auto va_test(int i, ...) -> void
{
    tuple_view<test_eva_t> const tv { i };

    static_assert(std::tuple_size_v<decltype(tv)> == 7);
    static_assert(std::same_as<std::tuple_element_t<0, decltype(tv)>, int const>);
    static_assert(std::same_as<std::tuple_element_t<1, decltype(tv)>, int const>);
    static_assert(std::same_as<std::tuple_element_t<2, decltype(tv)>, char const>);
    static_assert(std::same_as<std::tuple_element_t<3, decltype(tv)>, char const * const>);
    static_assert(std::same_as<std::tuple_element_t<4, decltype(tv)>, xx const>);
    static_assert(std::same_as<std::tuple_element_t<5, decltype(tv)>, float const>);
    static_assert(std::same_as<std::tuple_element_t<6, decltype(tv)>, long const>);

    static_assert(7 == tv.size());

    BOOST_TEST_REQUIRE(std::size(tv) == 7);

    BOOST_TEST(0 == i);
    BOOST_TEST(0 == std::get<0>(tv));
    BOOST_TEST(1 == std::get<1>(tv));
    BOOST_TEST('x' == std::get<2>(tv));
    BOOST_TEST("two"s == std::get<3>(tv));
    xx const xx_{4, 5, 6 };
    BOOST_TEST(xx_ == std::get<4>(tv));
    BOOST_TEST(3.f == std::get<5>(tv));
    BOOST_TEST(4 == std::get<6>(tv));

    auto const &[a, b, c, d, e, f, g] = tv;
    BOOST_TEST(0 == a);
    BOOST_TEST(1 == b);
    BOOST_TEST('x' == c);
    BOOST_TEST("two"s == d);
    BOOST_TEST(xx_ == e);
    BOOST_TEST(3.f == f);
    BOOST_TEST(4 == g);
}

BOOST_AUTO_TEST_CASE(va_tuple_view)
{
    va_test(0, 1, 'x', "two", xx{ 4, 5, 6 }, 3.f, 4l);
}

//-----------------------------------------------------------------------------
using test_eva_2_t = auto (int, char, float, ellipsis, int, char, char const *, xx, float, long) -> void;

auto va_test_2(int i, char j, float k, ...) -> void
{
    tuple_view<test_eva_2_t> const tv { i, j, k };

    static_assert(std::tuple_size_v<decltype(tv)> == 9);
    static_assert(std::same_as<std::tuple_element_t<0, decltype(tv)>, int const>);
    static_assert(std::same_as<std::tuple_element_t<1, decltype(tv)>, char const>);
    static_assert(std::same_as<std::tuple_element_t<2, decltype(tv)>, float const>);
    static_assert(std::same_as<std::tuple_element_t<3, decltype(tv)>, int const>);
    static_assert(std::same_as<std::tuple_element_t<4, decltype(tv)>, char const>);
    static_assert(std::same_as<std::tuple_element_t<5, decltype(tv)>, char const * const>);
    static_assert(std::same_as<std::tuple_element_t<6, decltype(tv)>, xx const>);
    static_assert(std::same_as<std::tuple_element_t<7, decltype(tv)>, float const>);
    static_assert(std::same_as<std::tuple_element_t<8, decltype(tv)>, long const>);
    static_assert(9 == tv.size());

    BOOST_TEST_REQUIRE(std::size(tv) == 9);

    BOOST_TEST(0 == i);
    BOOST_TEST('a' == j);
    BOOST_TEST(32.f == k);
    BOOST_TEST(0 == std::get<0>(tv));
    BOOST_TEST('a' == std::get<1>(tv));
    BOOST_TEST(32.f == std::get<2>(tv));
    BOOST_TEST(1 == std::get<3>(tv));
    BOOST_TEST('x' == std::get<4>(tv));
    BOOST_TEST("two"s == std::get<5>(tv));
    xx const xx_{4, 5, 6 };
    BOOST_TEST(xx_ == std::get<6>(tv));
    BOOST_TEST(3.F == std::get<7>(tv));
    BOOST_TEST(4l == std::get<8>(tv));

    auto const &[a, b, c, d, e, f, g, h, ii] = tv;
    BOOST_TEST(0 == a);
    BOOST_TEST('a' == b);
    BOOST_TEST(32.f == c);
    BOOST_TEST(1 == d);
    BOOST_TEST('x' == e);
    BOOST_TEST("two"s == f);
    BOOST_TEST(xx_ == g);
    BOOST_TEST(3.F == h);
    BOOST_TEST(4l == ii);
}

BOOST_AUTO_TEST_CASE(va_tuple_view_2)
{
    va_test_2(0, 'a', 32.f, 1, 'x', "two", xx{ 4, 5, 6 }, 3.f, 4l);
}

//-----------------------------------------------------------------------------
using pf1_t = auto (*)(int, long, float, double) -> double;
using f1_1_eva_t = auto (pf1_t, ellipsis, int, long, float, double) -> double;

auto sum(int i, long l, float f, double d) -> double
{
    return static_cast<double>(i) + static_cast<double>(l) + static_cast<double>(f) + d;
}

auto f1_(pf1_t f, ...) -> double
{
    tuple_view<f1_1_eva_t> h { f };

    static_assert(tuple_view<f1_1_eva_t>::size() == 5);
    static_assert(std::same_as<decltype(h)::eargs_type, std::tuple<pf1_t, ellipsis, int, long, float, double>>);
    static_assert((std::same_as<std::tuple_element_t<0, decltype(h)>, pf1_t>), "element 0 failed");
    static_assert(std::same_as<std::tuple_element_t<1, decltype(h)>, int>, "element 1 failed");
    static_assert(std::same_as<std::tuple_element_t<2, decltype(h)>, long>, "element 2 failed");
    static_assert(std::same_as<std::tuple_element_t<3, decltype(h)>, float>, "element 3 failed");
    static_assert(std::same_as<std::tuple_element_t<4, decltype(h)>, double>, "element 4 failed");

    BOOST_TEST(std::get<0>(h), 1);
    BOOST_TEST(std::get<1>(h), 2L);
    BOOST_TEST(std::get<2>(h), 3.F);
    BOOST_TEST(std::get<3>(h), 4.);

    auto hh = tuple_cut_left(h);
    auto const res = boost::fusion::invoke(*f, hh);
    return res;
}

BOOST_AUTO_TEST_CASE(fus_infoke)
{
    BOOST_CHECK_EQUAL(f1_(&sum, 1, 2L, 3.F, 4.), 10.);
}

//-----------------------------------------------------------------------------
float g_sum;

auto f2_(int i, ...) -> void
{
    boost::signals2::signal<auto (int, float, int) -> void> sig;
    sig.connect
    (
        [](int i0, float f1, int i2) 
        { 
            g_sum = static_cast<float>(i0) + f1 + static_cast<float>(i2); 
        }
    );
    tuple_view<auto (int, ellipsis, float, int) -> void> const tv {i };
    BOOST_TEST(std::get<0>(tv) == 1);
    BOOST_TEST(std::get<1>(tv) == 2.f);
    BOOST_TEST(std::get<2>(tv) == 3);

    fire_event(sig, tv);
    sig.disconnect_all_slots();
}

auto f3_(int i, ...) -> void
{
    boost::signals2::signal<auto (float, int) -> void> sig;
    sig.connect
    (
        [](float f1, int i2) 
        { 
            g_sum = f1 + static_cast<float>(i2); 
        }
    );

    tuple_view<auto (int, ellipsis, float, int) -> void> const tv { i };
    BOOST_TEST(std::get<0>(tv) == 4);
    BOOST_TEST(std::get<1>(tv) == 5.f);
    BOOST_TEST(std::get<2>(tv) == 6);

    auto const tvv = tuple_cut_left(tv);
    BOOST_TEST(std::get<0>(tvv) == 5.f);
    BOOST_TEST(std::get<1>(tvv) == 6);

    fire_event(sig, tvv);
    sig.disconnect_all_slots();
}

auto f4_(std::va_list va) -> void
{
    boost::signals2::signal<auto (int, float) -> void> sig;
    sig.connect
    (
        [](int i1, float f2)
        {
            g_sum = static_cast<float>(i1) + f2;
        }
    );

    tuple_view<auto (ellipsis, int, float) -> void> const tv{ va };
    BOOST_TEST(std::get<0>(tv) == 7);
    BOOST_TEST(std::get<1>(tv) == 8.f);

    fire_event(sig, tv);
    sig.disconnect_all_slots();
}

auto f5_(int i, ...) -> void
{
    BOOST_TEST(i == 6);

    std::va_list va;
    va_start(va, i);
    tuple_view<auto (ellipsis, int, float) -> void> const tv{ va };
    BOOST_TEST(std::get<0>(tv) == 7);
    BOOST_TEST(std::get<1>(tv) == 8.f);

    f4_(va);
    va_end(va);
}

auto f6_(int i, ...) -> float
{
    boost::signals2::signal<auto (int, float, int) -> float> sig;
    boost::signals2::scoped_connection sconn = sig.connect
    (
        [](int i0, float f1, int i2) 
        { 
            return static_cast<float>(i0) + f1 + static_cast<float>(i2); 
        }
    );
    tuple_view<auto (int, ellipsis, float, int) -> float> const tv{ i };
    static_assert(std::same_as<boost::optional<float>, decltype(fire_event(sig, tv))>, "smth. wrong");

    return *fire_event(sig, tv);
}

BOOST_AUTO_TEST_CASE(fire_evt)
{
    g_sum = 0.f;
    f2_(1, 2.f, 3);
    BOOST_TEST(g_sum == 6.f);

    g_sum = 0.f;
    f3_(4, 5.f, 6);
    BOOST_TEST(g_sum == 11.f);

    g_sum = 0.f;
    f5_(6, 7, 8.f);
    BOOST_TEST(g_sum == 15.f);

    BOOST_TEST(19.f == f6_(1, 8.f, 10));
}

//-----------------------------------------------------------------------------
using f7_eva_t = float(float, ellipsis, float, float, float);

auto f7_(float f, ...) -> float
{
    tuple_view<f7_eva_t> const tv{ f };
    BOOST_TEST(1.F == std::get<0>(tv));
    BOOST_TEST(2.F == std::get<1>(tv));
    BOOST_TEST(3.F == std::get<2>(tv));
    BOOST_TEST(4.F == std::get<3>(tv));

    return std::get<0>(tv) + std::get<1>(tv) + std::get<2>(tv) + std::get<3>(tv);
}


using f8_eva_t = double(double, ellipsis, double, double, double);

auto f8_(double d, ...) -> double
{
    tuple_view<f8_eva_t> const tv{ d };
    BOOST_TEST(1. == std::get<0>(tv));
    BOOST_TEST(2. == std::get<1>(tv));
    BOOST_TEST(3. == std::get<2>(tv));
    BOOST_TEST(4. == std::get<3>(tv));

    return std::get<0>(tv) + std::get<1>(tv) + std::get<2>(tv) + std::get<3>(tv);
}

BOOST_AUTO_TEST_CASE(float_double)
{
    auto const fres = f7_(1.F, 2.F, 3.F, 4.F);
    BOOST_TEST(10.F == fres);

    auto const dres = f8_(1., 2., 3., 4.);
    BOOST_TEST(10. == dres);
}

//using fn7_eva_t = void(ellipsis, int, bool, bool, float, int);
//
//auto f7_(std::va_list va) -> void
//{
//    auto const tv = make_va_tuple_view<fn7_eva_t>(from_va_list, va);
//    BOOST_TEST(tv.get<0>() == 1);
//    BOOST_TEST(tv.get<1>() == false);
//    BOOST_TEST(tv.get<2>() == true);
//    BOOST_TEST(tv.get<3>() == 2.F);
//    BOOST_TEST(tv.get<4>() == 3);
//}
//
//BOOST_AUTO_TEST_CASE(va_tuple)
//{
//    auto const args = make_va_tuple<fn7_eva_t>(1, false, true, 2.F, 3);
//    f7_(args.as_va_list());
//}

constexpr char const *test = "test_%s";
constexpr char const *me = "me";

template <typename... Args>
auto call_snprintf(char *buffer, std::size_t size, char const *format, Args &&...args) -> void
{
    std::invoke(&std::snprintf, buffer, size, format, std::forward<Args>(args)...);
}

BOOST_AUTO_TEST_CASE(sprintf_fwd)
{
    char buffer[0x100];
    call_snprintf(buffer, std::size(buffer), test, me);
    BOOST_TEST(buffer, "test_me");
}

using my_int_t = strong_typedef_flag_impl<struct my_int_, int>;
static_assert(c::va_arg_<my_int_t>);
static_assert(c::unit<my_int_t>);
static_assert(!c::ref_va_arg<my_int_t>);

using fn8_va_t = auto (char, my_int_t) -> void;
using fn8_eva_t = auto (ellipsis, char, my_int_t) -> void;

auto f8_(std::va_list va) -> void
{
    tuple_view<fn8_eva_t> const tv { va };
    BOOST_TEST(tv.get<0>() == 'c');
    BOOST_TEST(tv.get<1>() == 17);
}

BOOST_AUTO_TEST_CASE(unit)
{
    tuple<fn8_eva_t> args{ 'c', my_int_t{ 17 } };
    f8_(args.as_va_list());    
}

BOOST_AUTO_TEST_CASE(uint_2)
{
    auto const args = tuple<fn8_eva_t>{ 'c', my_int_t{ 17 } };
    auto const va = args.as_va_list();
    auto const tv = tuple_view<fn8_eva_t>{ va };
    BOOST_TEST(std::get<0>(tv) == 'c');
    BOOST_TEST(std::get<1>(tv) == my_int_t{ 17 });
}

using fn9_eva_t = auto (int, ellipsis, my_int_t, int) -> void;

auto f9_(int i, ...) -> void
{
    tuple_view<fn9_eva_t> const tv{ i };
    BOOST_TEST(1 == std::get<0>(tv));
    BOOST_TEST(2 == std::get<1>(tv));
    BOOST_TEST(3 == std::get<2>(tv));
}

BOOST_AUTO_TEST_CASE(uint_3)
{
    f9_(1, 2, 3);
}

BOOST_AUTO_TEST_SUITE_END() // va
BOOST_AUTO_TEST_SUITE_END() // cmn

