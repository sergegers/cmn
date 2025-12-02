#pragma once

#include <type_traits>

#include <cmn/fwd.h>

#include "traits.h"

namespace cmn::c
{

///////////////////////////////////////////////////////////////////////////////
//
// type formatting concepts
//
///////////////////////////////////////////////////////////////////////////////
template <typename T>
concept format_info = 
    adapted_enum<std::remove_cvref_t<decltype(std::declval<T>().options)>>
;

//-----------------------------------------------------------------------------
template <typename T>
concept formatted_type = 
    requires(T t) 
    {
        { io::format_traits<T>{}(t) } -> format_info;
    }
;

//-----------------------------------------------------------------------------
template <typename T>
concept list_symbols = 
    const_string<std::remove_cvref_t<decltype(std::declval<T>().open)>> 
 && const_string<std::remove_cvref_t<decltype(std::declval<T>().close)>> 
 && const_string<std::remove_cvref_t<decltype(std::declval<T>().delimiter)>>
;

//-----------------------------------------------------------------------------
template <typename T>
concept list_format_info = 
    format_info<T> 
 && requires() 
    {
        &T::list_symbols;
    }
;

}

