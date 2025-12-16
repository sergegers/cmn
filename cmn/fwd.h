#pragma once

#include <string>

namespace cmn
{

struct strong_typedef_tag {};

namespace io
{

enum class int_fmt_t: short;

//-----------------------------------------------------------------------------
//
// traits
//
template <typename TypeOrTag>
struct format_traits;

template 
<
      typename TypeOrTag
    , typename Char
    , typename CharTraits = std::char_traits<Char>
>
struct sink_format_traits;

//-----------------------------------------------------------------------------

template <typename Unit>
struct strong_typedef_fmt_traits
{
    static constexpr bool enable_luxury_io = false;
    static constexpr auto default_ = static_cast<int_fmt_t>(2665);  // int_fmt_t::default_
};

}

//-----------------------------------------------------------------------------
namespace enum_
{

enum class kind_t
{
    naive,  // enum w/o adaptation
    enum_,
    bitfield,
    combo
};

//-----------------------------------------------------------------------------
enum op_t
{
    op_empty            = 0x00,
    op_comparable       = 0x01,
    op_bitwise          = 0x02,
    op_steppable        = 0x04,
    op_ariphmetic       = 0x08,
    op_io               = 0x10,
    op_interoperable    = 0x20
};

}

}