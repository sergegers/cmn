#pragma once

#include <type_traits>
#include <new>
#include <utility>

#include <boost/type_traits/promote.hpp>
#include <boost/mp11.hpp>

#include <cmn/tuple/va/concepts.h>

namespace cmn::va::detail
{

using namespace boost::mp11;

//-----------------------------------------------------------------------------
//
// implementation dependent
//
template <c::va_arg_ VaArg>
constexpr std::size_t va_slot_size_in_bytes_v = sizeof(std::intptr_t);

template <c::va_arg_... VaArgs>
constexpr std::size_t va_arg_offset_in_bytes_v = (va_slot_size_in_bytes_v<VaArgs> + ... + 0ul);

//-----------------------------------------------------------------------------
//
// implementation dependent
//
template <c::va_arg_ VaArg>
using keep_type_t = std::conditional_t
<
      c::ref_va_arg<VaArg>
    , std::add_pointer_t<boost::promote_t<VaArg>>
    , boost::promote_t<VaArg>
>;

///////////////////////////////////////////////////////////////////////////////
//
// get() by index overloads
//
///////////////////////////////////////////////////////////////////////////////

// shortcuts
template <typename Tpl> using naked_ = std::remove_cvref_t<Tpl>;
template <std::size_t Idx_, typename Tpl> using res_ = typename naked_<Tpl>::template res_t<Idx_, Tpl>;
template <std::size_t Idx_, typename Tpl> using arg_ = typename naked_<Tpl>::template arg_t<Idx_>;

// named arguments
template <std::size_t Idx_, typename Tpl>
constexpr auto get(Tpl &&tpl) -> res_<Idx_, Tpl>
    requires (Idx_ < naked_<Tpl>::named_args_size)
{
    return std::get<Idx_>(std::forward_like<Tpl>(tpl.named_args()));
}

// promoted small arguments
template <std::size_t Idx_, typename Tpl> 
constexpr auto get(Tpl &&tpl) noexcept -> std::remove_reference_t<res_<Idx_, Tpl>>
    requires (Idx_ >= naked_<Tpl>::named_args_size)
    && !c::ref_va_arg<arg_<Idx_, Tpl>>
    && c::promoted_va_arg<arg_<Idx_, Tpl>>
{
    using result_type = std::remove_reference_t<res_<Idx_, Tpl>>;
    return static_cast<result_type>(*std::launder(std::forward<Tpl>(tpl).template slot<Idx_>()));
}

// non promoted small arguments
template <std::size_t Idx_, typename Tpl> 
constexpr auto get(Tpl &&tpl) noexcept -> res_<Idx_, Tpl>
    requires (Idx_ >= naked_<Tpl>::named_args_size)
    && !c::ref_va_arg<arg_<Idx_, Tpl>>
    && !c::promoted_va_arg<arg_<Idx_, Tpl>>
{
    return *std::launder(std::forward<Tpl>(tpl).template slot<Idx_>());
}

// big arguments
template <std::size_t Idx_, typename Tpl>
constexpr auto get(Tpl &&tpl) noexcept -> res_<Idx_, Tpl>
    requires (Idx_ >= naked_<Tpl>::named_args_size)
 && c::ref_va_arg<arg_<Idx_, Tpl>>
{
    return *std::launder(*std::forward<Tpl>(tpl).template slot<Idx_>());
}
//
///////////////////////////////////////////////////////////////////////////////
//
// put() by index overloads
// 
///////////////////////////////////////////////////////////////////////////////

// named arguments
template <std::size_t Idx_, typename Tpl, typename Arg>
    requires (Idx_ < naked_<Tpl>::named_args_size)
auto constexpr put(Tpl &tpl, Arg &&arg) noexcept -> void
{
    std::get<Idx_>(std::forward_like<Tpl>(tpl.named_args())) = std::forward<Arg>(arg);
}

// small arguments
template <std::size_t Idx_, typename Tpl, typename Arg> 
constexpr auto put(Tpl &tpl, Arg &&arg) noexcept -> void
    requires (Idx_ >= naked_<Tpl>::named_args_size)
    && !c::ref_va_arg<arg_<Idx_, Tpl>>
{
    auto &slot_ = *std::launder(std::forward<Tpl>(tpl).template slot<Idx_>());
    using slot_type = naked_<decltype(slot_)>;
    slot_.~slot_type();

    slot_ = static_cast<slot_type>(std::forward<Arg>(arg));
}

// big arguments
template <std::size_t Idx_, typename Tpl, typename Arg> 
constexpr auto put(Tpl &tpl, Arg &&arg) noexcept -> void
    requires (Idx_ >= naked_<Tpl>::named_args_size)
    && c::ref_va_arg<arg_<Idx_, Tpl>>
{
    auto &big_arg_slot_ = tpl.template big_arg_slot<Idx_>();
    big_arg_slot_ = std::forward<Arg>(arg);
    std::forward<Tpl>(tpl).template slot<Idx_>() = &big_arg_slot_;
}

//
///////////////////////////////////////////////////////////////////////////////

template <typename T> using big_arg_ = mp_bool<c::ref_va_arg<T>>;
template <typename VaIdx, typename VaArg> using big_arg_idx_ = mp_bool<c::ref_va_arg<VaArg>>;

template <typename Tpl> using va_args_ = typename Tpl::va_args_type;

template <typename Tpl>
using va_args_indices_ = mp_from_sequence<std::make_index_sequence<mp_size<va_args_<Tpl>>::value>>;

template <typename Tpl>
using big_slot_idss_ = mp_filter<big_arg_idx_, va_args_indices_<Tpl>, va_args_<Tpl>>;

//-----------------------------------------------------------------------------
//
// exposed
//
template <typename Tpl> using big_arg_buffer_t = mp_copy_if<va_args_<Tpl>, big_arg_>;

template <typename Tpl, std::size_t Idx_>
static constexpr std::size_t big_slot_idx_v = mp_find<big_slot_idss_<Tpl>, mp_size_t<Idx_ - Tpl::named_args_size>>::value;


}
