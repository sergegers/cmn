#pragma once

#include <algorithm>

namespace cmn
{

////////////////////////////////////////////////////////////////////////////////
//
// Universal copy function
//
////////////////////////////////////////////////////////////////////////////////
namespace detail
{

template <typename T>
struct copy_impl
{
    auto operator ()(T &dst, T const &src) const -> void
    {
        dst = src;
    }
};

template <typename T, std::size_t N_>
struct copy_impl<T[N_]>
{
    auto operator ()(T &dst, T const &src) const -> void
    {
        std::copy(&src[0], &src[N_], &dst[0]);
    }
};

}

template <typename T>
auto copy(T &dst, T const &src) -> void { detail::copy_impl<T> {}(dst, src); }

}
