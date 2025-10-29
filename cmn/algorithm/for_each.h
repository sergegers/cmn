#pragma once

#include <initializer_list>

#include <boost/mp11/concepts.hpp>   // for mpl, mp11 & fusion concepts

#include <cmn/algorithm/detail/result.h>

namespace cmn
{

///////////////////////////////////////////////////////////////////////////////
template <boost::c::mp11_list L, typename Func>
constexpr auto for_each_noctor_mp11(Func &&func) -> decltype(std::forward<Func>(func))
{
    return []<std::size_t... Idss_>(Func &&func, std::index_sequence<Idss_...>) -> decltype(auto)
    {
        std::initializer_list<int>
        {
            (
                detail::type_result_{ std::forward<Func>(func) }
                    .template with_index_arg<L>(std::integral_constant<std::size_t, Idss_>{})
                    (), 
                0) ...
        };

        return std::forward<Func>(func);        
    }
    (
          std::forward<Func>(func)
        , std::make_index_sequence<boost::mp11::mp_size<L>::value>{}
    );
};

}
