#pragma once

#include <utility>
#include <iterator>

namespace cmn
{

////////////////////////////////////////////////////////////////////////////////
//
// Erase elements from the associative container
//
////////////////////////////////////////////////////////////////////////////////
template <typename Pred, typename AssCnt>
constexpr auto erase(AssCnt &cnt, Pred &&pred) -> void
{
    for (auto it = cnt.begin(); it != cnt.end(); )
        if (std::forward<Pred>(pred)(*it)) it = cnt.erase(it); else ++it;
}

template <typename Pred, typename AssCnt, typename Fn>
constexpr auto erase(AssCnt &cnt, Pred &&pred, Fn &&fn) -> void
{
    for (auto it = std::begin(cnt); it != std::end(cnt); )
        if (std::forward<Pred>(pred)(*it))
        {
            std::forward<Fn>(fn)(*it);
            it = cnt.erase(it);
        }
        else { ++it; }
}

}
