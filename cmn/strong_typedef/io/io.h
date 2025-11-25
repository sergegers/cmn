#pragma once

#include <iosfwd>
#include <concepts>

#include <boost/optional.hpp>

#include <cmn/meta/concepts.h>
#include <cmn/io/manip/format_options.h>
#include <cmn/strong_typedef/strong_typedef.h>

namespace cmn
{

namespace io
{

namespace detail
{

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

template
<
      c::unit Unit
    , typename Char
    , typename CharTraits
>
[[nodiscard]] auto try_read(std::basic_istream<Char, CharTraits> &istr) noexcept -> boost::optional<Unit>
{
    using underlying_type = Unit::underlying_type;
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
    return write(ostr, io::get_format_options(ostr, unit), io::access::value_ref(unit));
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
    return read(istr,  io::get_format_options(istr, unit), io::access::value_ref(unit));
}

}
