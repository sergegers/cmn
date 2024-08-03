#pragma once

#include <locale>
#include <string>

#include <boost/lexical_cast.hpp>

namespace boost
{

///////////////////////////////////////////////////////////////////////////////
//
// lexical casts
//
///////////////////////////////////////////////////////////////////////////////
struct widen
{
    auto operator()(char c) const -> wchar_t
    {
        return std::use_facet<std::ctype<char> >(std::locale{}).widen(c);
    }
};

struct narrow
{
    auto operator()(wchar_t c) const -> char
    {
        return std::use_facet<std::ctype<wchar_t> >(std::locale()).narrow(c, '@');
    }
};

template <> inline auto lexical_cast<char, wchar_t>(wchar_t const &source) -> char
{ return narrow{}(source); }

template <> inline auto lexical_cast<wchar_t, char>(char const &source) -> wchar_t
{ return widen{}(source); }

template <> auto lexical_cast<std::string, std::wstring>(std::wstring const &source) -> std::string;
template <> auto lexical_cast<std::wstring, std::string>(std::string const &source) -> std::wstring;
template <> auto lexical_cast<std::string, std::wstring_view>(std::wstring_view const &source) -> std::string;
template <> auto lexical_cast<std::wstring, std::string_view>(std::string_view const &source) -> std::wstring;

}	

