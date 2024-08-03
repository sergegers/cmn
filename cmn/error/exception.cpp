
#include "exception.h"

namespace cmn::error
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

}