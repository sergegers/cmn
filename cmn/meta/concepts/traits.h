#pragma once

#include <concepts>
#include <type_traits>

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

//-----------------------------------------------------------------------------
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


}

