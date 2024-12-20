#pragma once

#include <iosfwd>
#include <concepts>
#include <functional>

#include <boost/implicit_cast.hpp>
#include <boost/optional.hpp>

#include <cmn/meta/concepts.h>
#include <cmn/meta/type_traits.h>

#include <cmn/io/manip/slot/fwd.h>
#include <cmn/io/manip/slot/manip.h>
#include <cmn/io/manip/slot/forwarder.h>

#include <cmn/enum/combo.h>
#include <cmn/util/strong_typedef.h>
#include <cmn/util/feature.h>

namespace cmn
{

namespace io
{

//
// predefined integer formats
//

// NOTE: zero value isn't used, so we can get mask from value
enum class int_fmt_t: short
{
    dec                 = 0b0000'0000'0000'0001,
    hex                 = 0b0000'0000'0000'0010,

    showbase            = 0b0000'0000'0000'0100,
    hidebase            = 0b0000'0000'0000'1000,

    asm_                = 0b0000'0000'0001'0000,
    c                   = 0b0000'0000'0010'0000,
                            
    short_              = 0b0000'0000'0100'0000,
    long_               = 0b0000'0000'1000'0000,
                            
    uppercase           = 0b0000'0001'0000'0000,
    lowercase           = 0b0000'0010'0000'0000,
                            
    sign                = 0b0000'0100'0000'0000,      // space for zero
    nosign              = 0b0000'1000'0000'0000,
    forcesign           = 0b0001'0000'0000'0000,      // + for zero
    
    empty               = 0b0000'0000'0000'0000,
    default_            = dec | hidebase | c    | short_ | lowercase | nosign,      // must be synced with
                                                                                    // strong_typedef_traits::default_
    // masks                            
    base_mask           = dec | hex,
    showbase_mask       = showbase | hidebase,
    lang_mask           = asm_ | c,
    width_mask          = short_ | long_,
    case_mask           = uppercase | lowercase,
    sign_mask           = sign | nosign | forcesign,
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

namespace manip
{

auto override_value(int_fmt_t orig, int_fmt_t over) -> int_fmt_t;

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

//-----------------------------------------------------------------------------
template
<
      typename Char
    , typename CharTraits
    , c::fmt_unit Unit
>
struct reader
{
    using te_type = underlying_type_t<Unit>;

    auto operator ()(std::basic_istream<Char, CharTraits> &istr, int_fmt_t fmt, te_type &te_unit) const
        -> std::basic_istream<Char, CharTraits> &
    {
        using enum int_fmt_t;

        if (Unit unit; has_any_feature(fmt, sign, forcesign))
        {
            auto const sgn = te_unit;
            access::in(istr, unit);
            // does not require unary minus
            te_unit = sgn < 0? -unit.value(): unit.value();

            return istr;
        }
        else
        {
            access::in(istr, unit);
            te_unit = unit.value();
            return istr;
        }
    }
};

// BUG: couldn't use auto function type
template
<
      typename Char
    , typename CharTraits
    , std::integral Unit
>
using te_reader = std::function<std::basic_istream<Char, CharTraits> & (std::basic_istream<Char, CharTraits> &, int_fmt_t, Unit &)>;

//-----------------------------------------------------------------------------
template
<
      typename Char
    , typename CharTraits
    , c::fmt_unit Unit
>
struct writer
{
    using te_type = underlying_type_t<Unit>;
    
    auto operator ()(std::basic_ostream<Char, CharTraits> &ostr, int_fmt_t fmt, te_type const &te_unit) const
        -> std::basic_ostream<Char, CharTraits> &
    {
        using enum int_fmt_t;
        if (has_any_feature(fmt, sign, forcesign) && te_unit < 0)
        {
            using signed_type = std::make_signed_t<te_type>;
            // does not require unary minus
            te_type const neg = -boost::implicit_cast<signed_type>(te_unit);
            return access::out(ostr, Unit{ neg }), ostr;
        }
        else
            return access::out(ostr, Unit{ te_unit }), ostr;        
    }
};

// BUG: couldn't use auto function type
template
<
      typename Char
    , typename CharTraits
    , std::integral Unit
>
using te_writer = std::function<std::basic_ostream<Char, CharTraits> & (std::basic_ostream<Char, CharTraits> &, int_fmt_t, Unit const &)>;

//-----------------------------------------------------------------------------
template
<
      typename Char
    , typename CharTraits
    , std::integral Unit
>
auto write(std::basic_ostream<Char, CharTraits> &ostr, int_fmt_t fmt, te_writer<Char, CharTraits, Unit> wtr, Unit const &unit)->
    std::basic_ostream<Char, CharTraits> &;

template
<
      typename Char
    , typename CharTraits
    , std::integral Unit
>
auto read(std::basic_istream<Char, CharTraits> &istr, int_fmt_t fmt, te_reader<Char, CharTraits, Unit> rdr, Unit &unit)->
    std::basic_istream<Char, CharTraits> &;

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

auto operator << (std::basic_ostream<Char, CharTraits> &ostr, Unit const &unit)->
    std::basic_ostream<Char, CharTraits> &
{ 
    using namespace io::detail;
    using writer_type = writer<Char, CharTraits, Unit>;
    using te_writer_type = te_writer<Char, CharTraits, underlying_type_t<Unit>>;

    return write(ostr, get_value(ostr, unit), te_writer_type{ writer_type{} }, unit.value());
}

template
<
      typename Char
    , typename CharTraits
    , c::fmt_unit Unit
>
    requires true

auto operator >> (std::basic_istream<Char, CharTraits> &istr, Unit &unit)-> std::basic_istream<Char, CharTraits> &
{
    using namespace io::detail;
    using reader_type = reader<Char, CharTraits, Unit>;
    using te_reader_type = te_reader<Char, CharTraits, underlying_type_t<Unit>>;

    return read(istr,  get_value(istr, unit), te_reader_type{ reader_type{} }, io::access::value_ref(unit));
}

namespace enum_::op
{

extern template auto operator << (std::ostream &, cmn::io::int_fmt_t) -> std::ostream &;

}

}
