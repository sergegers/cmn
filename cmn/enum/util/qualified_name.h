#pragma once

#include <string_view>
#include <iosfwd>

#include <cmn/meta/symbols.h>

namespace cmn::enum_
{

///////////////////////////////////////////////////////////////////////////////

template <typename Char, typename CharTraits = std::char_traits<Char>>
struct basic_qualified_name final
{
    using string_view_type = std::basic_string_view<Char, CharTraits>;
    using ostream_type = std::basic_ostream<Char, CharTraits>;
    using itself = basic_qualified_name;

    string_view_type        m_name;
    string_view_type        m_ns;
    string_view_type        m_enum_name;

    consteval basic_qualified_name(string_view_type name):
        m_name{ name }
    {
        constexpr std::basic_string_view qualifier = symbols<Char, CharTraits>::scope_resolution;

        auto const pos = name.find_last_of(qualifier);
        if (pos == string_view_type::npos)
        {
            m_ns = {};
            m_enum_name = name;
        }
        else
        {
            m_ns = name.substr(0, pos - 1);
            m_enum_name = name.substr(pos + qualifier.length() - 1);
        }
    }

    friend auto operator << (ostream_type &ostr, itself const &self) -> ostream_type &
    {
        return ostr << self.m_name;
    }
};

using qualified_name = basic_qualified_name<char>;
using wqualified_name = basic_qualified_name<wchar_t>;

//-----------------------------------------------------------------------------

template <typename Char, typename CharTraits = std::char_traits<Char>>
struct basic_qualified_member_name final
{
    using string_view_type = std::basic_string_view<Char, CharTraits>;
    using ostream_type = std::basic_ostream<Char, CharTraits>;
    using itself = basic_qualified_member_name;

    string_view_type        m_name;
    string_view_type        m_ns;
    string_view_type        m_enum_name;
    string_view_type        m_enum_member_name;

    consteval basic_qualified_member_name(string_view_type name):
        m_name{ name }
    {
        constexpr std::basic_string_view qualifier = symbols<Char, CharTraits>::scope_resolution;

        auto const pos = name.find_last_of(qualifier);
        if (pos == string_view_type::npos)
        {
            m_ns = {};
            m_enum_name = {};
            m_enum_member_name = name;
        }
        else
        {
            auto const pos2 = name.find_last_of(qualifier, pos - qualifier.length());
            if (pos2 == string_view_type::npos)
            {
                m_ns = {};
                m_enum_name = name.substr(0, - 1);
                m_enum_member_name = name.substr(pos + qualifier.length() - 1);
            }
            else
            {
                m_ns = name.substr(0, pos2 - 1);
                m_enum_name = name.substr(pos2 + qualifier.length() - 1, pos - pos2 - qualifier.length());
                m_enum_member_name = name.substr(pos + qualifier.length() - 1);
            }
        }
    }

    friend auto operator << (ostream_type &ostr, itself const &self) -> ostream_type &
    {
        return ostr << self.m_name;
    }
};

using qualified_member_name = basic_qualified_member_name<char>;
using wqualified_member_name = basic_qualified_member_name<wchar_t>;

}
