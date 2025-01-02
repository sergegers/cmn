
#include <string>
#include <tuple>
#include <utility>
#include <type_traits>

#include <boost/mp11.hpp>

#include <cmn/tuple/tuple.h>

#ifdef CMN_STATIC_TEST

namespace cmn
{

static_assert
(
    std::is_same_v
    <                                          // int, unsigned int, long, float, long double
        decltype(tuple_cut_left<1>(std::tuple{0, 1U, 2L, 3.F, 4.L })), 
        std::tuple<unsigned int, long, float, long double>
    >,
    "tuple_cut_left failed"
);

static_assert
(
    std::is_same_v
    <
    decltype(tuple_cut_right<3>(std::tuple{ 0, 1U, 2L, 3.F, 4.L })), 
        std::tuple<int, unsigned int, long> 
    >,
    "tuple_cut_right failed"
);

static_assert
(
    std::is_same_v
    <
    decltype(tuple_cut<1, 3>(std::tuple{0, 1U, 2L, 3.F, 4.L })), 
        std::tuple<unsigned int, long>
    >,
    "tuple_cut failed"
);

///////////////////////////////////////////////////////////////////////////////
//
// NOTE: tuple algorithms were removed & replaced boost.mp11 ones
//
static_assert
(
    std::is_same_v
    <
        boost::mp11::mp_erase_c<std::tuple<int, std::string, std::pair<std::wstring, float>>, 1, 2>,
        std::tuple<int, std::pair<std::wstring, float>>
    >
);

static_assert
(
    std::is_same_v
    <
        boost::mp11::mp_erase_c<std::tuple<int, std::string, std::pair<std::wstring, float>>, 0, 3>,
        std::tuple<>
    >
);

static_assert
(
    std::is_same_v
    <
        std::tuple<int, float, double>,
        boost::mp11::mp_push_back<std::tuple<int, float>, double>
    >
);

static_assert
(
    std::is_same_v
    <
        std::tuple<double, int, float>,
        boost::mp11::mp_push_front<std::tuple<int, float>, double>
    >
);


static_assert
(
    std::is_same_v
    <
        boost::mp11::mp_pop_back<std::tuple<int, float, double>>, 
        std::tuple<int, float>
    >
);

static_assert
(
    std::is_same_v
    <
        boost::mp11::mp_erase_c<std::tuple<int, float, double>, 1, 2>, 
        std::tuple<int, double>
    >
);


static_assert
(
    std::is_same_v
    <
        boost::mp11::mp_erase_c<std::tuple<int, float, double>, 1, 3>, 
        std::tuple<int>
    >
);

static_assert
(
    std::is_same_v
    <
        boost::mp11::mp_insert_c<std::tuple<int, float, double>, 2, std::string>, 
        std::tuple<int, float, std::string, double>
    >
);

static_assert
(
    std::is_same_v
    <
        boost::mp11::mp_append<std::tuple<int, std::wstring>, std::tuple<char, std::ostream &>>,
        std::tuple<int, std::wstring, char, std::ostream &>
    >
);

static_assert
(
    std::is_same_v
    <
        boost::mp11::mp_push_back<std::tuple<int, std::wstring>, char, std::ostream &>,
        std::tuple<int, std::wstring, char, std::ostream &>
    >
);

static_assert(boost::mp11::mp_find<std::tuple<int, char const, long>, int>::value == 0, "tuple_index failed");
static_assert(boost::mp11::mp_find<std::tuple<int, char const, long>, char const>::value == 1, "tuple_index failed");
static_assert(boost::mp11::mp_find<std::tuple<int, char const, long>, long>::value == 2, "tuple_index failed");
static_assert(boost::mp11::mp_find<std::tuple<int, char const, long>, float>::value == 3, "tuple_index failed");

} 

#endif