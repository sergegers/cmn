#pragma once

#include <boost/type_traits/promote.hpp>

#include <cmn/meta/concepts.h>
#include <cmn/meta/type_traits.h>

// specialize unit promotion
namespace boost
{

template <cmn::c::unit T>           struct promote<T>: promote<cmn::underlying_type_t<T>> {};
template <cmn::c::scoped_enum T>    struct promote<T>: promote<cmn::underlying_type_t<T>> {};

}

