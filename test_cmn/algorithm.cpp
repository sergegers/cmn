
#include <utility>
#include <tuple>

#include <boost/fusion/container/generation/make_vector.hpp>
#include <boost/fusion/view/zip_view.hpp>
#include <boost/fusion/adapted/std_tuple.hpp>
#include <boost/fusion/adapted/std_pair.hpp>

#include <boost/test/unit_test.hpp>

#include <cmn/algorithm/binary_find.h>
#include <cmn/algorithm/find.h>
#include <cmn/algorithm/at.h>
#include <cmn/algorithm/erase.h>
#include <cmn/algorithm/switch.h>

#include <cmn/util/util.h>
#include <cmn/tuple/io.h>

constexpr auto operator ==(char, std::string const &) -> bool { return false; }

BOOST_AUTO_TEST_SUITE(cmn)

struct test_proc
{
    template <typename T> auto operator ()() const -> void {}
};

struct test_func
{
    template <typename T> auto operator ()() const noexcept { return 1; }
};

static_assert(detail::is_type_proc_v<test_proc, int>, "is_proc_noctor_v failed 0");
static_assert(!detail::is_type_proc_v<test_func, int>, "is_proc_noctor_v failed 1"); 

BOOST_AUTO_TEST_SUITE(algorithm)

using namespace std::string_literals;
using namespace literals;

BOOST_AUTO_TEST_CASE(bfind_mp11)
{
    using namespace boost::mp11;
    using test_list_type = mp_list
    <
          mp_int<-3>
        , mp_int<1>
        , mp_int<2>
        , mp_int<3>
        , mp_int<4>
        , mp_int<11>
        , mp_int<99>
    >;

    BOOST_TEST(0 == binary_find_mp11<test_list_type>(mp_int < -3 > {}));
    BOOST_TEST(1 == binary_find_mp11<test_list_type>(mp_int < 1 > {}));
    BOOST_TEST(2 == binary_find_mp11<test_list_type>(mp_int < 2 > {}));
    BOOST_TEST(3 == binary_find_mp11<test_list_type>(mp_int < 3 > {}));
    BOOST_TEST(4 == binary_find_mp11<test_list_type>(mp_int < 4 > {}));
    BOOST_TEST(5 == binary_find_mp11<test_list_type>(mp_int < 11 > {}));
    BOOST_TEST(6 == binary_find_mp11<test_list_type>(mp_int < 99 > {}));
    BOOST_TEST(-1 == binary_find_mp11<test_list_type>(mp_int < -66 > {}));
    BOOST_TEST(-1 == binary_find_mp11<test_list_type>(mp_int < 0 > {}));
    BOOST_TEST(-1 == binary_find_mp11<test_list_type>(mp_int < 5 > {}));
    BOOST_TEST(-1 == binary_find_mp11<test_list_type>(mp_int < 100 > {}));
}

BOOST_AUTO_TEST_CASE(bfind_mpl_vis)
{
    using namespace boost::mp11;
    using test_list_type = mp_list<mp_int<-3>, mp_int<1>, mp_int<2>, mp_int<3>, mp_int<4>, mp_int<11>, mp_int<99>>;

    auto constexpr vis = []<typename Digit>(Digit /*digit*/) { return Digit::value; };

    BOOST_TEST(-3 == binary_find_mp11<test_list_type>(mp_int < -3 > {}, vis).first);
    BOOST_TEST(1 == binary_find_mp11<test_list_type>(mp_int < 1 > {}, vis).first);
    BOOST_TEST(2 == binary_find_mp11<test_list_type>(mp_int < 2 > {}, vis).first);
    BOOST_TEST(3 == binary_find_mp11<test_list_type>(mp_int < 3 > {}, vis).first);
    BOOST_TEST(4 == binary_find_mp11<test_list_type>(mp_int < 4 > {}, vis).first);
    BOOST_TEST(11 == binary_find_mp11<test_list_type>(mp_int < 11 > {}, vis).first);
    BOOST_TEST(99 == binary_find_mp11<test_list_type>(mp_int < 99 > {}, vis).first);
    BOOST_TEST(-1 == binary_find_mp11<test_list_type>(mp_int < -66 > {}, vis).second);
    BOOST_TEST(-1 == binary_find_mp11<test_list_type>(mp_int < 0 > {}, vis).second);
    BOOST_TEST(-1 == binary_find_mp11<test_list_type>(mp_int < 5 > {}, vis).second);
    BOOST_TEST(-1 == binary_find_mp11<test_list_type>(mp_int < 100 > {}, vis).second);
}

BOOST_AUTO_TEST_CASE(bfind_fus)
{
    using namespace boost::mp11;
    using boost::fusion::make_vector;

    auto test_vector = make_vector
    (
          mp_int<-3>{}
        , mp_int<1>{}
        , mp_int<2>{}
        , mp_int<4>{}
        , mp_int<11>{}
        , mp_int<99>{}
    );

    BOOST_TEST(0 == binary_find_fus(test_vector, mp_int < -3 > {}));
    BOOST_TEST(1 == binary_find_fus(test_vector, mp_int < 1 > {}));
    BOOST_TEST(2 == binary_find_fus(test_vector, mp_int < 2 > {}));
    BOOST_TEST(3 == binary_find_fus(test_vector, mp_int < 4 > {}));
    BOOST_TEST(4 == binary_find_fus(test_vector, mp_int < 11 > {}));
    BOOST_TEST(5 == binary_find_fus(test_vector, mp_int < 99 > {}));
    BOOST_TEST(-1 == binary_find_fus(test_vector, mp_int < -66 > {}));
    BOOST_TEST(-1 == binary_find_fus(test_vector, mp_int < 0 > {}));
    BOOST_TEST(-1 == binary_find_fus(test_vector, mp_int < 3 > {}));
    BOOST_TEST(-1 == binary_find_fus(test_vector, mp_int < 100 > {}));
}

BOOST_AUTO_TEST_CASE(bfind_fus_vis)
{
    using namespace boost::mp11;
    using boost::fusion::make_vector;

    auto test_vector = make_vector
    (
          mp_int<-3>{}
        , mp_int<1>{}
        , mp_int<2>{}
        , mp_int<4>{}
        , mp_int<11>{}
        , mp_int<99>{}
    );
    auto vis = []<typename Digit>(Digit) { return std::decay_t<Digit>::value; };

    BOOST_TEST(-3 == binary_find_fus(test_vector, mp_int < -3 > {}, vis).first);
    BOOST_TEST(1 == binary_find_fus(test_vector, mp_int < 1 > {}, vis).first);
    BOOST_TEST(2 == binary_find_fus(test_vector, mp_int < 2 > {}, vis).first);
    BOOST_TEST(4 == binary_find_fus(test_vector, mp_int < 4 > {}, vis).first);
    BOOST_TEST(11 == binary_find_fus(test_vector, mp_int < 11 > {}, vis).first);
    BOOST_TEST(99 == binary_find_fus(test_vector, mp_int < 99 > {}, vis).first);
    BOOST_TEST(-1 == binary_find_fus(test_vector, mp_int < -66 > {}, vis).second);
    BOOST_TEST(-1 == binary_find_fus(test_vector, mp_int < 0 > {}, vis).second);
    BOOST_TEST(-1 == binary_find_fus(test_vector, mp_int < 3 > {}, vis).second);
    BOOST_TEST(-1 == binary_find_fus(test_vector, mp_int < 100 > {}, vis).second);
}

BOOST_AUTO_TEST_CASE(bfind_fus_vis_single)
{
    using namespace boost::mp11;
    using boost::fusion::make_vector;

    auto test_vector = make_vector(mp_int<0>{});

    bool exec = false;
    auto vis = [&exec](auto /*digit*/) {exec = true; };

    BOOST_TEST(0 == binary_find_fus(test_vector, mp_int < 0 > {}, vis));
    BOOST_TEST(exec);

    exec = false;
    BOOST_TEST(-1 == binary_find_fus(test_vector, mp_int < 5 > {}, vis));
    BOOST_TEST(!exec);
}

BOOST_AUTO_TEST_CASE(bfind_fus_vis_args)
{
    using namespace boost::mp11;
    using boost::fusion::make_vector;

    auto test_vector = make_vector
    (
          mp_int<0>{}
        , mp_int<3>{}
        , mp_int<4>{}
        , mp_int<8>{}
        , mp_int<11>{}
    );

    auto vis = [](auto digit, int a, int b) { return std::tuple{ digit.value, a, b }; };

    {
        auto const res = binary_find_fus(test_vector, mp_int < 3 > {}, vis, 2, 3);
        BOOST_TEST(res == (std::pair{ std::tuple{ 3, 2, 3 }, 1z }));
    }

    {
        auto const res = binary_find_fus(test_vector, mp_int < 5 > {}, vis, 2, 3);
        BOOST_TEST(res == (std::pair{ std::tuple{ 0, 0, 0 }, -1z }));
    }
}

BOOST_AUTO_TEST_CASE(erase_ass)
{
    using namespace std::string_literals;

    std::map<int, std::string> src
    {
        { 0, "zero"s },
        { 1, "one"s },
        { 2, "two"s }
    };

    erase(src, [](auto const &pair) { return 0 != pair.first % 2; });

    std::map<int, std::string> dst
    {
        { 0, "zero"s },
        { 2, "two"s }
    };

    BOOST_TEST(src == dst);
}

struct inc_vis
{
    template <typename T> constexpr auto operator ()(T &t) const -> void { ++t; }
    constexpr auto operator ()(std::string &) const -> void {}
};

struct one_pred
{
    template <typename T> constexpr auto operator ()(T const &/*t*/) const -> bool { return false; }
    constexpr auto operator ()(char const &c) const -> bool { return c == 0x1; }
};

BOOST_AUTO_TEST_CASE(find_fus_)
{
    int i = 0; char j = 1; std::string k = "three"s;

    using ref_sequence_type = boost::fusion::vector<int &, char &, std::string &>;
    ref_sequence_type seq { i, j, k };

    auto const pos = find_if_fus
    (
        seq,
        one_pred {},
        inc_vis {}
    );

    using boost::fusion::at_c;
    BOOST_TEST(pos == 1);
    BOOST_TEST(at_c<0>(seq) == 0);
    BOOST_TEST(at_c<1>(seq) == 2);
}

BOOST_AUTO_TEST_CASE(find_if_fus_)
{
    auto t0 = std::tuple{ 1, 2, 'a' };
    auto t1 = std::tuple{ 3, 2, 'd' };

    using boost::fusion::zip_view;
    using boost::fusion::at_c;

    using zip_seq_type = std::tuple<decltype(t0) &, decltype(t1) &>;
    auto const zv = zip_view<zip_seq_type>{ zip_seq_type{ t0, t1 } };

    auto const pos = find_if_fus
    (
        zv, [](auto const &p) { return at_c<0>(p) == at_c<1>(p); }
    );

    BOOST_TEST(pos == 1);
}


template <typename T>
struct is_unsigned_impl;

template <typename T, T I_>
struct is_unsigned_impl<std::integral_constant<T, I_>>: 
    std::is_unsigned<T>
{};

struct is_unsigned_
{
    template <typename T>
    constexpr auto operator ()() const -> bool { return is_unsigned_impl<T>::value; }
};

struct return_value
{
    template <typename T>
    constexpr auto operator ()() const { return static_cast<unsigned long>(T::value); }
};

BOOST_AUTO_TEST_CASE(find_if_noctor_mp11_)
{
    using sequence_type = std::tuple
    <
        std::integral_constant<char, 2>, 
        std::integral_constant<short, -4>, 
        std::integral_constant<unsigned short, 3>,
        std::integral_constant<unsigned long, 33ul>
    >;
        
    BOOST_TEST(3 == find_if_noctor_mp11<sequence_type>(is_unsigned_ {}, return_value {}).first);
}

struct get : boost::static_visitor<int>
{
    get() = default;

    template <typename T>
    auto operator ()(T) const noexcept -> int { return T::value; }
};

BOOST_AUTO_TEST_CASE(at_mp11_)
{
    using numbers = boost::mp11::mp_list_c<int, 0, 1, 2, 3, 4, 5, 6, 7>;
    BOOST_TEST(at_mp11<numbers>(3, get{}) == 3);
}

namespace
{

struct switch_fn
{
    template <std::size_t Idx_>
    constexpr auto operator ()(std::integral_constant<std::size_t, Idx_>, std::size_t a, std::size_t b) const
        -> std::ptrdiff_t
    {
        return (Idx_ + a) * b;
    }

    constexpr auto operator ()(std::size_t idx, std::size_t /*a*/, std::size_t /*b*/) const
        -> std::ptrdiff_t
    {
        return idx;
    }
};

}

BOOST_AUTO_TEST_CASE(unordered_switch__)
{
    static constexpr auto call_switch = [](std::size_t idx, std::size_t a, std::size_t b)
    {
        return unordered_switch_
        (
              idx
            , std::index_sequence<0, 4, 2, 15>{}
            , switch_fn{}
            , switch_fn{}
            , a
            , b
        );
    };

    BOOST_TEST(call_switch(0, 1, 2) == 2);
    BOOST_TEST(call_switch(4, 1, 2) == 10);
    BOOST_TEST(call_switch(2, 4, 2) == 12);
    BOOST_TEST(call_switch(14, 3, 2) == 14);
}

BOOST_AUTO_TEST_CASE(ordered_switch__)
{
    static constexpr auto call_switch = [](std::size_t idx, std::size_t a, std::size_t b)
    {
        return ordered_switch_
        (
              idx
            , std::index_sequence<0, 2, 4, 15>{}
            , switch_fn{}
            , switch_fn{}
            , a
            , b
        );
    };

    BOOST_TEST(call_switch(0, 1, 2) == 2);
    BOOST_TEST(call_switch(4, 1, 2) == 10);
    BOOST_TEST(call_switch(2, 4, 2) == 12);
    BOOST_TEST(call_switch(14, 3, 2) == 14);
}

BOOST_AUTO_TEST_CASE(sequenced_switch__)
{
    static constexpr auto call_switch = [](std::size_t idx, std::size_t a, std::size_t b)
    {
        return sequenced_switch_
        (
            idx
            , std::index_sequence<0, 2, 4, 6, 8>{}
            , switch_fn{}
            , switch_fn{}
            , a
            , b
        );
    };

    BOOST_TEST(call_switch(0, 1, 2) == 2);
    BOOST_TEST(call_switch(4, 1, 2) == 10);
    BOOST_TEST(call_switch(6, 4, 2) == 20);
    BOOST_TEST(call_switch(14, 3, 2) == 14);
}

BOOST_AUTO_TEST_CASE(switch__)
{
    static constexpr auto call_seq_switch = [](std::size_t idx, std::size_t a, std::size_t b)
    {
        return switch_
        (
              idx
            , std::index_sequence<0, 2, 4, 6, 8>{}
            , switch_fn{}
            , switch_fn{}
            , a
            , b
        );
    };

    BOOST_TEST(call_seq_switch(0, 1, 2) == 2);
    BOOST_TEST(call_seq_switch(4, 1, 2) == 10);
    BOOST_TEST(call_seq_switch(6, 4, 2) == 20);
    BOOST_TEST(call_seq_switch(14, 3, 2) == 14);

    static constexpr auto call_ord_switch = [](std::size_t idx, std::size_t a, std::size_t b)
    {
        return switch_
        (
              idx
            , std::index_sequence<0, 2, 4, 15>{}
            , switch_fn{}
            , switch_fn{}
            , a
            , b
        );
    };

    BOOST_TEST(call_ord_switch(0, 1, 2) == 2);
    BOOST_TEST(call_ord_switch(4, 1, 2) == 10);
    BOOST_TEST(call_ord_switch(2, 4, 2) == 12);
    BOOST_TEST(call_ord_switch(14, 3, 2) == 14);

    static constexpr auto call_unord_switch = [](std::size_t idx, std::size_t a, std::size_t b)
    {
        return switch_
        (
              idx
            , std::index_sequence<0, 4, 2, 15>{}
            , switch_fn{}
            , switch_fn{}
            , a
            , b
        );
    };

    BOOST_TEST(call_unord_switch(0, 1, 2) == 2);
    BOOST_TEST(call_unord_switch(4, 1, 2) == 10);
    BOOST_TEST(call_unord_switch(2, 4, 2) == 12);
    BOOST_TEST(call_unord_switch(14, 3, 2) == 14);
}

BOOST_AUTO_TEST_SUITE_END() // algorithm
BOOST_AUTO_TEST_SUITE_END() // cmn