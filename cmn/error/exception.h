#pragma once

#include <cstddef>
#include <string>
#include <format>
#include <exception>

#include <boost/exception/all.hpp>

namespace cmn
{

class io_error: public virtual std::exception, public virtual boost::exception 
{
private:
    using inherited = std::exception;
public:
    io_error(char const *what_): inherited{ what_ } {}

    template <typename... Args> io_error(std::format_string<Args...> fmt, Args &&... args):
        inherited{ std::format(fmt, std::forward<Args>(args)...).c_str() }
    {}
};

namespace error
{

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
