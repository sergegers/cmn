#pragma once

// ReSharper disable once CppUnusedIncludeDirective
#include <string_view>

#include <cmn/meta/traits.h>   // int

////////////////////////////////////////////////////////////////////////////////////////////////
//
// "magic get" utilities
//
////////////////////////////////////////////////////////////////////////////////////////////////
namespace cmn::enum_ ::detail
{

#define CMN_WIDE2(x) L##x
#define CMN_WIDE1(x) CMN_WIDE2(x)
#define WFUNCSIG CMN_WIDE1(__FUNCSIG__)

consteval auto magic_get_enum_name(c::enum_ auto, char, std::char_traits<char>) -> std::string_view
{
    using namespace std::string_view_literals;

    // class std::basic_string_view<char,struct std::char_traits<char> > __cdecl cmn::enum_::detail::magic_get_enum_name<enum x>(enum cmn::x,char,struct std::char_traits<char>)
    std::string_view funcsig = __FUNCSIG__;

    constexpr auto prefix = "<enum "sv;
    auto const l = funcsig.find(prefix) + prefix.size();

    constexpr auto postix= ">"sv;
    auto const r = funcsig.find(postix, l);

    auto const res = funcsig.substr(l, r - l);
    return res;
}

consteval auto magic_get_enum_name(c::enum_ auto, wchar_t, std::char_traits<wchar_t>) -> std::wstring_view
{
    using namespace std::string_view_literals;

    // class std::basic_string_view<wchar_t,struct std::char_traits<wchar_t> > __cdecl cmn::enum_::detail::magic_get_enum_name<enum x>(enum cmn::x,wchar_t,struct std::char_traits<wchar_t>)
    std::wstring_view funcsig = WFUNCSIG;

    constexpr auto prefix = L"<enum "sv;
    auto const l = funcsig.find(prefix) + prefix.size();

    constexpr auto postix= L">"sv;
    auto const r = funcsig.find(postix, l);

    auto const res = funcsig.substr(l, r - l);
    return res;
}

///////////////////////////////////////////////////////////////////////////////
template <c::enum_ auto En_>
consteval auto magic_get_enum_member_name(int_<En_>, char, std::char_traits<char>) -> std::string_view
{
    using namespace std::string_view_literals;

    std::string_view funcsig = __FUNCSIG__;

    // scoped enum
    // class std::basic_string_view<char,struct std::char_traits<char> > __cdecl cmn::enum_::detail::magic_get_enum_member_name<cmn::x::x0>(struct std::integral_constant<enum cmn::x,0>,char,struct std::char_traits<char>)
    // non scoped enum
    // class std::basic_string_view<char,struct std::char_traits<char> > __cdecl cmn::enum_::detail::magic_get_enum_member_name<cmn::x0>(struct std::integral_constant<enum cmn::x,0>,char,struct std::char_traits<char>)
    constexpr auto prefix = "magic_get_enum_member_name<"sv;
    auto const l = funcsig.find(prefix) + prefix.size();

    constexpr auto postix= ">"sv;
    auto const r = funcsig.find(postix, l);

    auto const res = funcsig.substr(l, r - l);
    return res;
}

template <c::enum_ auto En_>
consteval auto magic_get_enum_member_name(int_<En_>, wchar_t, std::char_traits<wchar_t>) -> std::wstring_view
{
    using namespace std::string_view_literals;

    std::wstring_view funcsig = WFUNCSIG;

    // scoped enum
    // class std::basic_string_view<wchar_t,struct std::char_traits<wchar_t> > __cdecl cmn::enum_::detail::magic_get_enum_member_name<cmn::x::x0>(struct std::integral_constant<enum cmn::x,0>,wchar_t,struct std::char_traits<wchar_t>)
    // non scoped enum
    // class std::basic_string_view<wchar_t,struct std::char_traits<wchar_t> > __cdecl cmn::enum_::detail::magic_get_enum_member_name<cmn::x0>(struct std::integral_constant<enum cmn::x,0>,wchar_t,struct std::char_traits<wchar_t>)
    constexpr auto prefix = L"magic_get_enum_member_name<"sv;
    auto const l = funcsig.find(prefix) + prefix.size();

    constexpr auto postix= L">"sv;
    auto const r = funcsig.find(postix, l);

    auto const res = funcsig.substr(l, r - l);
    return res;
}

///////////////////////////////////////////////////////////////////////////////

template <c::enum_ Enum, typename Char, typename CharTraits>
constexpr auto basic_magic_enum_name_v = magic_get_enum_name(Enum{}, Char{}, CharTraits{});

template <c::enum_ Enum>
constexpr auto magic_enum_name_v = basic_magic_enum_name_v<Enum, char, std::char_traits<char>>;

template <c::enum_ Enum>
constexpr auto magic_enum_wname_v = basic_magic_enum_name_v<Enum, wchar_t, std::char_traits<wchar_t>>;


//-----------------------------------------------------------------------------
template <c::enum_ auto En_, typename Char, typename CharTraits>
constexpr auto basic_magic_enum_member_name_v = magic_get_enum_member_name(int_<En_>{}, Char{}, CharTraits{});

template <c::enum_ auto En_>
constexpr auto magic_enum_member_name_v = basic_magic_enum_member_name_v<En_, char, std::char_traits<char>>;

template <c::enum_ auto En_>
constexpr auto magic_enum_member_wname_v = basic_magic_enum_member_name_v<En_, wchar_t, std::char_traits<wchar_t>>;

#undef CMN_WIDE1
#undef CMN_WIDE2
#undef WFUNCSIG

}
