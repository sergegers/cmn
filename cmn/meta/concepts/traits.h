#pragma once

#include <concepts>
#include <type_traits>
#include <tuple>
#include <utility>

namespace cmn::c
{

////////////////////////////////////////////////////////////////////////////////
//
// republish some type traits as concepts
//
////////////////////////////////////////////////////////////////////////////////
template <typename T>
concept pointer = std::is_pointer_v<T>;

template <typename T>
concept class_ = std::is_class_v<T>;

template <typename T>
concept function = std::is_function_v<T>;

template <typename T>
concept polymorphic = std::is_polymorphic_v<T>;

template <typename T>
concept c_array = std::is_array_v<T>;

template <typename T, typename Elem>
concept c_array_of = c_array<T> && std::same_as<std::remove_all_extents_t<T>, Elem>;

template <typename T, typename Res, typename... Args>
concept invocable_r = std::is_invocable_r_v<Res, T, Args...>;

template <typename T>
concept enum_ = std::is_enum_v<T>;

template <typename T>
concept scoped_enum = enum_<T> && std::is_scoped_enum_v<T>;

template <typename T>
concept c_enum = enum_<T> && !std::is_scoped_enum_v<T>;

//-----------------------------------------------------------------------------
template <typename T>
concept dereferenceable = requires(T &t) 
{
    { *t };
};

//-----------------------------------------------------------------------------
//
// add smart pointer support
//
// https://stackoverflow.com/a/78595795/8452129
//
//-----------------------------------------------------------------------------
template <typename T>
concept pointer_like = 
    pointer<T> 
 || (
        dereferenceable<T> 
     && requires(T t)
        {
            { static_cast<bool>(t) };
            { t.operator -> () } -> std::convertible_to<decltype(&*t)>;
        }
     )
;

////////////////////////////////////////////////////////////////////////////////
//
// string concepts
//
///////////////////////////////////////////////////////////////////////////////
namespace detail
{

template <typename T>
concept string_types_ = requires(T const &ct, std::size_t idx) 
{
    typename T::value_type;
    typename T::traits_type;
    typename T::pointer;
    typename T::const_pointer;
    typename T::reference;
    typename T::const_reference;
};


}

template <typename T>
concept const_string =
    detail::string_types_<T>
 && requires(T const &t, std::size_t idx) 
    {
        typename T::value_type;
        typename T::traits_type;

        { t.data() } -> std::same_as<typename T::const_pointer>;
        { t[idx] } -> std::same_as<typename T::const_reference>;
        { std::size(t) } -> std::same_as<std::size_t>;
    }
;

//-----------------------------------------------------------------------------
namespace detail
{

template 
<
      typename T
    , typename Char
    , typename CharTraits
>
concept check_types_ = std::same_as<typename T::value_type, Char> && std::same_as<typename T::traits_type, CharTraits>;

}

template 
<
      typename T
    , typename Char
    , typename CharTraits
>
concept const_string_of =
    const_string<T> 
 && detail::check_types_<T, Char, CharTraits>
;

//-----------------------------------------------------------------------------
template <typename T>
concept string = 
    const_string<T> 
 && requires(T &t, std::size_t idx) 
    {
        { t.data() } -> std::same_as<typename T::pointer>;
        { t[idx] } -> std::same_as<typename T::reference>;
    }
;

//-----------------------------------------------------------------------------
template <typename T, typename Char, typename CharTraits>
concept string_of = string<T> && detail::check_types_<T, Char, CharTraits>;

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename Char, typename CharTraits>
concept printable = requires(std::basic_ostream<Char, CharTraits> &ostr, T const &t) 
{
    { ostr << t } -> std::same_as<decltype(ostr)>;
};

//-----------------------------------------------------------------------------
template <typename Src, typename Dest>
concept explicitly_convertible_to = requires(Src src) 
{
    { static_cast<Dest>(src) } -> std::same_as<Dest>;
};

//-----------------------------------------------------------------------------
template <typename T>
concept predicate = explicitly_convertible_to<T const &, bool> && requires(T const &t) 
{
    { !t } -> std::same_as<bool>;
};

//-----------------------------------------------------------------------------
//
// for throwing destructors in test mode only
//
#ifdef CMN_TEST_MOCK

template <typename T, typename... Args>
concept test_constructible_from = requires(Args &&...args) { T{std::forward<Args>(args)...}; };
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
template <typename T>
concept test_move_constuctible = test_constructible_from<T, T &&>;
#else
template <typename T>
concept test_move_constuctible = std::move_constructible<T>;
#endif

//-----------------------------------------------------------------------------
//
// strong_typedef, offset_value helper concepts
//
template <typename U, typename V>
concept int_convertible_to = std::integral<U> && std::integral<V> && std::convertible_to<U, V>;

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
struct is_instance_of_ : std::false_type
{
};

template <template <typename...> typename TemplateT, typename... Args>
struct is_instance_of_<TemplateT, TemplateT<Args...>> : std::true_type
{
};

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
struct is_complete : std::false_type
{
};

template <typename T>
struct is_complete<T, std::void_t<decltype(sizeof(T) != 0)>> : std::true_type
{
};

}

template <typename T>
concept complete = detail::is_complete<T>::value;

///////////////////////////////////////////////////////////////////////////////
//
// tuple concepts
//
///////////////////////////////////////////////////////////////////////////////

namespace detail
{

template <typename T, std::size_t Idx_, typename Elem>
concept const_tuple_elem = requires (T const &t)
{
    { std::get<Elem>(t) } -> std::same_as<Elem const &>;
    { std::get<Idx_>(t) } -> std::same_as<Elem const &>;
};

template <typename T, typename Idss, typename... Elems>
struct check_const_tuple_elems : std::false_type {};

template <typename T, std::size_t... Idss_, typename... Elems>
struct check_const_tuple_elems<T, std::index_sequence<Idss_...>, Elems...> :
    std::bool_constant<(const_tuple_elem<T, Idss_, Elems> && ...)>
{
};

}

template <typename T, typename... Elems>
concept const_tuple_of =
    (std::tuple_size_v<T> == sizeof... (Elems))
 && detail::check_const_tuple_elems<T, std::make_index_sequence<sizeof... (Elems)>, Elems...>::value
;

//-----------------------------------------------------------------------------
namespace detail
{

template <typename T, std::size_t Idx_, typename Elem>
concept tuple_elem = requires (T t)
{
    { std::get<Elem>(t) } -> std::same_as<Elem &>;
    { std::get<Elem>(std::move(t)) } -> std::same_as<Elem &&>;

    { std::get<Idx_>(t) } -> std::same_as<Elem &>;
    { std::get<Idx_>(std::move(t)) } -> std::same_as<Elem &&>;
};

template <typename T, typename Idss, typename... Elems>
struct check_tuple_elems: std::false_type {};

template <typename T, std::size_t... Idss_, typename... Elems>
struct check_tuple_elems<T, std::index_sequence<Idss_...>, Elems...>:
    std::bool_constant<(tuple_elem<T, Idss_, Elems> && ...)>
{};

}

template <typename T, typename... Elems>
concept tuple_of =
    const_tuple_of<T, Elems...>
 && detail::check_tuple_elems<T, std::make_index_sequence<sizeof... (Elems)>, Elems...>::value
;

}

