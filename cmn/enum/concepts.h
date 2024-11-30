#pragma once

#include <cmn/meta/concepts.h>

#include "traits.h"

///////////////////////////////////////////////////////////////////////////////
//
// Extended enum concepts
//
///////////////////////////////////////////////////////////////////////////////
namespace cmn::c
{

template <typename Enum>
concept e_naive = enum_::traits<Enum>::kind == enum_::kind_t::naive;

template <typename Enum>
concept e_enum = enum_::traits<Enum>::kind == enum_::kind_t::enum_;

template <typename Enum>
concept e_bitfield = enum_::traits<Enum>::kind == enum_::kind_t::bitfield;

template <typename Enum>
concept e_combo = enum_::traits<Enum>::kind == enum_::kind_t::combo;

template <typename Enum>
concept e_any_enum = e_enum<Enum> || e_bitfield<Enum> || e_combo<Enum>;

}
