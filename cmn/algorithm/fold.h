#pragma once

#include <cstddef>
#include <type_traits>

#include <boost/mp11.hpp>
#include <boost/fusion/sequence/intrinsic/at.hpp>

#if __has_include(<boost/mp11/concepts.hpp>) && __has_include(<boost/fusion/concepts.hpp>)
#   include <boost/mp11/concepts.hpp>
#   include <boost/fusion/concepts.hpp>
#else
#   include <cmn/meta/concepts.h>
#endif

#include <cmn/meta/type_traits.h>         // make_index_sequence_reverse

namespace cmn
{

////////////////////////////////////////////////////////////////////////////////
namespace detail
{

using namespace boost::mp11;

template <boost::c::mp11_list L, typename Fn, std::size_t Idx_>
struct mp11_fold_caller
{
    using itself = mp11_fold_caller<L, Fn, Idx_>;

    Fn &m_func;

    template <typename S>
    constexpr friend auto operator >> (itself const &self, S &&s) -> decltype(auto)
    {
        return self.m_func.template operator ()<mp_at_c<L, Idx_>>(std::forward<S>(s));
    }
};

}	

template <boost::c::mp11_list L, typename State, typename Func>
constexpr auto fold_noctor_mp11(State &&state, Func &&func) -> decltype(auto)
{
    using namespace boost::mp11;

    return []<std::size_t... Idss_>
    (State && state_, Func && func_, std::index_sequence<Idss_...>) constexpr -> decltype(auto)
    {
        return (detail::mp11_fold_caller<L, Func, Idss_>{ func_ } >> ... >> std::forward<State>(state_));
    }
    (
          std::forward<State>(state)
        , std::forward<Func>(func)
        , make_index_sequence_reverse<mp_size<L>::value>{}
    );
}

namespace detail
{

namespace fus = boost::fusion;

////////////////////////////////////////////////////////////////////////////////
// fold_fus_impl()
////////////////////////////////////////////////////////////////////////////////

template <boost::c::fus_sequence Sequence, typename Func, std::size_t Idx_>
struct fus_fold_caller
{
    using itself = fus_fold_caller<Sequence, Func, Idx_>;

    Sequence    &m_seq;
    Func        &m_func;

    template <typename State>
    constexpr friend auto operator >>(itself const &self, State &&state) -> decltype(auto)
    {
        return self.m_func(std::forward<State>(state), fus::at_c<Idx_>(self.m_seq));
    }
};

}   

////////////////////////////////////////////////////////////////////////////////
// decltype(auto) fold(Sequence &&seq, State &&state, Func &&func)
// perfect forwarding fold() version
////////////////////////////////////////////////////////////////////////////////
template <boost::c::fus_sequence Sequence, typename State, typename Func>
constexpr auto fold_fus(Sequence &seq, State &&state, Func &&func) -> decltype(auto)
{
    using namespace boost::mp11;
    namespace fus = boost::fusion;

    return []<std::size_t... Idss_>
    (Sequence & seq_, State && state_, Func && fn, std::index_sequence<Idss_...>) constexpr -> decltype(auto)
    {
        return (detail::fus_fold_caller<Sequence, Func, Idss_>{ seq_, fn } >> ... >> std::forward<State>(state_));
    }
    (
          seq
        , std::forward<State>(state)
        , std::forward<Func>(func)
        , make_index_sequence_reverse<fus::result_of::size<Sequence>::value>{}
    );
}

}
