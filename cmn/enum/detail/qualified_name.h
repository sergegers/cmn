#pragma once

#include <iosfwd>
#include <type_traits>
#include <string_view>
#include <string>

#include <cmn/meta/concepts.h>
#include <cmn/util/symbols.h>

#include "magic_get.h"

namespace cmn::enum_::detail
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

    template <c::enum_ Enum>
    consteval basic_qualified_name(Enum):
        itself{ basic_magic_enum_name_v<Enum, Char, CharTraits> }
    {}

    constexpr auto class_prefix() const noexcept -> std::basic_string<Char, CharTraits>
    {
        using string_type = std::basic_string<Char, CharTraits>;

        string_type pfx{ m_enum_name };
        pfx += symbols<Char, CharTraits>::scope_resolution.c_str();
        return pfx;
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

private:
    //
    // tag dispatched method family decompose(std::bool_constant<is_scoped_enum>, std::bool_constant<decompose_member_name>)
    //

    // ns0::...nsN::class_prefix::enum_member_name
    consteval auto decompose(std::true_type is_scoped_enum, c::instance_of_bool auto decompose_member_name) -> void
    {
        constexpr std::basic_string_view qualifier = symbols<Char, CharTraits>::scope_resolution;

        auto const pos = m_name.find_last_of(qualifier);
        if (pos == string_view_type::npos)
        {
            m_ns = {};
            m_enum_name = {};
            m_enum_member_name = m_name;
        }
        else
        {
            auto const pos2 = m_name.find_last_of(qualifier, pos - qualifier.length());
            if (pos2 == string_view_type::npos)
            {
                m_ns = {};
                m_enum_name = m_name.substr(0, - 1);
                if constexpr (decompose_member_name.value)
                    m_enum_member_name = m_name.substr(pos + qualifier.length() - 1);
            }
            else
            {
                m_ns = m_name.substr(0, pos2 - 1);
                m_enum_name = m_name.substr(pos2 + qualifier.length() - 1, pos - pos2 - qualifier.length());
                if constexpr (decompose_member_name.value)
                    m_enum_member_name = m_name.substr(pos + qualifier.length() - 1);
            }
        }
    }

    // ns0::...nsN::enum_member_name
    consteval auto decompose(std::false_type is_scoped_enum, c::instance_of_bool auto decompose_member_name) -> void
    {
        constexpr std::basic_string_view qualifier = symbols<Char, CharTraits>::scope_resolution;

        auto const pos = m_name.find_last_of(qualifier);
        if (pos == string_view_type::npos)
        {
            m_ns = {};
            if constexpr (decompose_member_name.value)
                m_enum_member_name = m_name;
        }
        else
        {
            m_ns = m_name.substr(0, pos - 1);
            if constexpr (decompose_member_name.value)
                m_enum_member_name = m_name.substr(pos + qualifier.length() - 1);
        }
    }
public:
    template <c::enum_ auto En_>
    consteval basic_qualified_member_name(int_<En_>):
        m_name{ basic_magic_enum_member_name_v<En_, Char, CharTraits> },
        m_enum_name{ basic_qualified_name<Char, CharTraits>{ En_ }.m_enum_name }
    {
        decompose(std::is_scoped_enum<decltype(En_)>{}, std::true_type{});
    }

    template <c::enum_ auto En_>
    consteval basic_qualified_member_name(int_<En_>, string_view_type enum_member_name):
        m_enum_member_name{ enum_member_name }
    {
        decompose(std::is_scoped_enum<decltype(En_)>{}, std::false_type{});
    }

    friend auto operator << (ostream_type &ostr, itself const &self) -> ostream_type &
    {
        return ostr << self.m_name;
    }
};

using qualified_member_name = basic_qualified_member_name<char>;
using wqualified_member_name = basic_qualified_member_name<wchar_t>;

}
