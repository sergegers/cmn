#pragma once

#include <concepts>
#include <type_traits>
#include <utility>

#include <cmn/fwd.h>

namespace cmn::c
{

////////////////////////////////////////////////////////////////////////////////
//
// republish some type traits as concepts
//
////////////////////////////////////////////////////////////////////////////////
template <typename T>
concept enum_ = std::is_enum_v<T>;

template <typename T>
concept scoped_enum = enum_<T> && std::is_scoped_enum_v<T>;

template <typename T>
concept c_enum = enum_<T> && !std::is_scoped_enum_v<T>;

////////////////////////////////////////////////////////////////////////////////
//
// type_info concepts
//
////////////////////////////////////////////////////////////////////////////////
template <typename T>
concept adapted_type = requires (T t)
{
    { adapt_type_info(t) };
};

//-----------------------------------------------------------------------------
template <typename T>
concept formatted_type =
    adapted_type<T> 
 && requires(decltype(adapt_type_info(std::declval<T>())) ti) 
    {
        { ti.formatting_options() } -> enum_;
    }
;

///////////////////////////////////////////////////////////////////////////////
//
// Extended enum concepts
//
///////////////////////////////////////////////////////////////////////////////
template <typename T>
concept enum_info_types_ = requires 
{
    typename T::enum_type;
    typename T::record_type;
    typename T::op_type;
    typename T::groups_type;
    typename T::mask_type;
    typename T::masks_type;
    typename T::interop_type;
    typename T::elements_type;
};

template <typename T>
concept enum_info = enum_info_types_<T> && enum_<typename T::enum_type> &&
    requires(T const &einfo, typename T::op_type &ops, typename T::groups_type &groups, typename T::masks_type &masks,
             std::size_t &sz, typename T::elements_type &elems) 
    {
        sz = T::size;
        ops = einfo.m_ops;
        groups = einfo.m_groups;
        masks = einfo.m_masks;
        elems = einfo.m_elements;
        { T::kind() } noexcept -> std::same_as<enum_::kind_t>;
        { std::as_const(einfo).min_value() } noexcept -> std::same_as<typename T::enum_type>;
        { std::as_const(einfo).max_value() } noexcept -> std::same_as<typename T::enum_type>;
        { std::as_const(einfo).nullable() } noexcept -> std::same_as<bool>;
        { std::as_const(einfo).unique() } noexcept -> std::same_as<bool>;
    }
;

//-----------------------------------------------------------------------------
template <typename E>
concept adapted_enum = 
    enum_<E> 
 && adapted_type<E>
 && requires(E e) 
{
    { adapt_type_info(e) } /*noexcept*/-> enum_info;
};

}
