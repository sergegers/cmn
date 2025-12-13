#pragma once

#include <cmn/fwd.h>

namespace cmn::c
{

template <typename T>
concept formatted = 
    adapted_enum<std::remove_cvref_t<decltype(T::options)>>
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
        { T::template source_options<Char, CharTraits>() };
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
        T::template source_options<Char, CharTraits>().open;
        T::template source_options<Char, CharTraits>().open.close;
        T::template source_options<Char, CharTraits>().open.delimiter;
    }
;


}
