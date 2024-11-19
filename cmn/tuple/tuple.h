#pragma once

#include <tuple>
#include <utility>
#include <type_traits>

#include <cmn/util/util.h>

namespace cmn
{

////////////////////////////////////////////////////////////////////////////////
// tuple_cut_right(), tuple_cut_left(), tuple_cut()
// make tuple from the another one by trimming their edges
// http://stackoverflow.com/questions/17854219/creating-a-sub-tuple-starting-from-a-stdtuplesome-types
////////////////////////////////////////////////////////////////////////////////

template <typename Tuple, std::size_t... Idx_>
constexpr auto tuple_shuffle(Tuple &&t, std::index_sequence<Idx_...>)
{
    // extension point
    using std::get;
    return std::tuple{ get<Idx_>(std::forward<Tuple>(t))... };
}

template <std::size_t LIdx_, typename Tuple>
constexpr auto tuple_cut_left(Tuple &&t)
{
    return tuple_shuffle
    (
        std::forward<Tuple>(t),
        make_index_sequence<LIdx_, std::tuple_size_v<std::decay_t<Tuple>>>{}
    );
}

template <typename Tuple>
constexpr auto tuple_cut_left(Tuple &&t)
{
    return tuple_cut_left<1>(std::forward<Tuple>(t));
}

template <std::size_t RIdx_, typename Tuple>
constexpr auto tuple_cut_right(Tuple &&t)
{
    return tuple_shuffle
    (
        std::forward<Tuple>(t),
        make_index_sequence<0, RIdx_>{}
    );
}

template <typename Tuple>
constexpr auto tuple_cut_right(Tuple &&t)
{
    constexpr auto ridx = std::tuple_size_v<std::decay_t<Tuple>> - 1;
    return tuple_cut_right<ridx>(std::forward<Tuple>(t));
}

template <std::size_t LIdx_, std::size_t RIdx_, typename Tuple>
constexpr auto tuple_cut(Tuple &&t)
{
    return tuple_shuffle
    (
        std::forward<Tuple>(t),
        make_index_sequence<LIdx_, RIdx_>{}
    );
}

////////////////////////////////////////////////////////////////////////////////
//
// tuple_swap()
//
////////////////////////////////////////////////////////////////////////////////
template <std::size_t P_, std::size_t Q_, typename Tuple>
constexpr auto tuple_swap(Tuple &&t)
{
    return tuple_shuffle
    (
        std::forward<Tuple>(t),
        index_swap_t<std::make_index_sequence<std::tuple_size_v<std::decay_t<Tuple>>>, P_, Q_>{}
    );
}

}   
