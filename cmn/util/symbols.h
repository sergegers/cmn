#pragma once

#include <string>

#include <cmn/util/fixed_string.h>

namespace cmn
{

template <typename Char, typename CharTraits>
constexpr auto to_char(basic_fixed_string<Char, 1, CharTraits> const &str) -> Char
{
    return str[0];
}

template <typename Char, std::size_t N_, typename CharTraits>
constexpr auto to_string(basic_fixed_string<Char, N_, CharTraits> const &str) -> std::basic_string<Char, CharTraits>
{
    return str.data();
}

////////////////////////////////////////////////////////////////////////////////
// symbols
////////////////////////////////////////////////////////////////////////////////
namespace symbols_
{

using namespace literals;

template <typename Char, typename CharTraits = std::char_traits<Char>> struct symbols;

template <>
struct symbols<char>
{
    static constexpr auto whitespace                = " "_fs;
    static constexpr auto nothing                   = ""_fs;
    static constexpr auto endl                      = "\n"_fs;
    static constexpr auto ends                      = "\0"_fs;
    static constexpr auto tab                       = "\t"_fs;
    static constexpr auto open_square_bracket       = "["_fs;
    static constexpr auto close_square_bracket      = "]"_fs;
    static constexpr auto open_angle_bracket        = "<"_fs;
    static constexpr auto close_angle_bracket       = ">"_fs;
    static constexpr auto open_figure_bracket       = "{"_fs;
    static constexpr auto close_figure_bracket      = "}"_fs;
    static constexpr auto open_parenthese           = "("_fs;
    static constexpr auto close_parenthese          = ")"_fs;
    static constexpr auto colon                     = ":"_fs;
    static constexpr auto comma                     = ","_fs;
    static constexpr auto quote                     = "\""_fs;
    static constexpr auto plus                      = "+"_fs;
    static constexpr auto minus                     = "-"_fs;
    static constexpr auto zero                      = "0"_fs;
    static constexpr auto asterisk                  = "*"_fs;
    static constexpr auto ampersand                 = "&"_fs;
    static constexpr auto circumflex                = "^"_fs;
    static constexpr auto octothorpe                = "#"_fs;
    static constexpr auto end                       = "end"_fs;
    static constexpr auto empty                     = "empty"_fs;
    static constexpr auto nullptr_                  = "nullptr"_fs;
    static constexpr auto hex_prefix                = "0x"_fs;
    static constexpr auto hex_postfix               = "h"_fs;
    static constexpr auto scope_resolution          = "::"_fs;
    static constexpr auto left_arrow                = "<-"_fs;
    static constexpr auto right_arrow               = "->"_fs;
    static constexpr auto void_                     = "void"_fs;
    static constexpr auto class_                    = "class"_fs;
    static constexpr auto struct_                   = "struct"_fs;
    static constexpr auto enum_                     = "enum"_fs;
    static constexpr auto a                         = "a"_fs;
    static constexpr auto b                         = "b"_fs;
    static constexpr auto c                         = "c"_fs;
    static constexpr auto h                         = "h"_fs;
    static constexpr auto l                         = "l"_fs;
    static constexpr auto s                         = "s"_fs;
    static constexpr auto t                         = "t"_fs;
    static constexpr auto u                         = "u"_fs;
    static constexpr auto x                         = "x"_fs;
    static constexpr auto vm                        = "vm"_fs;
    static constexpr auto vah                       = "vah"_fs;
    static constexpr auto vap                       = "vap"_fs;
    static constexpr auto vach                      = "vach"_fs;
};

template <>
struct symbols<wchar_t>
{
    static constexpr auto whitespace                = L" "_wfs;
    static constexpr auto nothing                   = L""_wfs;
    static constexpr auto open_square_bracket       = L"["_wfs;
    static constexpr auto endl                      = L"\n"_wfs;
    static constexpr auto ends                      = L"\0"_wfs;
    static constexpr auto tab                       = L"\t"_wfs;
    static constexpr auto close_square_bracket      = L"]"_wfs;
    static constexpr auto open_angle_bracket        = L"<"_wfs;
    static constexpr auto close_angle_bracket       = L">"_wfs;
    static constexpr auto open_curly_bracket        = L"{"_wfs;
    static constexpr auto close_curly_bracket       = L"}"_wfs;
    static constexpr auto open_parenthese           = L"("_wfs;
    static constexpr auto close_parenthese          = L")"_wfs;
    static constexpr auto colon                     = L":"_wfs;
    static constexpr auto comma                     = L","_wfs;
    static constexpr auto quote                     = L"\""_wfs;
    static constexpr auto plus                      = L"+"_wfs;
    static constexpr auto minus                     = L"-"_wfs;
    static constexpr auto zero                      = L"0"_wfs;
    static constexpr auto asterisk                  = L"*"_wfs;
    static constexpr auto ampersand                 = L"&"_wfs;
    static constexpr auto circumflex                = L"^"_wfs;
    static constexpr auto octothorpe                = L"#"_wfs;
    static constexpr auto end                       = L"end"_wfs;
    static constexpr auto empty                     = L"empty"_wfs;
    static constexpr auto nullptr_                  = L"nullptr"_wfs;
    static constexpr auto hex_prefix                = L"0x"_wfs;
    static constexpr auto hex_postfix               = L"h"_wfs;
    static constexpr auto scope_resolution          = L"::"_wfs;
    static constexpr auto left_arrow                = L"<-"_wfs;
    static constexpr auto right_arrow               = L"->"_wfs;
    static constexpr auto void_                     = L"void"_wfs;
    static constexpr auto class_                    = L"class"_wfs;
    static constexpr auto struct_                   = L"struct"_wfs;
    static constexpr auto enum_                     = L"enum"_wfs;
    static constexpr auto a                         = L"a"_wfs;
    static constexpr auto b                         = L"b"_wfs;
    static constexpr auto c                         = L"c"_wfs;
    static constexpr auto h                         = L"h"_wfs;
    static constexpr auto l                         = L"l"_wfs;
    static constexpr auto s                         = L"s"_wfs;
    static constexpr auto t                         = L"t"_wfs;
    static constexpr auto u                         = L"u"_wfs;
    static constexpr auto x                         = L"x"_wfs;
    static constexpr auto vm                        = L"vm"_wfs;
    static constexpr auto vah                       = L"vah"_wfs;
    static constexpr auto vap                       = L"vap"_wfs;
    static constexpr auto vach                      = L"vach"_wfs;
};

}

using symbols_::symbols;

}