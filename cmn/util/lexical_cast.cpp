
#if !__has_include(<boost/lexical_cast/string.hpp>)

#include <ranges>

#include "lexical_cast.h"

namespace boost
{

template <>
auto lexical_cast<std::wstring, std::string>(std::string const &source) -> std::wstring
{
    using std::views::transform;
    using std::ranges::to;

    return source | transform(widen{}) | to<std::wstring>();
}

template <>
auto lexical_cast<std::string, std::wstring>(std::wstring const &source) -> std::string
{
    using std::views::transform;
    using std::ranges::to;

    return source | transform(narrow{}) | to<std::string>();
}

template <>
auto lexical_cast<std::wstring, std::string_view>(std::string_view const &source) -> std::wstring
{
    using std::views::transform;
    using std::ranges::to;

    return source | transform(widen{}) | to<std::wstring>();
}

template <>
auto lexical_cast<std::string, std::wstring_view>(std::wstring_view const &source) -> std::string
{
    using std::views::transform;
    using std::ranges::to;

    return source | transform(narrow{}) | to<std::string>();
}

}

#endif