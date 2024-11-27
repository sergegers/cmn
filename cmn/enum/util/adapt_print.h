#pragma once

#include <cmn/enum/op.h>

#include <cmn/enum/util/enum_info.h>
#include <cmn/enum/util/groups_info.h>
#include <cmn/enum/util/group_info.h>
#include <cmn/enum/util/macro.h>

// Do not include <cmn/enum/bitfield.h> to avoid circular dependency
//#include <cmn/enum/bitfield.h>

namespace cmn::enum_::io
{

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

CMN_INJECT_ENUM_OPS()

static_assert(ops_v<print_t> == (op_comparable | op_bitwise | op_interoperable));
static_assert(c::bitfield<print_t>);

}
