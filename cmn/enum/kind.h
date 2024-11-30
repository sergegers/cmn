#pragma once

#include <type_traits>

namespace cmn::enum_
{

enum class kind_t
{
    naive,  // enum w/o adaptation
    enum_,
    bitfield,
    combo
};

template <kind_t Kind_>
using kkind_t = std::integral_constant<kind_t, Kind_>;

//-----------------------------------------------------------------------------
enum op_t
{
    op_empty           = 0x00,
    op_bitwise         = 0x01,
    op_steppable       = 0x02,
    op_comparable      = 0x04,
    op_ariphmetic      = 0x08,
    op_io              = 0x10,
    op_interoperable   = 0x20
};

}
