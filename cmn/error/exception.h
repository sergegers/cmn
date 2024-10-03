#pragma once

#include <cstddef>
#include <string>
#include <format>
#include <exception>
#include <string>

#include <boost/exception/all.hpp>

namespace cmn
{

template <typename Tag, typename Base = std::exception>
class basic_error: public virtual Base, public virtual boost::exception 
{
private:
    using inherited = Base;

    std::string m_what;
public:
    basic_error(): m_what{ "Unknown exception" } {}
    basic_error(char const *what_): m_what{ what_ } {}

    template <typename... Args>
    basic_error(std::format_string<Args...> fmt, Args &&... args):
        m_what{ std::format(fmt, std::forward<Args>(args)...) }
    {}

    [[nodiscard]] auto what() const -> char const * final
    {
        return m_what.c_str();
    }
};

using not_implemented = basic_error<struct not_implemented_>;
using unexpected = basic_error<struct unexpected_>;
using io_error = basic_error<struct io_error_>;
using format_error = basic_error<struct format_error_>;

namespace error
{

[[nodiscard]] auto get_error_description(boost::exception const &ex) noexcept -> std::string;


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
