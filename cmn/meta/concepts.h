#pragma once

#include <concepts>
#include <type_traits>

#include "promote.h"

namespace cmn
{

namespace c
{

////////////////////////////////////////////////////////////////////////////////
//
// republish some type traits as concepts
//
////////////////////////////////////////////////////////////////////////////////
template <typename T>
concept enum_ = std::is_enum_v<T>;

template <typename T>
concept scoped_enum = enum_<T> && std::is_scoped_enum_v<T>;

template <typename T>
concept noscoped_enum = enum_<T> && !std::is_scoped_enum_v<T>;
//-----------------------------------------------------------------------------
template <typename T>
concept enumerable = std::integral<T> || std::is_enum_v<T>;

}

///////////////////////////////////////////////////////////////////////////////
template <typename T>
struct mask_type : std::make_unsigned<T> {};

template <typename T> requires std::integral<T>
struct mask_type<T> : int_promotion<T> {};

template <typename T> requires c::enum_<T>
struct mask_type<T> : int_promotion<std::underlying_type_t<T>> {};

template <typename T> requires c::scoped_enum<T>
struct mask_type<T> : std::make_unsigned<std::underlying_type_t<T>> {};

template <c::enumerable T>
using mask_type_t = typename mask_type<T>::type;


namespace c
{

///////////////////////////////////////////////////////////////////////////////
template <typename Lhs, typename Rhs, typename Res>
concept bit_ops_ = requires(Lhs lhs, Rhs rhs)
{
    { lhs & rhs } noexcept -> std::same_as<Res>;
    { lhs | rhs } noexcept -> std::same_as<Res>;
    { lhs ^ rhs } noexcept -> std::same_as<Res>;
};

template <typename T, typename Res>
concept not_op_ = requires(T t)
{
    { ~t } noexcept -> std::same_as<Res>;
};

template <typename T, typename Res>
concept bit_n_ops_ = bit_ops_<T, T, Res> && not_op_<T, Res>;

//-----------------------------------------------------------------------------
template <typename T>
concept strong_bitfield = 
    enumerable<T>
 && std::equality_comparable<T>
 && bit_ops_<T, T, T>
;
//-----------------------------------------------------------------------------
template <typename T>
concept bitfield = 
    enumerable<T>
 && std::equality_comparable<T>
 && 
 (
        // for scoped bitfields
        bit_n_ops_<T, T> 
     && bit_ops_<T, mask_type_t<T>, T>
    ||
    std::equality_comparable_with<T, mask_type_t<T>>
     &&
     (
            // for integral types & C enums with implicit conversion to int types
           bit_n_ops_<T, int_promotion_t<T>>
        && bit_ops_<T, mask_type_t<T>, mask_type_t<T>>
      ||
            // for nonscoped enums with overloaded operators
           bit_n_ops_<T, T>
        && bit_ops_<T, mask_type_t<T>, mask_type_t<T>>
    )  
 )
;

}

///////////////////////////////////////////////////////////////////////////////
//
// ext enum concepts
//
///////////////////////////////////////////////////////////////////////////////
namespace enum_
{

enum class kind_t
{
    naive,  // enum w/o adaptation
    enum_,
    bitfield,
    combo
};

//-----------------------------------------------------------------------------
template <c::enum_ Enum, bool = std::is_enum_v<Enum>>
struct traits;

//-----------------------------------------------------------------------------
template <typename T>
struct mask_type : std::make_unsigned<T> {};

template <typename T> requires std::integral<T>
struct mask_type<T> : int_promotion<T> {};

template <typename T> requires c::enum_<T>
struct mask_type<T> : int_promotion<std::underlying_type_t<T>> {};

template <typename T> requires c::scoped_enum<T>
struct mask_type<T> : std::make_unsigned<std::underlying_type_t<T>> {};

template <c::enumerable T>
using mask_type_t = typename mask_type<T>::type;

}

namespace c
{

template <typename Enum>
concept e_naive = enum_::traits<Enum>::kind == enum_::kind_t::naive;

template <typename Enum>
concept e_enum = enum_::traits<Enum>::kind == enum_::kind_t::enum_;

template <typename Enum>
concept e_bitfield = enum_::traits<Enum>::kind == enum_::kind_t::bitfield;

template <typename Enum>
concept e_combo = enum_::traits<Enum>::kind == enum_::kind_t::combo;

template <typename Enum>
concept e_any_enum = e_enum<Enum> || e_bitfield<Enum> || e_combo<Enum>;

}

}
