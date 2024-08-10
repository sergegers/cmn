#pragma once

#include <string_view>

namespace cmn::enum_
{

////////////////////////////////////////////////////////////////////////////////////////////////
//
// "magic get" utilities
//
////////////////////////////////////////////////////////////////////////////////////////////////
#define CMN_WIDE2(x) L##x
#define CMN_WIDE1(x) CMN_WIDE2(x)
#define WFUNCSIG CMN_WIDE1(__FUNCSIG__)

consteval auto get_enum_name(c::enum_ auto) -> std::string_view
{
    using namespace std::string_view_literals;

    std::string_view funcsig = __FUNCSIG__;

    // <enum xxx>
    constexpr auto prefix = "<enum "sv;
    auto const l = funcsig.find_last_of('<') + prefix.size();

    constexpr auto postix= ">"sv;
    auto const r = funcsig.find_last_of(postix);

    auto const res = funcsig.substr(l, r - l);
    return res;
}

consteval auto get_enum_wname(c::enum_ auto) -> std::wstring_view
{
    using namespace std::string_view_literals;

    std::wstring_view funcsig = WFUNCSIG;

    // <enum xxx>
    constexpr auto prefix = L"<enum "sv;
    auto const l = funcsig.find_last_of('<') + prefix.size();

    constexpr auto postix= L">"sv;
    auto const r = funcsig.find_last_of(postix);

    auto const res = funcsig.substr(l, r - l);
    return res;
}

//-----------------------------------------------------------------------------
template <c::enum_ auto En_>
consteval auto get_enum_member_name() -> std::string_view
{
    using namespace std::string_view_literals;

    std::string_view funcsig = __FUNCSIG__;

    // auto __cdecl enum_member_name<e0>(void)
    constexpr auto prefix = "<"sv;
    auto const l = funcsig.find_last_of(prefix) + prefix.size();

    constexpr auto postix= ">"sv;
    auto const r = funcsig.find_last_of(postix);

    auto const res = funcsig.substr(l, r - l);
    return res;
}

template <c::enum_ auto En_>
consteval auto get_enum_member_wname() -> std::wstring_view
{
    using namespace std::string_view_literals;

    std::wstring_view funcsig = WFUNCSIG;

    // auto __cdecl enum_member_wname<e0>(void)
    constexpr auto prefix = L"<"sv;
    auto const l = funcsig.find_last_of(prefix) + prefix.size();

    constexpr auto postix= L">"sv;
    auto const r = funcsig.find_last_of(postix);

    auto const res = funcsig.substr(l, r - l);
    return res;
}

#undef CMN_WIDE1
#undef CMN_WIDE2
#undef WFUNCSIG

}
