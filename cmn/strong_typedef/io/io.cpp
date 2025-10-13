
#include <string_view>
#include <iomanip>
#include <ios>
#include <limits>
#include <cassert>

#include <boost/lexical_cast.hpp>
#include <boost/implicit_cast.hpp>
#include <boost/io/ios_state.hpp>

#include <cmn/meta/concepts.h>
#include <cmn/error/exception.h>
#include <cmn/util/lexical_cast.h>
#include <cmn/util/util.h>
#include <cmn/util/feature.h>

#include <cmn/enum/traits.h>
// ReSharper disable CppUnusedIncludeDirective
#include <cmn/enum/io.h>
// ReSharper restore CppUnusedIncludeDirective
#include "io.h"

namespace cmn::io
{

namespace manip
{

// remain not affected options unchanged
auto override_value(int_fmt_t old, int_fmt_t new_) -> int_fmt_t
{
    int_fmt_t res = old;
    for (auto const mask: enum_::masks_v<int_fmt_t>)
        if (auto const masked_new = get_mask(new_,  mask); has_mask(masked_new))
            res = set_mask(res, masked_new, mask);

    return res;
}

auto int_fmt_storage_t::index(std::ios_base &ios) -> int
{
    // call xalloc once to get an index at which we can store data for this
    // manipulator.
    static auto const idx = std::ios_base::xalloc();
    return idx;
}

auto int_fmt_storage_t::value(std::ios_base &ios) -> keep_type
{
    return ios.iword(index(ios));
}

auto int_fmt_storage_t::value(std::ios_base &ios, keep_type value) -> void
{
    auto const old_value = static_cast<int_fmt_t>(int_fmt_storage_t::value(ios));
    auto const new_value = override_value(old_value, static_cast<int_fmt_t>(value));

    ios.iword(index(ios)) = static_cast<int>(new_value);
}

}

namespace detail
{

template <typename Char, typename CharTraits>
auto make_steam_err_info
(
      std::basic_istream<Char, CharTraits> &istr
    , std::size_t chars_read = 0ul
    , std::size_t length = 100ul
) noexcept
{
    boost::io::basic_ios_exception_saver const _ { istr };
    istr.exceptions(0);

    using pos_type = typename std::basic_istream<Char, CharTraits>::pos_type;
    auto const old_pos = istr.tellg();
    auto const off = std::min(static_cast<pos_type>((length - chars_read) / 2), old_pos);
    istr.seekg(-off, std::ios_base::cur);
    std::basic_string<Char, CharTraits> line { std::istreambuf_iterator{ istr }, {} };
    if (line.length() > length) line.erase(length);
    istr.seekg(old_pos);

    return error_::errinfo_msg
    {
        {
            .m_msg = boost::lexical_cast<std::string>(std::move(line)),
            .m_open = static_cast<std::size_t>(off) - chars_read,
            .m_close = static_cast<std::size_t>(off)
        }
    };
}

template <typename Char, typename CharTraits, Char Char_, Char...  Chars_>
struct default_read_char final
{
    std::basic_istream<Char, CharTraits> &m_istr;

    template <Char C>
    struct char_
    {
        friend decltype(auto) operator << (std::ostream &ostr, char_)
        {
            return ostr << ", " << boost::lexical_cast<char>(C);
        }

        friend decltype(auto) operator << (std::ostream &ostr, char_)
            requires (C == Char_)
        {
            return ostr << boost::lexical_cast<char>(C);
        }
    };

    struct char_list
    {
        friend decltype(auto) operator << (std::ostream &ostr, char_list)
        {
            return ((ostr << char_<Char_>{}) << ... << char_<Chars_>{});
        }
    };

    constexpr explicit default_read_char(std::basic_istream<Char, CharTraits> &istr)
        : m_istr { istr } {}

    [[noreturn]] auto operator ()(Char c) const -> void
    {
        throw format_error{ "Input error. Unknown symbol [{}]. Allowed [{}] only.", c, char_list{} }
            << make_steam_err_info(m_istr, 1);
    }
};

///////////////////////////////////////////////////////////////////////////////
struct write_base final
{
    int_fmt_t const   m_fmt_opt;

    friend decltype(auto) operator << (c::instance_of<std::basic_ostream> auto &ostr, write_base const &bs)
    {
        using enum int_fmt_t;
        if (has_feature(bs.m_fmt_opt, showbase))
        {
            if (has_any_feature(bs.m_fmt_opt, c, asm_))
                ostr << std::noshowbase;
            else
                ostr << std::showbase;
        }
        else if (has_feature(bs.m_fmt_opt, hidebase))
            ostr << std::noshowbase;
        return ostr;
    }
};

//-----------------------------------------------------------------------------

struct read_base final
{
    int_fmt_t const   m_fmt_opt;

    friend decltype(auto) operator >> (c::instance_of<std::basic_istream> auto &istr, read_base bs)
    {
        using enum int_fmt_t;
        if (has_feature(bs.m_fmt_opt, showbase))
        {
            if (has_any_feature(bs.m_fmt_opt, c, asm_))
                istr >> std::noshowbase;
            else
                istr >> std::showbase;                
        }
        else if (has_feature(bs.m_fmt_opt, hidebase))
            istr >> std::noshowbase;

        return istr;
    }
};

///////////////////////////////////////////////////////////////////////////////
template <std::integral Unit>
struct write_sign final
{
    Unit                &m_unit;
    int_fmt_t const     m_fmt_opt;

    write_sign(Unit &unit, int_fmt_t fmt)
        : m_unit { unit },
          m_fmt_opt { fmt } {}

    template
    <
          typename Char
        , typename CharTraits
    >
    friend decltype(auto) operator << (std::basic_ostream<Char, CharTraits> &ostr, write_sign const &pfx)
    {
        using signed_type = std::make_signed_t<Unit>;
        using symbols_type = symbols<Char, CharTraits>;
        using enum int_fmt_t;

        static constexpr auto minus = to_char(symbols_type::minus);
        static constexpr auto space = to_char(symbols_type::whitespace);
        static constexpr auto plus = to_char(symbols_type::plus);
        static constexpr auto endl = to_char(symbols_type::endl);

        if (has_feature(pfx.m_fmt_opt, sign))
        {
            switch (auto const sgn = cmn::sgn(boost::implicit_cast<signed_type>(pfx.m_unit)))
            {
            case -1: ostr << minus; break;
            case  0: ostr << space; break;
            case  1: ostr << plus; break;

            default:
                BOOST_THROW_EXCEPTION(cmn::unexpected{});
            }
        }
        else if (has_feature(pfx.m_fmt_opt, forcesign))
        {
            switch (auto const sgn = cmn::sgn(boost::implicit_cast<signed_type>(pfx.m_unit)))
            {
            case -1: ostr << minus; break;
            case  0:
            case  1: ostr << plus; break;

            default:
                BOOST_THROW_EXCEPTION(cmn::unexpected{});
            }
        }

        return ostr;
    }
};

//-----------------------------------------------------------------------------
template <std::integral Unit>
struct read_sign final
{
    Unit                &m_unit;
    int_fmt_t const     m_fmt_opt;

    read_sign(Unit &unit, int_fmt_t fmt)
        : m_unit { unit },
          m_fmt_opt { fmt } {}

    template
    <
          typename Char
        , typename CharTraits
    >
    friend decltype(auto) operator >> (std::basic_istream<Char, CharTraits> &istr, read_sign pfx)
    {
        using signed_type = std::make_signed_t<Unit>;
        using symbols_type = symbols<Char, CharTraits>;
        using enum int_fmt_t;

        static constexpr auto minus = to_char(symbols_type::minus);
        static constexpr auto space = to_char(symbols_type::whitespace);
        static constexpr auto plus = to_char(symbols_type::plus);
        static constexpr auto endl = to_char(symbols_type::endl);

        if (has_feature(pfx.m_fmt_opt, sign))
        {
            Char sgn;
            do istr >> sgn; while (sgn == endl);

            switch (sgn)
            {
            case minus: pfx.m_unit = static_cast<Unit>(-1); break;
            case space: pfx.m_unit = 0; break;
            case plus: pfx.m_unit = 1; break;

            default:
                BOOST_THROW_EXCEPTION(cmn::unexpected{});
            }
        }
        else if (has_feature(pfx.m_fmt_opt, forcesign))
        {
            Char sgn;
            do istr >> sgn; while (sgn == endl);

            switch (sgn)
            {
            case minus: pfx.m_unit = static_cast<Unit>(-1); break;
            case plus: pfx.m_unit = 1; break;

            default:
                BOOST_THROW_EXCEPTION(cmn::unexpected{});
            }
        }
        return istr;
    }
};

///////////////////////////////////////////////////////////////////////////////
struct write_c_prefix final
{
    int_fmt_t const   m_fmt_opt;

    template
    <
          typename Char
        , typename CharTraits
    >
    friend decltype(auto) operator << (std::basic_ostream<Char, CharTraits> &ostr, write_c_prefix const &pfx)
    {
        using symbols_type = symbols<Char, CharTraits>;
        using enum int_fmt_t;

        if (has_all_features(pfx.m_fmt_opt, c, hex, showbase))
        {
            ostr << symbols_type::hex_prefix;
        }
        return ostr;
    }
};

//-----------------------------------------------------------------------------
struct read_c_prefix final
{
    int_fmt_t const   m_fmt_opt;

    template
    <
          typename Char
        , typename CharTraits
    >
    friend decltype(auto) operator >> (std::basic_istream<Char, CharTraits> &istr, read_c_prefix pfx)
    {
        using symbols_type = symbols<Char, CharTraits>;
        using enum int_fmt_t;

        static constexpr auto endl = to_char(symbols_type::endl);
        static constexpr auto &hex_pfx = symbols_type::hex_prefix;

        if (has_all_features(pfx.m_fmt_opt, c, hex, showbase))
        for (;;)
        {
            Char buf[hex_pfx.size() + 1];
            istr >> buf;
            if (istr.fail())
            {
                istr.clear();
                istr.ignore(std::numeric_limits<std::streamsize>::max(), endl);
            }
            else
            {
                std::basic_string_view<Char, CharTraits> buf_{ buf };
                if (buf_ != hex_pfx) 
                    BOOST_THROW_EXCEPTION(cmn::format_error{ "Stream read operation failed" });

                break;                    
            }
        }
        return istr;
    }
};

///////////////////////////////////////////////////////////////////////////////
struct write_asm_postfix final
{
    int_fmt_t const   m_fmt_opt;

    template
    <
          typename Char
        , typename CharTraits
    >
    friend decltype(auto) operator << (std::basic_ostream<Char, CharTraits> &ostr, write_asm_postfix const &pfx)
    {
        using symbols_type = symbols<Char, CharTraits>;
        using enum int_fmt_t;

        if (has_all_features(pfx.m_fmt_opt, asm_, hex, showbase))
            ostr << symbols_type::hex_postfix;
        return ostr;
    }
};

//-----------------------------------------------------------------------------
struct read_asm_postfix final
{
    int_fmt_t const   m_fmt_opt;

    template
    <
          typename Char
        , typename CharTraits
    >
    friend decltype(auto) operator >> (std::basic_istream<Char, CharTraits> &istr, read_asm_postfix pfx)
    {
        using symbols_type = symbols<Char, CharTraits>;
        using enum int_fmt_t;

        if (has_all_features(pfx.m_fmt_opt, asm_, hex, showbase))
        {
            Char h;
            istr >> h;
            if (h != to_char(symbols_type::hex_postfix)) 
                BOOST_THROW_EXCEPTION(cmn::format_error{ "Stream read operation failed" });
        }
        return istr;
    }
};

///////////////////////////////////////////////////////////////////////////////
struct write_radix final
{
    int_fmt_t const   m_fmt_opt;

    friend decltype(auto) operator << (c::instance_of<std::basic_ostream> auto &ostr, write_radix const &rdx)
    {
        using enum int_fmt_t;
        if (has_feature(rdx.m_fmt_opt, hex))
            ostr << std::hex;
        else if (has_feature(rdx.m_fmt_opt, dec))
            ostr << std::dec;

        return ostr;
    }
};

//-----------------------------------------------------------------------------
struct read_radix final
{
    int_fmt_t const   m_fmt_opt;

    friend decltype(auto) operator >> (c::instance_of<std::basic_istream> auto &istr, read_radix rdx)
    {
        using enum int_fmt_t;
        if (has_feature(rdx.m_fmt_opt, hex))
            istr >> std::hex;
        else if (has_feature(rdx.m_fmt_opt, dec))
            istr >> std::dec;

        return istr;
    }
};

///////////////////////////////////////////////////////////////////////////////
template <std::integral Unit>
struct write_width final
{
    int_fmt_t const   m_fmt_opt;

    write_width(Unit, int_fmt_t fmt): m_fmt_opt { fmt } {}

    template
    <
          typename Char
        , typename CharTraits
    >
    friend decltype(auto) operator << (std::basic_ostream<Char, CharTraits> &ostr, write_width const &w)
    {
        using symbols_type = symbols<Char, CharTraits>;
        using enum int_fmt_t;

        static constexpr auto zero = to_char(symbols_type::zero);
        static constexpr auto hex_digits = std::numeric_limits<std::make_unsigned_t<Unit>>::digits / 4;
        static constexpr auto dec_digits = std::numeric_limits<Unit>::digits10;

        if (has_feature(w.m_fmt_opt, long_))
        {
            if (has_feature(w.m_fmt_opt, dec))
                ostr << std::internal << std::setw(dec_digits) << std::setfill(zero);
            else if (has_feature(w.m_fmt_opt, hex))
                ostr << std::internal << std::setw(hex_digits) << std::setfill(zero);
        }
        else if (has_feature(w.m_fmt_opt, short_))
        {
            ostr.unsetf(std::ios_base::internal);
        }

        return ostr;
    }
};

//-----------------------------------------------------------------------------
template <std::integral Unit>
struct read_width final
{
    int_fmt_t const   m_fmt_opt;

    read_width(Unit, int_fmt_t fmt): m_fmt_opt { fmt } {}

    friend decltype(auto) operator >> (c::instance_of<std::basic_istream> auto &istr, read_width w)
    {
        using enum int_fmt_t;

        static constexpr auto hex_digits = std::numeric_limits<std::make_unsigned_t<Unit>>::digits / 4;
        static constexpr auto dec_digits = std::numeric_limits<Unit>::digits10;

        if (has_feature(w.m_fmt_opt, long_))
        {
            if (has_feature(w.m_fmt_opt, dec))
                istr >> std::internal >> std::setw(dec_digits);
            else if (has_feature(w.m_fmt_opt, hex))
                istr >> std::internal >> std::setw(hex_digits);
        }
        else if (has_feature(w.m_fmt_opt, short_))
        {
            istr.unsetf(std::ios_base::internal);
        }

        return istr;
    }
};

///////////////////////////////////////////////////////////////////////////////
struct write_case final
{
    int_fmt_t const   m_fmt_opt;

    friend decltype(auto) operator << (c::instance_of<std::basic_ostream> auto &ostr, write_case const &cs)
    {
        using enum int_fmt_t;
        if (has_feature(cs.m_fmt_opt, uppercase))
            ostr << std::uppercase;
        else if (has_feature(cs.m_fmt_opt, lowercase))
            ostr << std::nouppercase;

        return ostr;
    }
};

//-----------------------------------------------------------------------------
struct read_case final
{
    int_fmt_t const   m_fmt_opt;

    friend decltype(auto) operator >> (c::instance_of<std::basic_istream> auto &istr, read_case cs)
    {
        using enum int_fmt_t;
        if (has_feature(cs.m_fmt_opt, uppercase))
            istr >> std::uppercase;
        else if (has_feature(cs.m_fmt_opt, lowercase))
            istr >> std::nouppercase;

        return istr;
    }
};

///////////////////////////////////////////////////////////////////////////////
template
<
      typename Char
    , typename CharTraits
    , std::integral Int
>
auto write(std::basic_ostream<Char, CharTraits> &ostr, int_fmt_t fmt, Int const &unit) -> std::basic_ostream<Char, CharTraits> &
{
    boost::io::basic_ios_all_saver const _{ ostr };

    ostr << write_base { fmt } << write_sign{ unit, fmt } << write_c_prefix{ fmt } << write_radix { fmt };
    ostr << write_width { unit, fmt } << write_case { fmt };

    if constexpr (std::signed_integral<Int>)
    {
        using enum int_fmt_t;
        if (has_any_feature(fmt, sign, forcesign) && unit < 0)
        {
            // does not require unary minus
            out_int(ostr, -unit);
        }
        else
            out_int(ostr, unit);
    }
    else
        out_int(ostr, unit);

    return ostr << write_asm_postfix { fmt };
}

template auto write(std::ostream &ostr, int_fmt_t fmt, int const &unit) -> std::ostream &;
template auto write(std::ostream &ostr, int_fmt_t fmt, unsigned const &unit) -> std::ostream &;
template auto write(std::ostream &ostr, int_fmt_t fmt, unsigned char const &unit) -> std::ostream &;

///////////////////////////////////////////////////////////////////////////////
template
<
      typename Char
    , typename CharTraits
    , std::integral Int
>
auto read(std::basic_istream<Char, CharTraits> &istr, int_fmt_t fmt, Int &unit) -> std::basic_istream<Char, CharTraits> &
{
    boost::io::basic_ios_all_saver const _{ istr };

    istr.exceptions(std::ios_base::eofbit | std::ios_base::badbit);    // enable exceptions
    //istr.ignore(std::numeric_limits<std::streamsize>::max(), symbols_type::c_eos());

    istr >> read_base{ fmt } >> read_sign { unit, fmt } >> read_c_prefix { fmt } >> read_radix{ fmt };
    istr >> read_width { unit, fmt } >> read_case{ fmt };

    if constexpr (std::signed_integral<Int>)
    {
        using enum int_fmt_t;

        if (has_any_feature(fmt, sign, forcesign))
        {
            auto const sgn = unit;
            in_int(istr, unit);
            // does not require unary minus
            unit = sgn == static_cast<Int>(-1)? -unit: unit;
        }
        else
        {
            in_int(istr, unit);
        }
    }
    else
        in_int(istr, unit);

    return istr >> read_asm_postfix{ fmt };
}

template auto read(std::istream &istr, int_fmt_t fmt, int &unit) -> std::istream &;
template auto read(std::istream &istr, int_fmt_t fmt, unsigned &unit) -> std::istream &;
template auto read(std::istream &istr, int_fmt_t fmt, unsigned char &unit) -> std::istream &;

//-----------------------------------------------------------------------------
template
<
      typename Char
    , typename CharTraits
>
[[nodiscard]] auto try_read_(std::basic_istream<Char, CharTraits> &istr) noexcept -> boost::optional<std::ptrdiff_t>
{
    boost::io::basic_ios_all_saver const _{ istr };
    istr.exceptions(std::ios_base::goodbit);    // disable exceptions

    std::ptrdiff_t unit;
    istr >> unit;
    if (istr.fail())
    {
        istr.clear();
        return {}; 
    }

    return unit;
}

template auto try_read_(std::istream &istr) noexcept -> boost::optional<std::ptrdiff_t>;
template auto try_read_(std::wistream &istr) noexcept -> boost::optional<std::ptrdiff_t>;

}

}

namespace cmn::enum_::op
{

template auto operator << (std::ostream &, cmn::io::int_fmt_t) -> std::ostream &;
template auto operator << (std::wostream &, cmn::io::int_fmt_t) -> std::wostream &;

}
