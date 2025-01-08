
#include <string_view>
#include <iomanip>
#include <ios>

#include <boost/lexical_cast.hpp>

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

auto get_mask(int_fmt_t en) -> mask_type_t<int_fmt_t>
{
    // NOTE: zero value isn't used, so we can get mask from value
    for (auto const mask: enum_::masks_v<int_fmt_t>)
        if (has_value(en, mask))
            return mask;

    return to_mask(int_fmt_t::empty);
}

auto override_value(int_fmt_t orig, int_fmt_t over) -> int_fmt_t
{
    for (auto const mask: enum_::masks_v<int_fmt_t>)
        if (auto const masked_over = value(over,  mask); has_value(masked_over))
            orig = value(orig, masked_over, mask);

    return orig;
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
    // set mask
    auto const fvalue = static_cast<int_fmt_t>(value);
    auto const mask   = get_mask(fvalue);

    auto const old_fvalue = static_cast<int_fmt_t>(int_fmt_storage_t::value(ios));
    auto const new_fvalue = cmn::value(old_fvalue, fvalue, mask);

    auto const new_value = static_cast<keep_type>(0) | to_underlying(new_fvalue);

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
        throw format_error{ "Input error. Unknown symbol [%1%]. Allowed [%2%] only.", c, char_list{} }
            << make_steam_err_info(m_istr, 1);
    }
};

//-----------------------------------------------------------------------------
template
<
      typename Char
    , typename CharTraits
    , std::integral Unit
>
struct reader_ final
{
    using te_reader_type = te_reader<Char, CharTraits, Unit>;

    Unit                    &m_unit;
    int_fmt_t const         m_fmt;
    te_reader_type const    m_rdr;

    reader_(Unit &unit, int_fmt_t fmt, te_reader_type rdr)
        : m_unit { unit },
          m_fmt { fmt },
          m_rdr { rdr } {}

    friend decltype(auto) operator >> (std::basic_istream<Char, CharTraits> &istr, reader_ r)
    {
        return r.m_rdr(istr, r.m_fmt, r.m_unit);
    }    
};

//-----------------------------------------------------------------------------
template
<
      typename Char
    , typename CharTraits
    , std::integral Unit
>
struct writer_ final
{
    using te_writer_type = te_writer<Char, CharTraits, Unit>;

    Unit const              &m_unit;
    int_fmt_t const         m_fmt;
    te_writer_type const    m_wtr;

    constexpr writer_(Unit const &unit, int_fmt_t fmt, te_writer_type wtr)
        : m_unit { unit },
          m_fmt { fmt },
          m_wtr { wtr } {}

    friend decltype(auto) operator << (std::basic_ostream<Char, CharTraits> &ostr, writer_ const &w)
    {
        return w.m_wtr(ostr, w.m_fmt, w.m_unit);
    }
};

//-----------------------------------------------------------------------------
struct base final
{
    template
    <
          typename Char
        , typename CharTraits
    >
    struct base_ final
    {
        std::basic_ios<Char, CharTraits>    &m_ios;
        int_fmt_t const                     m_fmt;

        base_(std::basic_ios<Char, CharTraits> &ios, int_fmt_t fmt)
            : m_ios { ios },
              m_fmt { fmt } {}

        friend decltype(auto) operator << (std::basic_ostream<Char, CharTraits> &ostr, base_ const &bs_)
        {
            using enum int_fmt_t;
            if (has_feature(bs_.m_fmt, showbase))
            {
                if (has_any_feature(bs_.m_fmt, c, asm_))
                    ostr << std::noshowbase;
                else
                    ostr << std::showbase;
            }
            else if (has_feature(bs_.m_fmt, hidebase))
                ostr << std::noshowbase;
            return ostr;
        }

        friend decltype(auto) operator >> (std::basic_istream<Char, CharTraits> &istr, base_ bs_)
        {
            using enum int_fmt_t;
            if (has_feature(bs_.m_fmt, showbase))
            {
                if (has_any_feature(bs_.m_fmt, c, asm_))
                    istr >> std::noshowbase;
                else
                    istr >> std::showbase;                
            }
            else if (has_feature(bs_.m_fmt, hidebase))
                istr >> std::noshowbase;
            return istr;
        }
    };

    int_fmt_t const   m_fmt;

    friend decltype(auto) operator << (c::instance_of<std::basic_ostream> auto &ostr, base const &bs)
    {
        return ostr << base_{ ostr, bs.m_fmt };
    }

    friend decltype(auto) operator >> (c::instance_of<std::basic_istream> auto &istr, base bs)
    {
        return istr >> base_{ istr, bs.m_fmt };
    }
};
//-----------------------------------------------------------------------------
template <std::integral Unit>
struct sign_pfx final
{
    template
    <
          typename Char
        , typename CharTraits
    >
    struct sign_pfx_ final
    {
        using signed_type = std::make_signed_t<Unit>;
        using symbols_type = symbols<Char, CharTraits>;

        static constexpr auto minus = to_char(symbols_type::minus);
        static constexpr auto space = to_char(symbols_type::whitespace);
        static constexpr auto plus = to_char(symbols_type::plus);
        static constexpr auto endl = to_char(symbols_type::endl);

        std::basic_ios<Char, CharTraits>    &m_ios;
        Unit                                &m_unit;
        int_fmt_t const                     m_fmt;

        sign_pfx_(std::basic_ios<Char, CharTraits> &ios, Unit &unit, int_fmt_t fmt)
            : m_ios { ios },
              m_unit { unit },
              m_fmt { fmt } {}

        friend decltype(auto) operator << (std::basic_ostream<Char, CharTraits> &ostr, sign_pfx_ const &pfx_)
        {
            using enum int_fmt_t;
            if (has_feature(pfx_.m_fmt, sign))
            {
                switch (auto const sgn = cmn::sgn(boost::implicit_cast<signed_type>(pfx_.m_unit)))
                {
                case -1: ostr << minus; break;
                case  0: ostr << space; break;
                case  1: ostr << plus; break;

                default:
                    BOOST_THROW_EXCEPTION(cmn::unexpected{});
                }
            }
            else if (has_feature(pfx_.m_fmt, forcesign))
            {
                switch (auto const sgn = cmn::sgn(boost::implicit_cast<signed_type>(pfx_.m_unit)))
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

        friend decltype(auto) operator >> (std::basic_istream<Char, CharTraits> &istr, sign_pfx_ pfx_)
        {
            using enum int_fmt_t;
            if (has_feature(pfx_.m_fmt, sign))
            {
                Char sgn;
                do istr >> sgn; while (sgn == endl);

                switch (sgn)
                {
                case minus: pfx_.m_unit = static_cast<Unit>(-1); break;
                case space: pfx_.m_unit = 0; break;
                case plus: pfx_.m_unit = 1; break;

                default:
                    BOOST_THROW_EXCEPTION(cmn::unexpected{});
                }
            }
            else if (has_feature(pfx_.m_fmt, forcesign))
            {
                Char sgn;
                do istr >> sgn; while (sgn == endl);

                switch (sgn)
                {
                case minus: pfx_.m_unit = static_cast<Unit>(-1); break;
                case plus: pfx_.m_unit = 1; break;

                default:
                    BOOST_THROW_EXCEPTION(cmn::unexpected{});
                }
            }
            return istr;
        }
    };

    Unit                &m_unit;
    int_fmt_t const     m_fmt;

    sign_pfx(Unit &unit, int_fmt_t fmt)
        : m_unit { unit },
          m_fmt { fmt } {}

    friend decltype(auto) operator << (c::instance_of<std::basic_ostream> auto &ostr, sign_pfx const &pfx)
    {
        return ostr << sign_pfx_ { ostr, pfx.m_unit, pfx.m_fmt };
    }

    friend decltype(auto) operator >> (c::instance_of<std::basic_istream> auto &istr, sign_pfx pfx)
    {
        return istr >> sign_pfx_ { istr, pfx.m_unit, pfx.m_fmt };
    }
};

//-----------------------------------------------------------------------------
struct c_pfx final
{
    template
    <
          typename Char
        , typename CharTraits
    >
    struct c_pfx_ final
    {
        using symbols_type = symbols<Char, CharTraits>;
        static constexpr auto endl = to_char(symbols_type::endl);

        std::basic_ios<Char, CharTraits>    &m_ios;
        int_fmt_t const                     m_fmt;

        c_pfx_(std::basic_ios<Char, CharTraits> &ios, int_fmt_t fmt)
            : m_ios { ios },
              m_fmt { fmt } {}

        friend decltype(auto) operator << (std::basic_ostream<Char, CharTraits> &ostr, c_pfx_ const &pfx_)
        {
            using enum int_fmt_t;
            if (has_all_features(pfx_.m_fmt, c, hex, showbase))
            {
                ostr << symbols_type::hex_prefix;
            }
            return ostr;
        }

        friend decltype(auto) operator >> (std::basic_istream<Char, CharTraits> &istr, c_pfx_ pfx_)
        {
            using enum int_fmt_t;
            if (has_all_features(pfx_.m_fmt, c, hex, showbase))
            for (;;)
            {
                Char buf[symbols_type::hex_prefix.size() + 1];
                istr >> buf;
                if (istr.fail())
                {
                    istr.clear();
                    istr.ignore(std::numeric_limits<std::streamsize>::max(), endl);
                }
                else
                {
                    std::basic_string_view<Char, CharTraits> buf_{ buf };
                    if (buf_ != symbols_type::hex_prefix) 
                        BOOST_THROW_EXCEPTION(cmn::format_error{ "Stream read operation failed" });

                    break;                    
                }
            }
            return istr;
        }
    };

    int_fmt_t const   m_fmt;

    friend decltype(auto) operator << (c::instance_of<std::basic_ostream> auto &ostr, c_pfx const &pfx)
    {
        return ostr << c_pfx_{ ostr, pfx.m_fmt };
    }

    friend decltype(auto) operator >> (c::instance_of<std::basic_istream> auto &istr, c_pfx pfx)
    {
        return istr >> c_pfx_{ istr, pfx.m_fmt };
    }
};

//-----------------------------------------------------------------------------
struct asm_pfx final
{
    template
    <
          typename Char
        , typename CharTraits
    >
    struct asm_pfx_ final
    {
        using symbols_type = symbols<Char, CharTraits>;

        std::basic_ios<Char, CharTraits>    &m_ios;
        int_fmt_t const                     m_fmt;

        asm_pfx_(std::basic_ios<Char, CharTraits> &ios, int_fmt_t fmt)
            : m_ios { ios },
              m_fmt { fmt } {}

        friend decltype(auto) operator << (std::basic_ostream<Char, CharTraits> &ostr, asm_pfx_ const &pfx_)
        {
            using enum int_fmt_t;
            if (has_all_features(pfx_.m_fmt, asm_, hex, showbase))
                ostr << symbols_type::hex_postfix;
            return ostr;
        }

        friend decltype(auto) operator >> (std::basic_istream<Char, CharTraits> &istr, asm_pfx_ pfx_)
        {
            using enum int_fmt_t;
            if (has_all_features(pfx_.m_fmt, asm_, hex, showbase))
            {
                Char h;
                istr >> h;
                if (h != to_char(symbols_type::hex_postfix)) 
                    BOOST_THROW_EXCEPTION(cmn::format_error{ "Stream read operation failed" });
            }
            return istr;
        }
    };

    int_fmt_t const   m_fmt;

    friend decltype(auto) operator << (c::instance_of<std::basic_ostream> auto &ostr, asm_pfx const &pfx)
    {
        return ostr << asm_pfx_{ ostr, pfx.m_fmt };
    }

    friend decltype(auto) operator >> (c::instance_of<std::basic_istream> auto &istr, asm_pfx pfx)
    {
        return istr >> asm_pfx_{ istr, pfx.m_fmt };
    }
};

//-----------------------------------------------------------------------------
struct radix final
{
    template
    <
          typename Char
        , typename CharTraits
    >
    struct radix_ final
    {
        std::basic_ios<Char, CharTraits>    &m_ios;
        int_fmt_t const                     m_fmt;

        radix_(std::basic_ios<Char, CharTraits> &ios, int_fmt_t fmt)
            : m_ios { ios },
              m_fmt { fmt } {}

        friend decltype(auto) operator << (std::basic_ostream<Char, CharTraits> &ostr, radix_ const &rdx_)
        {
            using enum int_fmt_t;
            if (has_feature(rdx_.m_fmt, hex))
                ostr << std::hex;
            else if (has_feature(rdx_.m_fmt, dec))
                ostr << std::dec;

            return ostr;
        }

        friend decltype(auto) operator >> (std::basic_istream<Char, CharTraits> &istr, radix_ rdx_)
        {
            using enum int_fmt_t;
            if (has_feature(rdx_.m_fmt, hex))
                istr >> std::hex;
            else if (has_feature(rdx_.m_fmt, dec))
                istr >> std::dec;

            return istr;
        }
    };

    int_fmt_t const   m_fmt;

    friend decltype(auto) operator << (c::instance_of<std::basic_ostream> auto &ostr, radix const &rdx)
    {
        return ostr << radix_{ ostr, rdx.m_fmt };
    }

    friend decltype(auto) operator >> (c::instance_of<std::basic_istream> auto &istr, radix rdx)
    {
        return istr >> radix_{ istr, rdx.m_fmt };
    }
};

//-----------------------------------------------------------------------------
template <std::integral Unit>
struct width final
{
    static constexpr auto hex_digits = 
        std::numeric_limits<std::make_unsigned_t<Unit>>::digits / 4;

    static constexpr auto dec_digits = std::numeric_limits<Unit>::digits10;

    template
    <
          typename Char
        , typename CharTraits
    >
    struct width_ final
    {
        using symbols_type = symbols<Char, CharTraits>;
        static constexpr auto zero = to_char(symbols_type::zero);

        std::basic_ios<Char, CharTraits>    &m_ios;
        int_fmt_t const                     m_fmt;

        width_(std::basic_ios<Char, CharTraits> &ios, int_fmt_t fmt)
            : m_ios { ios },
              m_fmt { fmt } {}

        friend decltype(auto) operator << (std::basic_ostream<Char, CharTraits> &ostr, width_ const &w_)
        {
            using enum int_fmt_t;
            if (has_feature(w_.m_fmt, long_))
            {
                if (has_feature(w_.m_fmt, dec))
                    ostr << std::internal << std::setw(dec_digits) << std::setfill(zero);
                else if (has_feature(w_.m_fmt, hex))
                    ostr << std::internal << std::setw(hex_digits) << std::setfill(zero);
            }
            else if (has_feature(w_.m_fmt, short_))
            {
                ostr.unsetf(std::ios_base::internal);
            }

            return ostr;
        }

        friend decltype(auto) operator >> (std::basic_istream<Char, CharTraits> &istr, width_ w_)
        {
            using enum int_fmt_t;
            if (has_feature(w_.m_fmt, long_))
            {
                if (has_feature(w_.m_fmt, dec))
                    istr >> std::internal >> std::setw(dec_digits);
                else if (has_feature(w_.m_fmt, hex))
                    istr >> std::internal >> std::setw(hex_digits);
            }
            else if (has_feature(w_.m_fmt, short_))
            {
                istr.unsetf(std::ios_base::internal);
            }

            return istr;
        }
    };

    int_fmt_t const   m_fmt;

    width(Unit, int_fmt_t fmt): m_fmt { fmt } {}

    friend decltype(auto) operator << (c::instance_of<std::basic_ostream> auto &ostr, width const &w)
    {
        return ostr << width_{ ostr, w.m_fmt };
    }

    friend decltype(auto) operator >> (c::instance_of<std::basic_istream> auto &istr, width w)
    {
        return istr >> width_{ istr, w.m_fmt };
    }
};

//-----------------------------------------------------------------------------
struct case_ final
{
    template
    <
          typename Char
        , typename CharTraits
    >
    struct case__ final
    {
        std::basic_ios<Char, CharTraits>    &m_ios;
        int_fmt_t const                     m_fmt;

        case__(std::basic_ios<Char, CharTraits> &ios, int_fmt_t fmt)
            : m_ios { ios },
              m_fmt { fmt } {}

        friend decltype(auto) operator << (std::basic_ostream<Char, CharTraits> &ostr, case__ const &cs_)
        {
            using enum int_fmt_t;
            if (has_feature(cs_.m_fmt, uppercase))
                ostr << std::uppercase;
            else if (has_feature(cs_.m_fmt, lowercase))
                ostr << std::nouppercase;

            return ostr;
        }

        friend decltype(auto) operator >> (std::basic_istream<Char, CharTraits> &istr, case__ cs_)
        {
            using enum int_fmt_t;
            if (has_feature(cs_.m_fmt, uppercase))
                istr >> std::uppercase;
            else if (has_feature(cs_.m_fmt, lowercase))
                istr >> std::nouppercase;

            return istr;
        }
    };

    int_fmt_t const   m_fmt;

    friend decltype(auto) operator << (c::instance_of<std::basic_ostream> auto &ostr, case_ const &cs)
    {
        return ostr << case__{ ostr, cs.m_fmt };
    }

    friend decltype(auto) operator >> (c::instance_of<std::basic_istream> auto &istr, case_ cs)
    {
        return istr >> case__{ istr, cs.m_fmt };
    }
};

//-----------------------------------------------------------------------------
template
<
      typename Char
    , typename CharTraits
    , std::integral Unit
>
auto write(std::basic_ostream<Char, CharTraits> &ostr, int_fmt_t fmt, te_writer<Char, CharTraits, Unit> wtr, Unit const &unit)->
    std::basic_ostream<Char, CharTraits> &
{
    boost::io::basic_ios_all_saver const _{ ostr };

    ostr << base{ fmt } << sign_pfx{ unit, fmt } << c_pfx{ fmt } << radix{ fmt };
    ostr << width{ unit, fmt } << case_{ fmt } << writer_{ unit, fmt, wtr } << asm_pfx{ fmt };
    return ostr;    
}

template auto write(std::ostream &ostr, int_fmt_t fmt, te_writer<char, std::char_traits<char>, int> wtr, int const &unit) -> std::ostream &;
template auto write(std::ostream &ostr, int_fmt_t fmt, te_writer<char, std::char_traits<char>, unsigned> wtr, unsigned const &unit) -> std::ostream &;
template auto write(std::ostream &ostr, int_fmt_t fmt, te_writer<char, std::char_traits<char>, unsigned char> wtr, unsigned char const &unit) -> std::ostream &;

template
<
      typename Char
    , typename CharTraits
    , std::integral Unit
>
auto read(std::basic_istream<Char, CharTraits> &istr, int_fmt_t fmt, te_reader<Char, CharTraits, Unit> rdr, Unit &unit)->
    std::basic_istream<Char, CharTraits> &
{
    boost::io::basic_ios_all_saver const _{ istr };
    istr.exceptions(std::ios_base::eofbit | std::ios_base::badbit);    // enable exceptions
    //istr.ignore(std::numeric_limits<std::streamsize>::max(), symbols_type::c_eos());

    istr >> base{ fmt } >> sign_pfx{ unit, fmt } >> c_pfx{ fmt } >> radix{ fmt };
    istr >> width { unit, fmt } >> case_{ fmt } >> reader_{ unit, fmt, rdr } >> asm_pfx{ fmt };
    return istr;    
}

template auto read(std::istream &istr, int_fmt_t fmt, te_reader<char, std::char_traits<char>, int> rdr, int &unit) -> std::istream &;
template auto read(std::istream &istr, int_fmt_t fmt, te_reader<char, std::char_traits<char>, unsigned> rdr, unsigned &unit) -> std::istream &;
template auto read(std::istream &istr, int_fmt_t fmt, te_reader<char, std::char_traits<char>, unsigned char> rdr, unsigned char &unit) -> std::istream &;

[[nodiscard]] auto try_read_(c::instance_of<std::basic_istream> auto &istr) noexcept -> boost::optional<std::ptrdiff_t>
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

}
