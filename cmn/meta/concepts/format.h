#pragma once

#include <type_traits>

#include <cmn/fwd.h>
#include "enum.h"

namespace cmn::c
{

template <typename T>
concept formatted = 
    adapted_enum<std::remove_cvref_t<decltype(io::format_traits<T>::options)>>
;

//-----------------------------------------------------------------------------
template 
<
      typename T
    , typename Char
    , typename CharTraits
>
concept source_formatted =
    formatted<T>
 && requires
    {
        { io::format_traits<T>::template source_options<Char, CharTraits> };
    }
;

//-----------------------------------------------------------------------------
template
<
    typename T
    , typename Char
    , typename CharTraits
>
concept list_source_options = requires
{
    { T::open } -> const_string_ref_of<Char, CharTraits>;
    { T::close } -> const_string_ref_of<Char, CharTraits>;
    { T::delimiter } -> const_string_ref_of<Char, CharTraits>;
};

//-----------------------------------------------------------------------------
template
<
    typename T
    , typename Char
    , typename CharTraits
>
concept const_list_source_options_ref = list_source_options<std::remove_cvref_t<T>, Char, CharTraits>;

//-----------------------------------------------------------------------------
template
<
    typename T
    , typename Char
    , typename CharTraits
>
concept list_formatted =
    source_formatted<T, Char, CharTraits>
 && requires
    {
        { io::format_traits<T>::template source_options<Char, CharTraits> } -> 
            const_list_source_options_ref<Char, CharTraits>;
    }
;

}
