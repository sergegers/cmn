#pragma once

#include <cstddef>
#include <type_traits>
#include <tuple>

#include <boost/fusion/container/map.hpp>

#include <boost/fusion/support/pair.hpp>
#include <boost/fusion/sequence/intrinsic/at_key.hpp>

#if __has_include(<boost/fusion/type_traits.hpp>)
#   include <boost/fusion/type_traits.hpp>
#else
#   include <cmn/meta/boost/fusion/type_traits.hpp>
#endif

#if __has_include(<boost/fusion/support/ipair.hpp>)
#   include <boost/fusion/support/ipair.hpp>
#else
#   include <cmn/meta/boost/fusion/support/ipair.hpp>
#endif

namespace boost::fusion
{

//-----------------------------------------------------------------------------
template <std::size_t... Keys_, typename... Values>
    requires (sizeof...(Keys_) == sizeof...(Values))
using imap = map<ipair<Keys_, Values>...>;


}

///////////////////////////////////////////////////////////////////////////////
//
// adapt to std::tuple<> interface
//
///////////////////////////////////////////////////////////////////////////////
namespace std
{

template <size_t... Keys_, typename... Values>
struct tuple_size<boost::fusion::imap<Keys_..., Values...>>
    : 
    boost::fusion::result_of::size<boost::fusion::imap<Keys_..., Values...>>
{};

template <size_t Idx_, std::size_t... Keys_, typename... Values>
struct tuple_element<Idx_, boost::fusion::imap<Keys_..., Values...>>
{
    using type = boost::fusion::result_of::value_at_key_t<
        boost::fusion::imap<Keys_..., Values...>
        , boost::fusion::ikey<Idx_>
    >;
};

//-----------------------------------------------------------------------------
template <size_t Idx_, size_t... Keys_, typename... Values>
constexpr decltype(auto) get(boost::fusion::imap<Keys_..., Values...> &t) noexcept
{
    namespace fus = boost::fusion;
    return fus::at_key<fus::ikey<Idx_>>(t);
}

template <size_t Idx_, size_t... Keys_, typename... Values>
constexpr decltype(auto) get(boost::fusion::imap<Keys_..., Values...> const &t) noexcept
{
    namespace fus = boost::fusion;
    return fus::at_key<fus::ikey<Idx_>>(t);
}

template <size_t Idx_, size_t... Keys_, typename... Values>
constexpr decltype(auto) get(boost::fusion::imap<Keys_..., Values...> &&t) noexcept
{
    namespace fus = boost::fusion;
    return move(fus::at_key<fus::ikey<Idx_>>(mnove(t)));
}

}
