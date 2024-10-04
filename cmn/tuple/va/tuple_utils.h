#pragma once

#include <tuple>
#include <cstdarg>
#include <climits>

#include <boost/mp11/algorithm.hpp>

#include <cmn/tuple/va/detail/promote.h>

namespace cmn
{

struct ellipsis;

namespace va
{
class tuple_tag {};
 
struct from_va_list_t {};
constexpr from_va_list_t from_va_list;

//-----------------------------------------------------------------------------
template <typename Arg, bool EllipsisArg_>
constexpr auto slot_size() -> std::size_t
{
    // see <vadefs.h>
    return sizeof(void *);

    // TODO:
    //if constexpr (sizeof(void *) == 64 / CHAR_BIT)
    //    return sizeof(void *);
    //else
    //{
    //    // see define __crt_va_arg(ap, t)     (*(t*)((ap += _INTSIZEOF(t)) - _INTSIZEOF(t)))
    //    using keep_type = keep_type_t<Arg, EllipsisArg_>;
    //    return _INTSIZEOF(keep_type);
    //}
}

////////////////////////////////////////////////////////////////////////////////
//
// tuple_ellipsis_index()
//
// returns ellipsis index or -1 if the ellipsis is absent
//
////////////////////////////////////////////////////////////////////////////////
template <c::eva_argument... EArgs>
constexpr auto tuple_ellipsis_index() -> std::ptrdiff_t
{
    return [] <typename ArgTpl, std::size_t... Indices_>(std::index_sequence<Indices_...> idss)
        -> std::ptrdiff_t
    {
        return
        (
            (idss.size() - 1) + ... + 
                (std::is_same_v<std::tuple_element_t<Indices_, ArgTpl>, ellipsis>? Indices_: -1)
        );
    }
    .template operator()<std::tuple<EArgs... >>(std::make_index_sequence<sizeof... (EArgs)>{});
};

//-----------------------------------------------------------------------------
//
// https://stackoverflow.com/a/23863962/8452129
//
template<typename...Ts>
using tuple_cat_t = decltype(std::tuple_cat(std::declval<Ts>()...));

// arguments w/o ellipsis
template <c::eva_argument... EArgs>
using tuple_args_t = tuple_cat_t
    <
        std::conditional_t
        <
              std::is_same_v<EArgs, ellipsis>
            , std::tuple<>
            , std::tuple<EArgs>
        >...
    >
;

//-----------------------------------------------------------------------------
template <std::ptrdiff_t EllipsisIdx_, typename... PromotedArgs, std::size_t... EllipsisIndices_>
constexpr auto tuple_offset_impl
(
    std::tuple<PromotedArgs...>,
    std::index_sequence<EllipsisIndices_...>
) noexcept
    -> std::size_t
{
    if constexpr (EllipsisIdx_ == -1)
        return (0ul + ... + slot_size<PromotedArgs, false>());
    else
        return (0ul + ... + slot_size<PromotedArgs, EllipsisIndices_ >= EllipsisIdx_>());
};

template <std::size_t Idx_, c::eva_argument... EArgs>
constexpr auto tuple_offset() noexcept -> std::size_t
{
    using promoted_args_type = tuple_args_t<EArgs...>;
    using args_type = boost::mp11::mp_erase_c<promoted_args_type, Idx_, std::tuple_size_v<promoted_args_type>>;

    return 
        tuple_offset_impl<tuple_ellipsis_index<EArgs...>()>
        (
            args_type {}, 
            std::make_index_sequence<std::tuple_size_v<args_type>>{}
        );
};

//-----------------------------------------------------------------------------
template <c::eva_argument... EArgs>
constexpr auto tuple_size_in_bytes() noexcept -> std::size_t
{
    using args_type = tuple_args_t<EArgs...>;

    return 
        tuple_offset_impl<tuple_ellipsis_index<EArgs...>()>
        (
            args_type {},
            std::make_index_sequence<std::tuple_size_v<args_type>>{}
        );
};

////////////////////////////////////////////////////////////////////////////////
//
// tuple_element
// metafunction returns element type
//
////////////////////////////////////////////////////////////////////////////////
template <std::size_t Idx_, c::eva_argument... EArgs>
struct tuple_element
{
    using args_type = tuple_args_t<EArgs...>;
    static_assert(Idx_ < std::tuple_size_v<args_type>, "Index is out of bounds.");

    using type = std::tuple_element_t<Idx_, args_type>;
};

template <std::size_t Idx_, c::eva_argument... EArgs>
using tuple_element_t = typename tuple_element<Idx_, EArgs...>::type;

////////////////////////////////////////////////////////////////////////////////
//
// va_tuple_keep_element
// properly process promotions
//
////////////////////////////////////////////////////////////////////////////////
template <std::size_t Idx_, c::eva_argument... EArgs>
using tuple_keep_element = 
    keep_type
    <
          tuple_element_t<Idx_, EArgs...>
          // if ellipsis index == -1 comparison is always false
        , Idx_ >= static_cast<std::size_t>(tuple_ellipsis_index<EArgs...>())
    >
;

template <std::size_t Idx_, c::eva_argument... EArgs>
using tuple_keep_element_t = typename tuple_keep_element<Idx_, EArgs...>::type;

}

}