#pragma once

#include <cmn/meta/concepts.h>
#include <cmn/tuple/va/tuple_view.h>

namespace boost
{

namespace fusion
{

struct fusion_sequence_tag;

}

namespace mpl
{

template <typename>
struct sequence_tag;

template <typename... EArgs, typename Res>
struct sequence_tag<cmn::va::tuple_view<auto (EArgs...) -> Res>>
{
    using type = fusion::fusion_sequence_tag;
};

template <typename... EArgs, typename Res>
struct sequence_tag<cmn::va::tuple_view<auto (EArgs...) -> Res> const>
{
    using type = fusion::fusion_sequence_tag;
};

//template <typename... EArgs, typename Res>
//struct sequence_tag<cmn::va::tuple<auto (EArgs...) -> Res>>
//{
//    using type = fusion::fusion_sequence_tag;
//};
//
//template <typename... EArgs, typename Res>
//struct sequence_tag<cmn::va::tuple<auto (EArgs...) -> Res> const>
//{
//    using type = fusion::fusion_sequence_tag;
//};

}   

}   

