#pragma once

#include <tuple>
#include <concepts>

#include "type_info.h"

namespace cmn
{

enum format_info_slot_t
{
    fis_options,
    fis_list_open,
    fis_list_close,
    fis_list_delim
};

namespace c
{

///////////////////////////////////////////////////////////////////////////////
//
// type formatting concepts
//
///////////////////////////////////////////////////////////////////////////////

namespace detail
{

template <typename T>
concept format_info_types_ = requires { typename std::tuple_element_t<fis_options, T>; };

}

template <typename T>
concept format_info = 
    detail::format_info_types_<T> 
 && adapted_enum<std::tuple_element_t<fis_options, T>> 
 && requires(T t) 
    {
        { std::get<fis_options>(std::as_const(t)) } -> std::same_as<std::tuple_element_t<fis_options, T>>;
    };

template <typename T>
concept formatted_type = requires(T t) 
{
    { get_default_format_info(t) };
};

}

}