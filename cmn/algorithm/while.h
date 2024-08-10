#pragma once

#include <type_traits>

#include <boost/mp11.hpp>
#include <boost/mp11/concepts.hpp>
#include <boost/mp11/ext.hpp>

namespace cmn
{

////////////////////////////////////////////////////////////////////////////////
namespace detail
{

using namespace boost::mp11;

template <boost::c::mp11_list L>
struct while_mp11_impl
{
    template <boost::c::mp11_list Rest, typename Pred>
    auto operator ()(std::type_identity<Rest>, Pred &&pred) const -> void
    {
        if constexpr (!mp_empty_v<Rest>)
        {
            using current_type = mp_front<Rest>;
            if (pred(current_type {}))
            {
                using next_rest = mp_pop_front<Rest>;
                (*this)(std::type_identity<next_rest>{}, std::forward<Pred>(pred));
            }            
        }
    }
};

} 

////////////////////////////////////////////////////////////////////////////////
//
// void while_mpl(Pred pred)
//
////////////////////////////////////////////////////////////////////////////////

// TODO: + visitor
template <boost::c::mp11_list L, typename Pred>
auto while_mp11(Pred &&pred) -> void
{
    detail::while_mp11_impl<L> {} (std::type_identity<L>{}, std::forward<Pred>(pred));
}

}
