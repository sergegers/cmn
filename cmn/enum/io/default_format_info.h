#pragma once

#if __has_include(<boost/fusion/container/imap.hpp>)
#   include <boost/fusion/container/imap.hpp>
#else
#   include <cmn/meta/boost/fusion/container/imap.hpp>
#endif

#include <cmn/meta/concepts.h>

namespace cmn
{



template <c::adapted_enum E>
consteval auto get_default_format_info(E)
{
    namespace fus = boost::fusion;
    return fus::make_imap<fis_options>();
}

}
