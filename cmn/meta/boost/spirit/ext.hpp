#pragma once

#include <boost/spirit/home/support/iterators/istream_iterator.hpp>

namespace boost::spirit
{

using wistream_iterator = basic_istream_iterator<wchar_t>;

}
