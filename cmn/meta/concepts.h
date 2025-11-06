#pragma once

#include <cstddef>
#include <concepts>
#include <type_traits>
#include <string>
#include <iosfwd>
#include <exception>
#include <utility>
#include <ios>
#include <format>
#include <utility>

#include <boost/mp11.hpp>
#include <boost/mpl/pair.hpp>
#include <boost/type_traits/promote.hpp>
#include <boost/exception/all.hpp>

#if __has_include(<boost/mp11/concepts.hpp>)
#   include <boost/mp11/concepts.hpp>
#else
#   include <cmn/meta/boost/mp11/concepts.hpp>
#endif

#include <cmn/fwd.h>

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
concept pointer = std::is_pointer_v<T>;

template <typename T>
concept dereferenceable =  requires (T &t)
{
    { *t };
};

//-----------------------------------------------------------------------------
//
// add smart pointer support
//
// https://stackoverflow.com/a/78595795/8452129
//-----------------------------------------------------------------------------
template <typename T>
concept pointer_like = 
    pointer<T>
 ||
(
    dereferenceable<T>
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
concept c_enum = enum_<T> && !std::is_scoped_enum_v<T>;

template <typename T>
concept c_array = std::is_array_v<T>;

template <typename T, typename Elem>
concept c_array_of =
    c_array<T>
 && std::same_as<std::remove_all_extents_t<T>, Elem>
;

///////////////////////////////////////////////////////////////////////////////
namespace detail
{

using namespace boost::mp11;

using char_types_t = mp_list
<
      char
    , unsigned char
    , signed char
    , wchar_t
    , char8_t
    , char16_t
    , char32_t
>;

template <typename T>
consteval auto is_char() -> bool
{
    return mp_find<char_types_t, std::remove_cvref_t<T>>::value < mp_size<char_types_t>::value;
}

}

template <typename T> concept char_ = std::integral<T> && detail::is_char<T>();

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

///////////////////////////////////////////////////////////////////////////////
//
// unit concepts
//
///////////////////////////////////////////////////////////////////////////////
template <typename T>
concept unit = 
    std::derived_from<T, strong_typedef_tag>
 && requires
    {
        typename T::underlying_type;
    }
 && std::constructible_from<typename T::underlying_type>
;

// rich format support
template <typename T>
concept fmt_unit =
    unit<T>
 && io::strong_typedef_fmt_traits<T>::enable_luxury_io
;

//-----------------------------------------------------------------------------
template <typename T>
concept interop_unit =
    unit<T>
 && requires
    {
        typename T::difference_type;
    }
;

///////////////////////////////////////////////////////////////////////////////
template <typename T>
concept enumerable = std::integral<T> || enum_<T> || unit<T>;

///////////////////////////////////////////////////////////////////////////////
//
// Integral constant concepts
//
///////////////////////////////////////////////////////////////////////////////
namespace detail
{

template <enumerable I, typename T>
struct instance_of_enumerable_: std::false_type {};

template <enumerable I, I I_, template <typename, auto> typename IntConstntT>
struct instance_of_enumerable_<I, IntConstntT<I, I_>>: std::true_type {};

}

template <typename T, typename I>
concept instance_of_enumerable =
    enumerable<I>
 && detail::instance_of_enumerable_<I, T>::value
;

template <typename T>
concept instance_of_bool = instance_of_enumerable<T, bool>;

template <typename T>
concept instance_of_unsigned = instance_of_enumerable<T, unsigned>;

}

///////////////////////////////////////////////////////////////////////////////
template <typename T>
struct interop_type : std::type_identity<T> {};

template <typename T> requires std::integral<T>
struct interop_type<T> : boost::promote<T> {};

template <typename T> requires c::c_enum<T>
struct interop_type<T> : boost::promote<T> {};

template <typename T> requires c::scoped_enum<T>
struct interop_type<T> : std::underlying_type<T> {};

template <typename T> requires c::unit<T>
struct interop_type<T> : std::type_identity<typename T::underlying_type> {};

template <typename T> requires c::interop_unit<T>
struct interop_type<T> : std::type_identity<typename T::difference_type> {};

// always integer
template <c::enumerable T> using interop_type_t = interop_type<T>::type;

///////////////////////////////////////////////////////////////////////////////
namespace c
{

namespace detail
{

template <typename Lhs, typename Rhs, typename Res>
concept bit_ops_ = requires(Lhs lhs, Rhs rhs)
{
    { lhs & rhs } /*noexcept*/ -> std::same_as<Res>;
    { lhs | rhs } /*noexcept*/ -> std::same_as<Res>;
    { lhs ^ rhs } /*noexcept*/ -> std::same_as<Res>;
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
     && std::equality_comparable_with<T, interop_type_t<T>>
     &&
     (
            // for integral types & C enums with implicit conversion to int types
           detail::commutative_bit_ops_<T, interop_type_t<T>, interop_type_t<T>>
            // for integral types & scoped enums with overloaded operators
        || detail::commutative_bit_ops_<T, interop_type_t<T>, T>
     )  
 )
;

///////////////////////////////////////////////////////////////////////////////
namespace detail
{

template <typename Lhs, typename Rhs, typename Res>
concept commutative_ariphmetic_ops__ = requires(Lhs lhs, Rhs rhs)
{
    { lhs + rhs } /*noexcept*/ -> std::same_as<Res>;
    { lhs * rhs } /*noexcept*/ -> std::same_as<Res>;
};

template <typename Lhs, typename Rhs, typename Res>
concept commutative_ariphmetic_ops_ =
    commutative_ariphmetic_ops__<Lhs, Rhs, Res>
 && commutative_ariphmetic_ops__<Rhs, Lhs, Res>
;

template <typename Lhs, typename Rhs, typename Res>
concept noncommutative_ariphmetic_ops_ = requires(Lhs lhs, Rhs rhs)
{
    { lhs - rhs } /*noexcept*/ -> std::same_as<Res>;
    { lhs / rhs } /*noexcept*/ -> std::same_as<Res>;
    { lhs % rhs } /*noexcept*/ -> std::same_as<Res>;
};

}

template <typename T>
concept ptr_ariphmetic = 
    enumerable<T>
 && std::equality_comparable<T>
    // for integral, unit types or scoped enums with overloaded operators
 && detail::commutative_ariphmetic_ops_<T, interop_type_t<T>, T>
 && detail::noncommutative_ariphmetic_ops_<T, interop_type_t<T>, T>
;

template <typename T>
concept strong_ariphmetic = 
    enumerable<T>
 && std::equality_comparable<T>
 && detail::commutative_ariphmetic_ops__<T, T, T>
 && detail::noncommutative_ariphmetic_ops_<T, T, T>
;

template <typename T>
concept ariphmetic = 
    strong_ariphmetic<T>
 || 
    enumerable<T>
 && std::equality_comparable<T>
 && std::equality_comparable_with<T, interop_type_t<T>>
 (
        // for integral types & C enums with implicit conversion to int types
        detail::commutative_ariphmetic_ops_<T, interop_type_t<T>, interop_type_t<T>>
     && detail::noncommutative_ariphmetic_ops_<T, interop_type_t<T>, interop_type_t<T>>
     ||
        // for integral, unit types or scoped enums with overloaded operators
        detail::commutative_ariphmetic_ops_<T, interop_type_t<T>, T>
     && detail::noncommutative_ariphmetic_ops_<T, interop_type_t<T>, T>
 )
;

////////////////////////////////////////////////////////////////////////////////

template <typename T>
concept string = requires (T const &ct, std::size_t idx)
{
    typename T::value_type;
    typename T::traits_type;

//    { ct.c_str() } -> std::same_as<typename T::value_type const *>;
    { ct[idx] } -> std::same_as<typename T::value_type const &>;
    { std::size(ct) } -> std::same_as<std::size_t>;
};

//-----------------------------------------------------------------------------
template <typename T, typename Char, typename CharTraits>
concept string_of =
    string<T>
 && std::same_as<typename T::value_type, Char>
 && std::same_as<typename T::traits_type, CharTraits>
;

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename Char, typename CharTraits>
concept printable = requires (std::basic_ostream<Char, CharTraits> &ostr, T const &t)
{
    { ostr << t } -> std::same_as<decltype(ostr)>;
};

//-----------------------------------------------------------------------------
template <typename Src, typename Dest>
concept explicitly_convertible_to = requires (Src src)
{
    { static_cast<Dest>(src) } -> std::same_as<Dest>;
};

//-----------------------------------------------------------------------------
template <typename T>
concept predicate =
    explicitly_convertible_to<T const &, bool>
 && requires (T const &t)
    {
        { !t } -> std::same_as<bool>;
    }
;

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

//-----------------------------------------------------------------------------
//
// strong_typedef, offset_value helper concepts
//
template <typename U, typename V>
concept int_convertible_to =
    std::integral<U>
 && std::integral<V>
 && std::convertible_to<U, V>
;

///////////////////////////////////////////////////////////////////////////////
//
// exception concepts
//
template <typename E> concept exception =
    std::derived_from<E, std::exception>
 && std::derived_from<E, boost::exception>
;

template <typename E>
concept std_only_exception =
    std::derived_from<E, std::exception>
 && !std::derived_from<E, boost::exception>
;

template <typename E>
concept boost_only_exception =
    !std::derived_from<E, std::exception>
 && std::derived_from<E, boost::exception>
;

//-----------------------------------------------------------------------------
template <typename T> concept error_info = instance_of<T, boost::error_info>;

template <typename T>
concept error_info_pair =
    instance_of<T, boost::mpl::pair>
 && error_info<typename T::first>
 && error_info<typename T::second>
;

namespace detail
{

using namespace boost::mp11;

template <typename T>
struct is_error_info_map_: std::false_type {};

template <error_info_pair... ErrorInfoPairs>
struct is_error_info_map_<mp_list<ErrorInfoPairs...>>: std::true_type {};

//-----------------------------------------------------------------------------
template <typename T>
struct is_error_info_list_: std::false_type {};

template <error_info... ErrorInfos>
struct is_error_info_list_<mp_list<ErrorInfos...>>: std::true_type {};

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

///////////////////////////////////////////////////////////////////////////////
//
// Extended enum concepts
//
///////////////////////////////////////////////////////////////////////////////
template <typename T>
concept enum_info_types_ =
    requires
    {
        typename T::enum_type;
        typename T::record_type;
        typename T::op_type;
        typename T::groups_type;
        typename T::mask_type;
        typename T::masks_type;
        typename T::interop_type;
        typename T::elements_type;
    }
;

template <typename T>
concept enum_info =
    enum_info_types_<T>
 && enum_<typename T::enum_type>
 && requires
    (
          T const &einfo
        , typename T::op_type &ops
        , typename T::groups_type &groups
        , typename T::masks_type &masks
        , std::size_t &sz
        , typename T::elements_type &elems
    )
    {
        sz = T::size;
        ops = einfo.m_ops;
        groups = einfo.m_groups;
        masks = einfo.m_masks;
        elems = einfo.m_elements;
        { std::as_const(einfo).kind() } -> std::same_as<enum_::kind_t>;
        { std::as_const(einfo).min_value() } -> std::same_as<typename T::enum_type>;
        { std::as_const(einfo).max_value() } -> std::same_as<typename T::enum_type>;
        { std::as_const(einfo).nullable() } -> std::same_as<bool>;
    }
;

//-----------------------------------------------------------------------------
template <typename E>
concept adapted_enum =
    enum_<E>
 && requires (E e)
    {
        { adapt_enum_info(e) } -> enum_info;
    }
;

///////////////////////////////////////////////////////////////////////////////
//
// Slot manipulator concepts
//
///////////////////////////////////////////////////////////////////////////////
template <typename U, typename V>
concept interoperable_with = std::equality_comparable_with<U, V>;

//-----------------------------------------------------------------------------
template <typename T>
concept decoder =
    std::default_initializable<T>
 && requires
    {
        typename T::decode_type;
        typename T::keep_type;    
    }
 && requires(typename T::decode_type in, typename T::keep_type out)
    {
        { T::decode(in) } -> std::same_as<typename T::keep_type>;
        { T::encode(out) } -> std::same_as<typename T::decode_type>;
    }
;

//-----------------------------------------------------------------------------
template <typename T, typename DecodeType>
concept decoder_for =
    decoder<T>
 && requires(DecodeType const &dt)
    {
        { T::decode(dt) } -> std::same_as<typename T::keep_type>;
    }
;

//-----------------------------------------------------------------------------
template <typename T, typename Decoder> concept decoded_by = decoder_for<Decoder, T>;

//-----------------------------------------------------------------------------
template <typename T, typename DecodeType>
concept static_decoder_for =
    decoder_for<T, DecodeType>
 && requires(DecodeType const &dt)
    {
        { T::static_decode(dt) } -> interoperable_with<typename T::keep_type>;
    }
;

///////////////////////////////////////////////////////////////////////////////
//
// low level manipulator slot processor
//
template <typename T>
concept storage =
    std::default_initializable<T>
 && requires(std::ios_base &ios)
    {
        typename T::tag_type;
        typename T::keep_type;

        { T::index(ios) } -> std::same_as<int>;                                         // slot index
        { T::value(ios) } -> std::same_as<typename T::keep_type>;                       // get value
        { T::value(ios, typename T::keep_type{}) } -> std::same_as<void>;               // set value
    }
;

template <typename T>
concept restore_storage = 
    storage<T>
 && requires(std::ios_base & ios)
    {
        { T::restore(ios) } -> std::same_as<void>;
    }
;
//-----------------------------------------------------------------------------
//
// slot_manip stuff

// NOTE: introduce concept to drop <slot/manip.h> dependency
template <typename T>
concept slot_manipulator =
    requires (std::ios_base &ios)
    {
        typename T::decoder_type;
        typename T::decode_type;
        typename T::storage_type;
        typename T::keep_type;

        { T::index(ios) } -> std::same_as<int>;
        { T::value(ios) } -> std::same_as<typename T::decode_type>;
        // TODO: refine concept
    }
 && storage<typename T::storage_type>
;

}

}
