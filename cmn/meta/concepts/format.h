#pragma once

#include <concepts>

#include <cmn/fwd.h>
#include "enum.h"

namespace cmn::c
{

template <typename T>
concept format_options = requires(T const &fo)
{
    typename T::options_type;
    { fo.options } -> adapted_enum_cvref;
};

// TODO:
////-----------------------------------------------------------------------------
//namespace detail
//{
//
//template <typename T>
//concept list_sink_format_option_types_ = 
//    requires
//    {
//        typename T::char_type;
//        typename T::char_traits_type;
//    }
//;
//
//template
//<
//      typename T
//    , typename Char
//    , typename CharTraits
//>
//concept check_list_sink_format_option_types_ =
//    std::same_as<typename T::char_type, Char>
// && std::same_as<typename T::char_traits_type, CharTraits>
//;
//
//}
//
// TODO:
template <typename T>
concept list_sink_format_options =
 format_options<T>;
// && detail::list_sink_format_option_types_<T>
// && requires (T const& fo)
//{
//    { fo.open };
//    { fo.close };
//    { fo.delimiter };
//    { T::options_type::brackets };
//    { T::options_type::delimiter };
//}
//;

// TODO:
template
<
    typename T
    , typename Char
    , typename CharTraits
>
concept list_sink_format_options_of =
    list_sink_format_options<T>;
// && detail::check_list_sink_format_option_types_<T, Char, CharTraits>
// && requires (T const &fo)
//    {
//        { fo.open } -> const_string_of_cvref<Char, CharTraits>;
//        { fo.close } -> const_string_of_cvref<Char, CharTraits>;
//        { fo.delimiter } -> const_string_of_cvref<Char, CharTraits>;
//    }
//;
//
//-----------------------------------------------------------------------------
template <typename T>
concept formatted = format_options<io::format_traits<T>>;

//-----------------------------------------------------------------------------
template
<
    typename T
    , typename Char
    , typename CharTraits
>
concept list_sink_formatted =
    formatted<T>
 && list_sink_format_options_of<io::sink_format_traits<T, Char, CharTraits>, Char, CharTraits>
;

}
