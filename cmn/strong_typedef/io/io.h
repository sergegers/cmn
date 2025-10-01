#pragma once

#include <iosfwd>
#include <concepts>

#include <boost/optional.hpp>

#include <cmn/meta/concepts.h>
#include <cmn/meta/type_traits.h>

#include <cmn/io/manip/slot/fwd.h>
#include <cmn/io/manip/slot/manip.h>
#include <cmn/io/manip/slot/forwarder.h>

#include <cmn/enum/combo.h>
#include <cmn/util/feature.h>

#include <cmn/strong_typedef/strong_typedef.h>

namespace cmn
{

namespace io
{

//
// predefined integer formats
//
enum class radix_fmt_t
{
    dec                 = 0b0000'0000'0000'0001,    // default
    hex                 = 0b0000'0000'0000'0010,    // x
    mask                = dec | hex
};

enum class base_fmt_t
{
    showbase            = 0b0000'0000'0000'0100,    // #
    hidebase            = 0b0000'0000'0000'1000,    // default
    mask                = showbase | hidebase    
};

enum class lang_fmt_t
{
    asm_                = 0b0000'0000'0001'0000,    // a
    c                   = 0b0000'0000'0010'0000,    // default
    mask                = asm_ | c    
};

enum class width_fmt_t
{
    short_              = 0b0000'0000'0100'0000,    // s, default
    long_               = 0b0000'0000'1000'0000,    // l
    mask                = short_ | long_
};

enum class case_fmt_t
{
    uppercase           = 0b0000'0001'0000'0000,    // u
    lowercase           = 0b0000'0010'0000'0000,    // default
    mask                = uppercase | lowercase    
};

enum class sign_fmt_t
{
    sign                = 0b0000'0100'0000'0000,    // ( ) space for zero
    nosign              = 0b0000'1000'0000'0000,    // default
    forcesign           = 0b0001'0000'0000'0000,    // (+) + for zero
    mask                = sign | nosign | forcesign    
};

//-----------------------------------------------------------------------------
//
// NOTE: zero value isn't used, so we can get mask from value
//
enum class int_fmt_t: short
{
    dec                 = radix_fmt_t::dec,         // default
    hex                 = radix_fmt_t::hex,         // x

    showbase            = base_fmt_t::showbase,     // #
    hidebase            = base_fmt_t::hidebase,     // default

    asm_                = lang_fmt_t::asm_,         // a
    c                   = lang_fmt_t::c,            // default
                          
    short_              = width_fmt_t::short_,       // s, default
    long_               = width_fmt_t::long_,        // l
                          
    uppercase           = case_fmt_t::uppercase,    // u
    lowercase           = case_fmt_t::lowercase,    // default
                          
    sign                = sign_fmt_t::sign,         // ( ) space for zero
    nosign              = sign_fmt_t::nosign,       // default
    forcesign           = sign_fmt_t::forcesign,    // (+) + for zero
    
    empty               = 0b0000'0000'0000'0000,
    default_            = dec | hidebase | c    | short_ | lowercase | nosign,      // must be synced with
                                                                                    // strong_typedef_traits::default_
    // masks                            
    radix_mask          = radix_fmt_t::mask,
    base_mask           = base_fmt_t::mask,
    lang_mask           = lang_fmt_t::mask,
    width_mask          = width_fmt_t::mask,
    case_mask           = case_fmt_t::mask,
    sign_mask           = sign_fmt_t::mask,
    // predefined formats
    sshort_asm_up_hex   = hex | showbase | asm_ | short_ | uppercase | sign,        // +1Bh
    long_asm_up_hex     = hex | showbase | asm_ | long_ | uppercase | nosign,       // 0000001Bh
    long_c_up_hex       = hex | showbase | c | long_ | uppercase | forcesign        // +0x0000001B
};

consteval auto adapt_enum_info(int_fmt_t en)
{
    using namespace cmn::enum_;
    using enum int_fmt_t;

    return enum_info
    (
          default_ops(en, kind_t::combo) | op_interoperable
        , group_::make<dec, hex>()
        , group_::make<showbase, hidebase>()
        , group_::make<asm_, c>()
        , group_::make<short_, long_>()
        , group_::make<uppercase, lowercase>()
        , group_::make<sign, nosign, forcesign>()
    );
}

CMN_ENUM_INJECT_OPS()

static_assert(c::strong_bitfield<int_fmt_t>);

namespace manip
{

auto override_value(int_fmt_t old, int_fmt_t new_) -> int_fmt_t;

// set one group at once
struct int_fmt_storage_t
{
    using keep_type = int_keep_type;
    using tag_type = int_fmt_storage_t;

    static auto index(std::ios_base &ios) -> int;
    static auto value(std::ios_base &ios) -> keep_type;
    static auto value(std::ios_base &ios, keep_type value) -> void;
};

using int_fmt_slot_manip = slot_manip
<
      int_fmt_t 
    , int_fmt_t::empty      // here is empty values, initial value
    , int_fmt_t::empty      // supplies by strong_typedef_fmt_traits
    , int_decoder<int_fmt_t>
    , int_fmt_storage_t
>;

using int_fmt_forwarder = slot_manip_forwarder<int_fmt_slot_manip>;

inline constexpr int_fmt_forwarder int_fmt{};

constexpr auto udec = int_fmt(int_fmt_t::dec);
constexpr auto uhex = int_fmt(int_fmt_t::hex);
constexpr auto ushowbase = int_fmt(int_fmt_t::showbase);
constexpr auto uhidebase = int_fmt(int_fmt_t::hidebase);
constexpr auto uupercase = int_fmt(int_fmt_t::uppercase);
constexpr auto ulowercase = int_fmt(int_fmt_t::lowercase);
constexpr auto uc = int_fmt(int_fmt_t::c);
constexpr auto uasm = int_fmt(int_fmt_t::asm_);
constexpr auto ulong_ = int_fmt(int_fmt_t::long_);
constexpr auto ushort_ = int_fmt(int_fmt_t::short_);
constexpr auto usign = int_fmt(int_fmt_t::sign);
constexpr auto unosign = int_fmt(int_fmt_t::nosign);
constexpr auto uforcesign = int_fmt(int_fmt_t::forcesign);

}

namespace detail
{

template
<
      typename Char
    , typename CharTraits
    , c::unit Unit
>
auto get_value(std::basic_ios<Char, CharTraits> &ios, Unit const &/*unit*/) -> int_fmt_t
{
    auto const unit_default = default_v<Unit>;  // get default type formatting options from traits
    auto const manip_value = manip::int_fmt_slot_manip::value(ios);
    return manip::override_value(unit_default, manip_value); // override default values by the stream ones
}

///////////////////////////////////////////////////////////////////////////////
template
<
      typename Char
    , typename CharTraits
    , std::integral Int
>
auto write(std::basic_ostream<Char, CharTraits> &ostr, int_fmt_t fmt, Int const &unit) -> std::basic_ostream<Char, CharTraits> &;

template
<
      typename Char
    , typename CharTraits
    , std::integral Int
>
auto read(std::basic_istream<Char, CharTraits> &istr, int_fmt_t fmt, Int &unit) -> std::basic_istream<Char, CharTraits> &;

//-----------------------------------------------------------------------------
template
<
      typename Char
    , typename CharTraits
>
[[nodiscard]] auto try_read_(std::basic_istream<Char, CharTraits> &istr) noexcept -> boost::optional<std::ptrdiff_t>;


}

using manip::int_fmt;
using manip::udec;
using manip::uhex;
using manip::ushowbase;
using manip::uhidebase;
using manip::uupercase;
using  manip::ulowercase;
using manip::uc;
using manip::uasm;
using manip::ulong_;
using manip::ushort_;
using manip::usign;
using manip::unosign;
using manip::uforcesign;

template
<
      c::unit Unit
    , typename Char
    , typename CharTraits
>
[[nodiscard]] auto try_read(std::basic_istream<Char, CharTraits> &istr) noexcept -> boost::optional<Unit>
{
    using underlying_type = typename Unit::underlying_type;
    return detail::try_read_(istr).map
    (
        [](std::ptrdiff_t i)
        {
            return Unit{ static_cast<underlying_type>(i) };
        }
    );
}

}

template
<
      typename Char
    , typename CharTraits
    , c::fmt_unit Unit
>
    requires true

auto operator << (std::basic_ostream<Char, CharTraits> &ostr, Unit const &unit) -> std::basic_ostream<Char, CharTraits> &
{
    using namespace io::detail;
    return write(ostr, get_value(ostr, unit), io::access::value_ref(unit));
}

template
<
      typename Char
    , typename CharTraits
    , c::fmt_unit Unit
>
    requires true

auto operator >> (std::basic_istream<Char, CharTraits> &istr, Unit &unit) -> std::basic_istream<Char, CharTraits> &
{
    using namespace io::detail;
    return read(istr,  get_value(istr, unit), io::access::value_ref(unit));
}

namespace enum_::op
{

extern template auto operator << (std::ostream &, cmn::io::int_fmt_t) -> std::ostream &;
extern template auto operator << (std::wostream &, cmn::io::int_fmt_t) -> std::wostream &;

}

}
