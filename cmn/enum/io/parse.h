#pragma once

#include <cstddef>
#include <string>
#include <iterator>

#include <boost/optional/optional_fwd.hpp>
#include <boost/spirit/include/qi_symbols.hpp>

#include <cmn/fwd.h>
#include <cmn/meta/type_traits.h>

#include <cmn/util/symbols.h>

#include <cmn/enum/print_t.h>
#include <cmn/enum/detail/qualified_name.h>

namespace cmn::enum_::io
{

template
<
      typename Char
    , typename CharTraits = std::char_traits<Char>
>
struct basic_fmt_specs
{
    using string_type = std::basic_string<Char, CharTraits>;

    string_type     open;
    string_type     separator;
    string_type     close;
    print_t         po;
};

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
    , basic_fmt_specs<Char, CharTraits> const &fmt_specs
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
    , basic_fmt_specs<Char, CharTraits> const &fmt_specs
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
    , basic_fmt_specs<Char, CharTraits> const &fmt_specs
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
    , basic_fmt_specs<Char, CharTraits> const &fmt_specs
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
    , basic_fmt_specs<Char, CharTraits> const &fmt_specs
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
    , basic_fmt_specs<Char, CharTraits> const &fmt_specs
    , It const &begin
    , It const &end
)
    -> std::ptrdiff_t;

}
