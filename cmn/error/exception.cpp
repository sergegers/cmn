
#include "exception.h"

namespace cmn::error_
{

auto operator << (std::ostream &ostr, msg_ const &msg) -> std::ostream &
{
    auto const decode_pos = [length = msg.m_msg.length()](std::size_t pos) 
    {
        return pos == std::string::npos?
            pos:
            pos >= length? 
                std::string::npos: pos
        ;
    };

    auto const open = decode_pos(msg.m_open);
    auto const close = decode_pos(msg.m_close);
    if (open == std::string::npos)
    {
        if (close == std::string::npos)
            ostr << msg.m_o << msg.m_msg << msg.m_c;
        else
        {
            ostr << msg.m_msg.substr(0, close);
            ostr << msg.m_o << msg.m_msg.substr(close) << msg.m_c;
        }
    }
    else
    {
        if (open == close)
        {
            ostr << msg.m_msg.substr(0, open);
            ostr << msg.m_o << msg.m_msg.substr(close);                                
        }
        else
        {
            ostr << msg.m_msg.substr(0, open);
            ostr << msg.m_o << msg.m_msg.substr(open, close - open);
            ostr << msg.m_c;
            if (close != std::string::npos) ostr << msg.m_msg.substr(close);
        }
    }

    return ostr;
}

namespace detail
{

auto get_description_(boost::exception const &ex) noexcept -> std::string
{
    using namespace std::string_literals;

    try
    {
        return boost::diagnostic_information(ex, true);
    }
    catch (...)
    {
        return ""s;
    }

    // also see https://stackoverflow.com/questions/48191012/how-to-iterate-over-all-error-infos-in-boostexception
}

auto get_description_(std::exception const &ex) noexcept -> std::string
{
    if (auto const *bex = dynamic_cast<boost::exception const *>(&ex))
        return get_description(*bex);
    else
        return ex.what();
}

}

}
