#pragma once

#include <string>
#include <utility>

#include <cmn/fwd.h>
#include <cmn/meta/concepts.h>

#include <cmn/io/manip/format_options.h>

#include <cmn/enum/op.h>
#include <cmn/enum/detail/enum_info.h>
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
      empty         = 0x00
    , brackets      = 0x01
    , delimiter     = 0x02
    , tail          = 0x04
    , class_prefix  = 0x08
    , ns_           = 0x10
    , ns            = ns_ | class_prefix
};

///////////////////////////////////////////////////////////////////////////////
//
// Print options manipulator
//
///////////////////////////////////////////////////////////////////////////////
consteval auto adapt_enum_info(print_t)
{
    // TODO: + [empty, ns] to magic constants

    using enum print_t;
    return enum_info
    {
        op_comparable | op_bitwise | op_interoperable,
        group_::make<brackets, delimiter, tail, class_prefix, ns_>()
    };
}

CMN_ENUM_INJECT_OPS()

#ifdef CMN_STATIC_TEST

static_assert(std::same_as<interop_type_t<print_t>, int>);
static_assert(ops_v<print_t> == (op_comparable | op_bitwise | op_interoperable));
static_assert(c::strong_bitfield<print_t>);
static_assert(c::bitfield<print_t>);

#endif

}
