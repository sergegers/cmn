#pragma once

#include <cmn/meta/concepts.h>
#include <cmn/enum/kind.h>
#include <cmn/enum/op.h>
#include <cmn/enum/info.h>

#include <cmn/enum/detail/macro.h>

// Do not include <cmn/enum/bitfield.h> to avoid circular dependency
//#include <cmn/enum/bitfield.h>

namespace cmn::enum_::io
{

///////////////////////////////////////////////////////////////////////////////
//
// Print options
//
///////////////////////////////////////////////////////////////////////////////

enum class print_t
{
      empty         = 0x0
    , tail          = 0x1
    , ns            = 0x2
    , class_prefix  = 0x4
};

///////////////////////////////////////////////////////////////////////////////
//
// Print options manipulator
//
///////////////////////////////////////////////////////////////////////////////
consteval auto adapt_enum_info(print_t)
{
    using enum print_t;
    return enum_info
    {
        op_comparable | op_bitwise | op_interoperable,
        group_::make<empty, tail, class_prefix>()
    };
}

CMN_ENUM_INJECT_OPS()

//static_assert(ops_v<print_t> == (op_comparable | op_bitwise | op_interoperable));
static_assert(c::bitfield<print_t>);

}
