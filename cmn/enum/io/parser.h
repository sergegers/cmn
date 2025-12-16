#pragma once

#include <cstddef>
#include <string>
#include <iterator>

#include <boost/optional/optional_fwd.hpp>
#include <boost/spirit/include/qi_symbols.hpp>

#include <cmn/fwd.h>
#include <cmn/meta/traits.h>

#include <cmn/enum/traits.h>
#include <cmn/enum/io/format_traits.h>

#include "print.h"

namespace cmn::enum_::io
{

///////////////////////////////////////////////////////////////////////////////

//
// NOTE: transfer istream iterator doesn't meet iterator requirements
//

// parse kind_t::enum
template
<
      typename Char
    , typename CharTraits
    , /*std::input_iterator*/ typename It
>
auto try_parse
(
      int_<kind_t::enum_>
    , boost::spirit::qi::symbols<Char, std::ptrdiff_t> const &items
    , basic_qualified_name<Char, CharTraits> const &enum_name
    , sink_format_options<Char, CharTraits> const &fmt_opt
    , It const &begin
    , It const &end
)
  noexcept -> boost::optional<std::ptrdiff_t>;

// parse kind_t::bitfield
template
<
      typename Char
    , typename CharTraits
    , /*std::input_iterator*/ typename It
>
auto try_parse
(
      int_<kind_t::bitfield>
    , boost::spirit::qi::symbols<Char, std::ptrdiff_t> const &items
    , basic_qualified_name<Char, CharTraits> const &enum_name
    , sink_format_options<Char, CharTraits> const &fmt_opt
    , It const &begin
    , It const &end
)
  noexcept -> boost::optional<std::ptrdiff_t>;

// parse kind_t::combo
template
<
      typename Char
    , typename CharTraits
    , /*std::input_iterator*/ typename It
>
auto try_parse
(
      int_<kind_t::combo>
    , boost::spirit::qi::symbols<Char, std::ptrdiff_t> const &items
    , basic_qualified_name<Char, CharTraits> const &enum_name
    , sink_format_options<Char, CharTraits> const &fmt_opt
    , It const &begin
    , It const &end
)
  noexcept -> boost::optional<std::ptrdiff_t>;

///////////////////////////////////////////////////////////////////////////////

// parse kind_t::enum_
template
<
      typename Char
    , typename CharTraits
    , /*std::input_iterator*/ typename It
>
auto parse
(
      int_<kind_t::enum_>
    , boost::spirit::qi::symbols<Char, std::ptrdiff_t> const& items
    , basic_qualified_name<Char, CharTraits> const &enum_name
    , sink_format_options<Char, CharTraits> const &fmt_opt
    , It const &begin
    , It const &end
)
    -> std::ptrdiff_t;

// parse kind_t::bitfield
template
<
      typename Char
    , typename CharTraits
    , /*std::input_iterator*/ typename It
>
auto parse
(
      int_<kind_t::bitfield>
    , boost::spirit::qi::symbols<Char, std::ptrdiff_t> const& items
    , basic_qualified_name<Char, CharTraits> const &enum_name
    , sink_format_options<Char, CharTraits> const &fmt_opt
    , It const &begin
    , It const &end
)
    -> std::ptrdiff_t;

// parse kind_t::combo
template
<
      typename Char
    , typename CharTraits
    , /*std::input_iterator*/ typename It
>
auto parse
(
      int_<kind_t::combo>
    , boost::spirit::qi::symbols<Char, std::ptrdiff_t> const& items
    , basic_qualified_name<Char, CharTraits> const &enum_name
    , sink_format_options<Char, CharTraits> const &fmt_opt
    , It const &begin
    , It const &end
)
    -> std::ptrdiff_t;

}
