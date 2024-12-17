#pragma once

#include <type_traits>

#include "manip.h"

namespace cmn::io
{

namespace manip
{

//
////////////////////////////////////////////////////////////////////////////////

template <typename Manip>
struct slot_manip_forwarder final
{
    template <typename... Args>
    constexpr auto operator ()(Args &&... args) const noexcept(noexcept(Manip{ std::forward<Args>(args)... })) -> Manip
    {
        return Manip{ std::forward<Args>(args)... };
    }
};

// Deduce manipulator type from forwarder argument
template <template <typename> typename ManipT>
struct deduce_slot_manip_forwarder final
{
    template <typename Arg>
    constexpr auto operator ()(Arg &&arg) const
    {
        return ManipT<Arg>{ std::forward<Arg>(arg) };
    }
};

}

using manip::slot_manip_forwarder;
using manip::deduce_slot_manip_forwarder;

}
