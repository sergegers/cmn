#pragma once

#include <iosfwd>
#include <concepts>

#include <boost/optional.hpp>

#include <cmn/meta/concepts.h>
#include <cmn/meta/type_traits.h>

#include <cmn/enum/feature.h>

#include <cmn/io/manip/slot/fwd.h>
#include <cmn/io/manip/slot/manip.h>
#include <cmn/io/manip/slot/forwarder.h>

#include <cmn/strong_typedef/strong_typedef.h>
#include <cmn/strong_typedef/io/int_fmt.h>

namespace cmn
{

namespace io
{

namespace manip
{

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
constexpr auto uoct = int_fmt(int_fmt_t::oct);
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
    return set_feature(unit_default, manip_value); // override default values by the stream ones
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
