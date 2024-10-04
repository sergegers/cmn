#pragma once

#include <concepts>
#include <type_traits>

#include <boost/mp11.hpp>
#include <boost/type_traits/promote.hpp>

#include <boost/fusion/support/is_sequence.hpp>
#include <boost/fusion/support/category_of.hpp>

namespace boost::c
{

template <typename L> concept mp11_list = mp11::mp_is_list<L>::value;
template <typename S> concept mp11_set = mp11::mp_is_set<S>::value;
template <typename M> concept mp11_map = mp11::mp_is_map<M>::value;

///////////////////////////////////////////////////////////////////////////////
template <typename S>
concept fus_sequence = fusion::traits::is_sequence<S>::value;

template <typename S>
concept random_access_fus_sequence =
       fus_sequence<S> && fusion::traits::is_random_access<S>::value;

}

namespace cmn
{

namespace io
{

enum class int_fmt_t: short;

//-----------------------------------------------------------------------------
//
// traits
//
template <typename Unit>
struct strong_typedef_fmt_traits
{
    static constexpr bool enable_luxury_io = false;
    static constexpr auto default_ = static_cast<int_fmt_t>(2665);  // int_fmt_t::default_
};

}

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

template <typename U, typename V>
concept int_convertible_to =
    std::integral<U>
 && std::integral<V>
 && std::convertible_to<U, V>
;

}

///////////////////////////////////////////////////////////////////////////////
//
// instance_of
//
// https://cukic.co/2019/03/15/template-meta-functions-for-detecting-template-instantiation/
//
///////////////////////////////////////////////////////////////////////////////
namespace detail
{

template <template <typename...> typename TemplateT, typename T>
struct is_instance_of_: std::false_type {};

template <template <typename...> typename TemplateT, typename... Args>
struct is_instance_of_<TemplateT, TemplateT<Args...>>: std::true_type {};

}

namespace c
{

template <typename T, template <typename...> typename TemplateT>
concept instance_of = detail::is_instance_of_<TemplateT, T>::value;

}


///////////////////////////////////////////////////////////////////////////////
template <typename T>
struct mask_type : std::make_unsigned<T> {};

template <typename T> requires std::integral<T>
struct mask_type<T> : boost::promote<T> {};

template <typename T> requires c::enum_<T>
struct mask_type<T> : boost::promote<std::underlying_type_t<T>> {};

template <typename T> requires c::scoped_enum<T>
struct mask_type<T> : std::make_unsigned<std::underlying_type_t<T>> {};

template <c::enumerable T>
using mask_type_t = typename mask_type<T>::type;

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

template <kind_t Kind_>
using kkind_t = std::integral_constant<kind_t, Kind_>;

//-----------------------------------------------------------------------------
enum op_t
{
    op_empty           = 0x00,
    op_bitwise         = 0x01,
    op_steppable       = 0x02,
    op_comparable      = 0x04,
    op_ariphmetic      = 0x08,
    op_io              = 0x10,
    op_interoperable   = 0x20
};

///////////////////////////////////////////////////////////////////////////////
//
// Print options
//
///////////////////////////////////////////////////////////////////////////////
namespace io
{

enum class print_t
{
      empty         = 0x0
    , tail          = 0x1
    , ns            = 0x2
    , class_prefix  = 0x4
};

}

//-----------------------------------------------------------------------------
template <c::enum_ Enum, typename = void>
struct traits;

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
           bit_n_ops_<T, boost::promote_t<T>>
        && bit_ops_<T, mask_type_t<T>, mask_type_t<T>>
      ||
            // for nonscoped enums with overloaded operators
           bit_n_ops_<T, T>
        && bit_ops_<T, mask_type_t<T>, mask_type_t<T>>
    )  
 )
;

}

////////////////////////////////////////////////////////////////////////////////
struct strong_typedef_tag {};

namespace c
{

template <typename T>
concept unit = 
    std::is_base_of_v<strong_typedef_tag, T>
 && requires
    {
        typename T::underlying_type;
    }
 && std::constructible_from<typename T::underlying_type>
;

template <typename T>
concept fmt_unit =
    unit<T>
 && io::strong_typedef_fmt_traits<T>::enable_luxury_io
;

}

}
