#pragma once

#include <concepts>
#include <type_traits>
#include <cstddef>
#include <string>
#include <format>
#include <exception>
#include <string>
#include <string_view>

// ReSharper disable once CppUnusedIncludeDirective
#include <boost/exception/all.hpp>
#include <boost/type_index.hpp>
#include <boost/core/use_default.hpp>

#include <cmn/meta/concepts.h>
#include <cmn/meta/type_traits.h>
#include <cmn/util/param.h>

namespace cmn
{

namespace error_
{

enum class redirect_impl_t
{
    to_std_base,    // redirect ctor args to std::exception base
    to_boost_base   // redirect ctor args to boost::exception base
};

using redirect_to_std_base = int_<redirect_impl_t::to_std_base>;
using redirect_to_boost_base = int_<redirect_impl_t::to_boost_base>;

}

//-----------------------------------------------------------------------------
// forward declaration
template
<
      typename Tag  // to distinguish exception types generated with the same template arguments
    , c::std_only_exception StdBase = std::exception
    , c::boost_only_exception BoostBase = boost::exception
    , error_::redirect_impl_t RedirectImpl = error_::redirect_impl_t::to_boost_base
    , c::complete... Tags   // to catch exceptions with the same tag
>
class basic_error_;

//-----------------------------------------------------------------------------
//
// implement exception
//
//-----------------------------------------------------------------------------
template
<
      typename Tag
    , c::std_only_exception StdBase
    , c::boost_only_exception BoostBase
    , c::complete... Tags
>
class basic_error_<Tag, StdBase, BoostBase, error_::redirect_impl_t::to_std_base, Tags...>:
    public StdBase,
    public BoostBase,
    public Tags...
{
public:
    BOOST_TYPE_INDEX_REGISTER_CLASS

    using StdBase::StdBase;
};

//-----------------------------------------------------------------------------
template
<
      typename Tag
    , c::std_only_exception StdBase
    , c::boost_only_exception BoostBase
    , c::complete... Tags
>
class basic_error_<Tag, StdBase, BoostBase, error_::redirect_impl_t::to_boost_base, Tags...>:
    public StdBase,
    public BoostBase,
    public Tags...
{
public:
    BOOST_TYPE_INDEX_REGISTER_CLASS

    using BoostBase::BoostBase;

    auto what() const noexcept -> char const * override
    {
        return BoostBase::what_();
    }
};

//-----------------------------------------------------------------------------
template
<
      typename Tag
    , c::std_only_exception StdBase
    , c::boost_only_exception BoostBase
    , c::instance_of_enumerable<error_::redirect_impl_t> RedirectImpl
    , c::complete... Tags
>
using basic_error = basic_error_<Tag, StdBase, BoostBase, value_v<RedirectImpl>, Tags...>;

///////////////////////////////////////////////////////////////////////////////

class base_error: public boost::exception
{
private:
    std::string m_what;
protected:
    auto what_() const noexcept -> char const * { return m_what.c_str(); }

public:
    BOOST_TYPE_INDEX_REGISTER_CLASS

    base_error(): m_what{ "Unknown exception" } {}
    base_error(std::string_view what_): m_what{ what_ } {}

    template <typename... Args>
    base_error(std::format_string<Args...> fmt, Args &&... args):
        m_what{ std::format(fmt, std::forward<Args>(args)...) }
    {}
};

//-----------------------------------------------------------------------------
template
<
      typename Tag
    , typename StdBase = boost::use_default         // std::exception
    , typename BoostBase = boost::use_default       // base_error
    , typename RedirectImpl = boost::use_default    // error_::redirect_to_boost_base
    , c::complete... Tags
>
using define_error = decoder_t
<
      basic_error
    , Tag
    , param<StdBase, std::exception>
    , param<BoostBase, base_error>
    , param<RedirectImpl, error_::redirect_to_boost_base>
    , Tags...
>;

#define CMN_DEFINE_ERROR_WITH_TAG(tag)  \
    template    \
    <   \
          typename Tag  \
        , typename StdBase = ::boost::use_default \
        , typename BoostBase = ::boost::use_default \
        , typename RedirectImpl = ::boost::use_default  \
        , ::cmn::c::complete... Tags    \
    >   \
    using define_error = ::cmn::decoder_t \
    <   \
          ::cmn::basic_error    \
        , Tag   \
        , ::cmn::param<StdBase, ::std::exception> \
        , ::cmn::param<BoostBase, ::cmn::base_error>  \
        , ::cmn::param<RedirectImpl, ::cmn::error_::redirect_to_boost_base> \
        , tag   \
        , Tags...   \
    >


//-----------------------------------------------------------------------------
using error = define_error<struct cmn_error_>;
using not_implemented = define_error<struct not_implemented_>;
using unexpected = define_error<struct unexpected_>;
using io_error = define_error<struct io_error_>;

using format_error = define_error<struct format_error_, std::format_error, boost::exception, error_::redirect_to_std_base>;

namespace error_
{

namespace detail
{

[[nodiscard]] auto get_description_(boost::exception const &ex) noexcept -> std::string;
[[nodiscard]] auto get_description_(std::exception const &ex) noexcept -> std::string;

}

template <typename E>
    requires (std::derived_from<E, std::exception> || std::derived_from<E, boost::exception>)
[[nodiscard]] auto get_description(E const &ex) noexcept -> std::string
{
    // properly static dispatch by exception type
    if constexpr (std::derived_from<E, boost::exception>)
        return detail::get_description_(static_cast<boost::exception const &>(ex));
    else
        return detail::get_description_(static_cast<std::exception const &>(ex));
}

struct msg_
{
    std::string m_msg;
    std::size_t m_open  = std::string::npos;    // half opened interval
    std::size_t m_close = std::string::npos;
    char        m_o     = '\xB2';
    char        m_c     = '\xB1';

    friend auto operator << (std::ostream &ostr, msg_ const &msg) -> std::ostream &;
};

using errinfo_msg = boost::error_info<struct errinfo_msg_, msg_>;

}

}
