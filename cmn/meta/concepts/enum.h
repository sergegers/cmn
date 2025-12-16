#pragma once

#include <cstddef>
#include <concepts>
#include <type_traits>
#include <utility>

#include <cmn/fwd.h>

#include "traits.h"

namespace cmn::c
{

///////////////////////////////////////////////////////////////////////////////
//
// Extended enum concepts
//
///////////////////////////////////////////////////////////////////////////////
namespace detail
{

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

} // namespace detail

template <typename T>
concept enum_info = 
    detail::enum_info_types_<T> 
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
        { T::kind() } noexcept -> std::same_as<enum_::kind_t>;
        { std::as_const(einfo).min_value() } noexcept -> std::same_as<typename T::enum_type>;
        { std::as_const(einfo).max_value() } noexcept -> std::same_as<typename T::enum_type>;
        { std::as_const(einfo).nullable() } noexcept -> std::same_as<bool>;
        { std::as_const(einfo).unique() } noexcept -> std::same_as<bool>;
    };

//-----------------------------------------------------------------------------
template <typename E>
concept adapted_enum = 
    enum_<E> 
    && requires(E e) 
    {
        { adapt_enum_info(e) } /*noexcept*/ -> enum_info;
    }
;

template <typename T>
concept adapted_enum_cvref = adapted_enum<std::remove_cvref_t<T>>;

}
