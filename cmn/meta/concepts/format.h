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
concept list_formatted =
    source_formatted<T, Char, CharTraits>
 && requires
    {
        io::format_traits<T>::template source_options<Char, CharTraits>.open;
        io::format_traits<T>::template source_options<Char, CharTraits>.close;
        io::format_traits<T>::template source_options<Char, CharTraits>.delimiter;
    }
;

}
