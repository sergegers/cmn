#include <boost/test/unit_test.hpp>

#include <cmn/tuple/va/tuple.h>
#include <cmn/tuple/va/tuple_view.h>
#include <cmn/tuple/va/tuple_adapt.h>
#include <cmn/tuple/va/tuple_event.h>
#include <cmn/util/strong_typedef.h>

BOOST_AUTO_TEST_SUITE(cmn)
BOOST_AUTO_TEST_SUITE(va)

using namespace std::string_literals;

#ifdef CHT_STATIC_TEST

namespace 
{

// tuple
BOOST_MPL_ASSERT_MSG
( 
    (
        std::is_same_v
        <
            tuple_args_t<int, ellipsis, int, int>,
            std::tuple<int, int, int>
        >
    ),
    VA_TUPLE_ARGS_TEST_0_FAILED,
    (tuple_args_t<int, ellipsis, int, int>)
);

static_assert(std::is_same_v<keep_type_t<float, true>, double>);
static_assert(sizeof(std::intptr_t) == slot_size<float, true>());
static_assert(sizeof(std::intptr_t) == slot_size<float, false>());
static_assert(sizeof(std::intptr_t) == slot_size<char, false>());
static_assert(sizeof(std::intptr_t) == slot_size<char, true>());

static_assert(tuple_ellipsis_index<int, ellipsis, int, char>() == 1l);
static_assert(tuple_ellipsis_index<int, int, char>() == -1l);

static_assert(tuple_offset<0, int, ellipsis, int, char>() == 0ul);
static_assert(tuple_offset<1, int, ellipsis, int, char>() == sizeof(std::intptr_t));
static_assert(tuple_offset<2, int, ellipsis, int, char>() == sizeof(std::intptr_t) * 2);
static_assert(0ul == tuple_offset<0, int, ellipsis, float, double>(), "va_tuple_offset() test 0 failed");
static_assert(sizeof(std::intptr_t) == tuple_offset<1, int, ellipsis, float, double>(), "va_tuple_offset() test 1 failed");
static_assert(sizeof(std::intptr_t) * 2 == tuple_offset<2, int, ellipsis, float, double>(), "va_tuple_offset() test 2 failed");
static_assert(sizeof(std::intptr_t) * 3 == tuple_offset<3, int, ellipsis, float, double>(), "va_tuple_offset() test 3 failed");

static_assert(sizeof(std::intptr_t) * 3 == tuple_size_in_bytes<int, int, int>(), "va_tuple_size() test 0 failed");
static_assert(sizeof(std::intptr_t) * 3 == tuple_size_in_bytes<float, int, double>(), "tuple_size_in_bytes() test 1 failed");
static_assert(sizeof(std::intptr_t) * 3 == tuple_size_in_bytes<int, ellipsis, float, double>(), "tuple_size_in_bytes() test 2 failed");



static_assert(std::is_same_v<tuple_element_t<0, int, ellipsis, char, long>, int>, "va_tuple_element_t test 0 failed");
static_assert(std::is_same_v<tuple_element_t<1, int, ellipsis, char, long>, char>, "va_tuple_element_t test 1 failed");
static_assert(std::is_same_v<tuple_element_t<2, int, ellipsis, char, long>, long>, "va_tuple_element_t test 2 failed");

static_assert(std::is_same_v<tuple_keep_element_t<0, int, ellipsis, float, double>, int>,
    "va_tuple_keep_element test 0 failed");
static_assert(std::is_same_v<tuple_keep_element_t<1, int, ellipsis, float, double>, double>,
    "va_tuple_keep_element test 1 failed");
static_assert(std::is_same_v<tuple_keep_element_t<1, int, ellipsis, float const, double>, double>,
    "va_tuple_keep_element test 2 failed");
// NOTE: reference isn't allowed
static_assert(std::is_same_v<tuple_keep_element_t<1, int, ellipsis, float *, double>, float *>,
    "va_tuple_keep_element test 3 failed");
static_assert(std::is_same_v<tuple_keep_element_t<2, int, ellipsis, float, double>, double>,
    "va_tuple_keep_element test 4 failed");
static_assert(std::is_same_v<tuple_keep_element_t<2, int, ellipsis, float, double *>, double *>,
    "va_tuple_keep_element test 5 failed");
static_assert(std::is_same_v<tuple_keep_element_t<2, int, ellipsis, float, double const>, double>,
    "va_tuple_keep_element test 6 failed");

static_assert(has_ellipsis_v<int(int, ellipsis, char)>, "has_ellipsis 0 failed");
static_assert(!has_ellipsis_v<int(int, float, char)>, "has_ellipsis 1 failed");

static_assert
(
    std::is_same_v
    <
        decltype(make_va_tuple_view<int(ellipsis, char)>(from_va_list, std::va_list{})),
        tuple_view<ellipsis, char>
    >,
    "make_va_tuple_view 0 failed"
);

static_assert
(
    std::is_same_v
    <
        decltype(make_va_tuple_view<auto (ellipsis, float, char) -> void>(from_va_list, std::va_list{})),
        tuple_view<ellipsis, float, char>
    >,
    "make_va_tuple_view 1 failed"
);

static_assert
(
    std::is_same_v
    <
        decltype(make_va_tuple_view<void(int, ellipsis, float, char)>(from_va_list, std::va_list{})),
        tuple_view<ellipsis, float, char>
    >,
    "make_va_tuple_view 2 failed"
);

// event
static_assert
(
    std::is_same_v
    <
        boost::optional<int>, 
        decltype
        (
            fire_event
            (
                std::declval<boost::signals2::signal<int (float, int)> &>(),
                std::declval<tuple_view<float, ellipsis, int> &>()
            )
        )
    >,
    "fire_event() 0 failed"
);


static_assert
(
    std::is_same_v
    <
        void, 
        decltype
        (
            fire_event
            (
                std::declval<boost::signals2::signal<void (float, int)> &>(),
                std::declval<tuple_view<float, ellipsis, int> &>()
            )
        )
    >,
    "fire_event() 1 failed"
);

} 

#endif

using test_eva_t = void(int, ellipsis, int, char const *, float, int);

auto va_test(int i, ...) -> void
{
    {
        std::va_list va;

        va_start(va, i);
        auto const tv = make_va_tuple_view<test_eva_t>(from_va_list, va);

        // count of variadic arguments
        static_assert(std::tuple_size_v<decltype(tv)> == 4);
        BOOST_TEST(tv.size() == 4);

        BOOST_TEST(0 == i);
        BOOST_TEST(1 == std::get<0>(tv));
        BOOST_TEST("two"s == std::get<1>(tv));
        BOOST_TEST(3.F == std::get<2>(tv));
        BOOST_TEST(4 == std::get<3>(tv));
        va_end(va);
    }

    {
        auto const tv = make_va_tuple_view<test_eva_t>(i);
        static_assert(5 == tv.size(), "");

        BOOST_TEST_REQUIRE(std::size(tv) == 5);

        BOOST_TEST(0 == std::get<0>(tv));
        BOOST_TEST(1 == std::get<1>(tv));
        BOOST_TEST("two"s == std::get<2>(tv));
        BOOST_TEST(3.F == std::get<3>(tv));
        BOOST_TEST(4 == std::get<4>(tv));

        auto const &[a, b, c, d, e] = tv;
        BOOST_TEST(0 == a);
        BOOST_TEST(1 == b);
        BOOST_TEST("two"s == c);
        BOOST_TEST(3.F == d);
        BOOST_TEST(4 == e);
    }
}

#pragma warning(push)
#pragma warning(disable: 4840)

BOOST_AUTO_TEST_CASE(va_tuple_view)
{
    va_test(0, 1, "two", 3.F, 4);
}

#pragma warning(pop)

using pf1_t = double(*)(int, long, float, double);
using f1_1_eva_t = double(pf1_t, ellipsis, int, long, float, double);

auto sum(int i, long l, float f, double d) -> double
{
    return static_cast<double>(i) + static_cast<double>(l) + static_cast<double>(f) + d;
}

auto f1_(pf1_t f, ...) -> double
{
    std::va_list va;
    va_start(va, f);

    auto h = make_va_tuple_view<f1_1_eva_t>(from_va_list , va);
    BOOST_TEST(h.size() == 4);
    
    BOOST_MPL_ASSERT_MSG(
        (std::is_same_v<decltype(h)::args_type, std::tuple<ellipsis, int, long, float, double>>),
        Type_distorsion, (decltype(h), decltype(h)::args_type));
    BOOST_MPL_ASSERT_MSG((std::is_same_v<std::tuple_element_t<0, decltype(h)>, int>),
        Element_0_failed, (decltype(h), std::tuple_element_t<0, decltype(h)>));
    static_assert(std::is_same_v<std::tuple_element_t<1, decltype(h)>, long>, "element 1 failed");
    static_assert(std::is_same_v<std::tuple_element_t<2, decltype(h)>, float>, "element 2 failed");
    static_assert(std::is_same_v<std::tuple_element_t<3, decltype(h)>, double>, "element 3 failed");

    BOOST_TEST(std::get<0>(h), 1);
    BOOST_TEST(std::get<1>(h), 2L);
    BOOST_TEST(std::get<2>(h), 3.F);
    BOOST_TEST(std::get<3>(h), 4.);

    auto const res = boost::fusion::invoke(*f, h);

    va_end(va);
    return res;
}

BOOST_AUTO_TEST_CASE(apply)
{
    BOOST_CHECK_EQUAL(f1_(&sum, 1, 2L, 3.F, 4.), 10.);
}

float g_sum;

auto f2_(int i, ...) -> void
{
    boost::signals2::signal<void(int, float, int)> sig;
    sig.connect
    (
        [](int i0, float f1, int i2) 
        { 
            g_sum = static_cast<float>(i0) + f1 + static_cast<float>(i2); 
        }
    );
    auto const tv = make_va_tuple_view<void(int, ellipsis, float, int)>(i);
    fire_event(sig, tv);
    sig.disconnect_all_slots();
}

auto f3_(int i, ...) -> void
{
    boost::signals2::signal<void(float, int)> sig;
    sig.connect
    (
        [](float f1, int i2) 
        { 
            g_sum = f1 + static_cast<float>(i2); 
        }
    );

    std::va_list va;
    va_start(va, i);
    auto const tv = make_va_tuple_view<void(int, ellipsis, float, int)>(from_va_list, va);
    fire_event(sig, tv);
    sig.disconnect_all_slots();
}

auto f4_(std::va_list va) -> void
{
    boost::signals2::signal<void(int, float)> sig;
    sig.connect
    (
        [](int i1, float f2)
        {
            g_sum = static_cast<float>(i1) + f2;
        }
    );

    auto const tv = make_va_tuple_view<void(ellipsis, int, float)>(from_va_list, va);
    fire_event(sig, tv);
    sig.disconnect_all_slots();
}

auto f5_(int i, ...) -> void
{
    std::va_list va;
    va_start(va, i);
    f4_(va);
}


auto f6_(int i, ...) -> float
{
    boost::signals2::signal<float(int, float, int)> sig;
    boost::signals2::scoped_connection sconn = sig.connect
    (
        [](int i0, float f1, int i2) 
        { 
            return static_cast<float>(i0) + f1 + static_cast<float>(i2); 
        }
    );
    auto const tv = make_va_tuple_view<void(int, ellipsis, float, int)>(i);
    static_assert(std::is_same<boost::optional<float>, decltype(fire_event(sig, tv))>::value, "smth. wrong");

    return *fire_event(sig, tv);
}

BOOST_AUTO_TEST_CASE(fire_evt)
{
    g_sum = 0.F;
    f2_(1, 2.F, 3);
    BOOST_TEST(g_sum == 6.F);

    g_sum = 0.F;
    f3_(4, 5.F, 6);
    BOOST_TEST(g_sum == 11.F);

    g_sum = 0.F;
    f5_(6, 7, 8.F);
    BOOST_TEST(g_sum == 15.F);

    BOOST_TEST(19.F == f6_(1, 8.F, 10));
}

using f7_eva_t = float(float, ellipsis, float, float, float);

auto f7_(float f, ...) -> float
{
    auto const tv = make_va_tuple_view<f7_eva_t>(f);
    BOOST_TEST(1.F == std::get<0>(tv));
    BOOST_TEST(2.F == std::get<1>(tv));
    BOOST_TEST(3.F == std::get<2>(tv));
    BOOST_TEST(4.F == std::get<3>(tv));

    return std::get<0>(tv) + std::get<1>(tv) + std::get<2>(tv) + std::get<3>(tv);
}


using f8_eva_t = double(double, ellipsis, double, double, double);

auto f8_(double d, ...) -> double
{
    auto const tv = make_va_tuple_view<f8_eva_t>(d);
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

using fn7_eva_t = void(ellipsis, int, bool, bool, float, int);

auto f7_(std::va_list va) -> void
{
    auto const tv = make_va_tuple_view<fn7_eva_t>(from_va_list, va);
    BOOST_TEST(tv.get<0>() == 1);
    BOOST_TEST(tv.get<1>() == false);
    BOOST_TEST(tv.get<2>() == true);
    BOOST_TEST(tv.get<3>() == 2.F);
    BOOST_TEST(tv.get<4>() == 3);
}

BOOST_AUTO_TEST_CASE(va_tuple)
{
    auto const args = make_va_tuple<fn7_eva_t>(1, false, true, 2.F, 3);
    f7_(args.as_va_list());
}

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

using fn8_va_t = auto (char, my_int_t) -> void;
using fn8_eva_t = auto (ellipsis, char, my_int_t) -> void;

auto f8_(std::va_list va) -> void
{
    auto const tv = make_va_tuple_view<fn8_eva_t>(from_va_list, va);
    BOOST_TEST(tv.get<0>() == 'c');
    BOOST_TEST(tv.get<1>() == 17);
}

BOOST_AUTO_TEST_CASE(unit)
{
    auto const args = make_va_tuple<fn8_eva_t>('c', my_int_t{ 17 });
    f8_(args.as_va_list());    
}

BOOST_AUTO_TEST_CASE(uint_2)
{
    auto const args = make_va_tuple<fn8_eva_t>('c', my_int_t{ 17 });
    auto const va = args.as_va_list();
    auto const tv = make_va_tuple_view<fn8_eva_t>(from_va_list, va);
    BOOST_TEST(std::get<0>(tv) == 'c');
    BOOST_TEST(std::get<1>(tv) == my_int_t{ 17 });
}

using fn9_eva_t = auto (int, ellipsis, my_int_t, int) -> void;

auto f9_(int i, ...) -> void
{
    auto const tv = make_va_tuple_view<fn9_eva_t>(i);
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

