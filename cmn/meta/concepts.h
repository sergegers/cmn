#pragma once

#include <concepts>
#include <string>
#include <type_traits>
#include <iosfwd>
#include <exception>

#include <boost/mp11.hpp>
#include <boost/mpl/pair.hpp>
#include <boost/type_traits/promote.hpp>
#include <boost/exception/all.hpp>

#if __has_include(<boost/mp11/concepts.hpp>) && __has_include(<boost/fusion/concepts.hpp>)

#include <boost/mp11/concepts.hpp>
#include <boost/fusion/concepts.hpp>

#else

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

#endif

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
concept pointer = std::is_pointer_v<T>;

template <typename T>
concept dereferencable =  requires (T &t)
{
    { *t };
};

//-----------------------------------------------------------------------------
//
// also support smart pointers
//
// https://stackoverflow.com/a/78595795/8452129
//-----------------------------------------------------------------------------
template <typename T>
concept pointer_like = 
    pointer<T>
 ||
(
    dereferencable<T>
 && requires (T t)
 {
    { static_cast<bool>(t) };
    { t.operator -> () } -> std::convertible_to<decltype( &*t )>;
 }
);

//-----------------------------------------------------------------------------
template <typename T>
concept class_ = std::is_class_v<T>;

template <typename T>
concept function = std::is_function_v<T>;

template <typename T>
concept polymorphic = std::is_polymorphic_v<T>;

template <typename T>
concept enum_ = std::is_enum_v<T>;

template <typename T>
concept scoped_enum = enum_<T> && std::is_scoped_enum_v<T>;

template <typename T>
concept noscoped_enum = enum_<T> && !std::is_scoped_enum_v<T>;
//-----------------------------------------------------------------------------
template <typename T>
concept enumerable = std::integral<T> || std::is_enum_v<T>;


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

template <typename T, template <typename...> typename TemplateT>
concept instance_of = detail::is_instance_of_<TemplateT, T>::value;

///////////////////////////////////////////////////////////////////////////////
//
// Integral constant concepts
//
///////////////////////////////////////////////////////////////////////////////
namespace detail
{

template <std::integral I, typename T>
struct instance_of_integral_const: std::false_type {};

template <std::integral I, I I_, template <typename, auto> typename IntConstntT>
struct instance_of_integral_const<I, IntConstntT<I, I_>>: std::true_type {};

}

template <typename T>
concept instance_of_bool = detail::instance_of_integral_const<bool, T>::value;

//-----------------------------------------------------------------------------
//
// check if type is complete
// https://stackoverflow.com/a/53298134/8452129
//
//-----------------------------------------------------------------------------
namespace detail
{

template <typename T, typename Enabler = void>
struct is_complete : std::false_type {};

template <typename T>
struct is_complete<T, std::void_t<decltype(sizeof(T) != 0)>> : std::true_type {};

}

template <typename T>
concept complete = detail::is_complete<T>::value;

}

///////////////////////////////////////////////////////////////////////////////
//
// ext enum concepts
//
///////////////////////////////////////////////////////////////////////////////
namespace enum_
{

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
namespace detail
{

template <typename Lhs, typename Rhs, typename Res>
concept bit_ops_ = requires(Lhs lhs, Rhs rhs)
{
    { lhs & rhs } noexcept -> std::same_as<Res>;
    { lhs | rhs } noexcept -> std::same_as<Res>;
    { lhs ^ rhs } noexcept -> std::same_as<Res>;
    { ~lhs } noexcept -> std::same_as<Res>;
};

template <typename Lhs, typename Rhs, typename Res>
concept commutative_bit_ops_ = 
    bit_ops_<Lhs, Rhs, Res>
 && bit_ops_<Rhs, Lhs, Res>
;

}

//-----------------------------------------------------------------------------
//
// for scoped bitfields
//
template <typename T>
concept strong_bitfield = 
    enumerable<T>
 && std::equality_comparable<T>
 && detail::bit_ops_<T, T, T>
;

//-----------------------------------------------------------------------------
//
// for any bitfields
//
template <typename T>
concept bitfield =
    strong_bitfield<T>
 ||
 (
        enumerable<T>
     && std::equality_comparable<T>
     && std::equality_comparable_with<T, enum_::mask_type_t<T>>
     &&
     (
            // for integral types & C enums with implicit conversion to int types
           detail::commutative_bit_ops_<T, enum_::mask_type_t<T>, enum_::mask_type_t<T>>
            // for integral types & scoped enums with overloaded operators
        || detail::commutative_bit_ops_<T, enum_::mask_type_t<T>, T>
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

//-----------------------------------------------------------------------------
template <typename T, typename Char, typename CharTraits>
concept basic_string = requires (T const &ct, std::size_t idx)
{
    { ct.c_str() } -> std::same_as<Char const *>;
    { ct[idx] } -> std::same_as<Char const &>;
};

template <typename T> concept string = basic_string<T, char, std::char_traits<char>>;
template <typename T> concept wstring = basic_string<T, wchar_t, std::char_traits<wchar_t>>;

//-----------------------------------------------------------------------------
template <typename T, typename Char, typename CharTraits>
concept printable = requires (std::basic_ostream<Char, CharTraits> &ostr, T const &t)
{
    { ostr << t } -> std::same_as<decltype(ostr)>;
};

//-----------------------------------------------------------------------------
template <typename T>
concept predicate = requires (T const &t)
{
    { t } -> std::convertible_to<bool>;
    { !t } -> std::same_as<bool>;
};

//-----------------------------------------------------------------------------
//
// for throwing destructors in test mode only
//
#ifdef CMN_TEST_MOCK

template <typename T, typename... Args>
concept test_constructible_from = requires (Args &&... args)
{
    T{ std::forward<Args>(args)... };
};
#else

//-----------------------------------------------------------------------------
template <typename T, typename... Args>
concept test_constructible_from = std::constructible_from<T, Args...>;

#endif
//-----------------------------------------------------------------------------
//
// for one argument & inverted argument order
//
template <typename Arg, typename T>
concept test_constructible = test_constructible_from<T, Arg>;

//-----------------------------------------------------------------------------

#ifdef CMN_TEST_MOCK
template <typename T> concept test_move_constuctible = test_constructible_from<T, T &&>;
#else
template <typename T> concept test_move_constuctible = std::move_constructible<T>;
#endif

///////////////////////////////////////////////////////////////////////////////
//
// exception concepts
//
template <typename E> concept exception =
    std::derived_from<E, std::exception>
 && std::derived_from<E, boost::exception>
;

template <typename T> concept error_info = instance_of<T, boost::error_info>;

template <typename T>
concept error_info_pair =
    instance_of<T, boost::mpl::pair>
 && error_info<typename T::first>
 && error_info<typename T::second>
;

namespace detail
{

template <typename T>
struct is_error_info_map_: std::false_type {};

template <error_info_pair... ErrorInfoPairs>
struct is_error_info_map_<boost::mp11::mp_list<ErrorInfoPairs...>>: std::true_type {};

//-----------------------------------------------------------------------------
template <typename T>
struct is_error_info_list_: std::false_type {};

template <error_info... ErrorInfos>
struct is_error_info_list_<boost::mp11::mp_list<ErrorInfos...>>: std::true_type {};

//-----------------------------------------------------------------------------
template <boost::c::mp11_list L>
constexpr bool mp_is_unique_v = std::is_same_v<L, boost::mp11::mp_unique<L>>;

}

///////////////////////////////////////////////////////////////////////////////
template <typename T>
concept error_info_map =
    detail::is_error_info_map_<T>::value
 && detail::mp_is_unique_v<boost::mp11::mp_map_keys<T>>
;

template <typename T>
concept error_info_list =
    detail::is_error_info_list_<T>::value
 && detail::mp_is_unique_v<T>
;

//
///////////////////////////////////////////////////////////////////////////////

}

}
