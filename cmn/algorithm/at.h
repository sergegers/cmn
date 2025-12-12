#pragma once

#include <type_traits>
#include <array>
#include <stdexcept>
#include <variant>
#include <utility>

// ReSharper disable CppUnusedIncludeDirective

// boost.mp11
#include <boost/mp11.hpp>
#include <boost/mp11/mpl_list.hpp>

// boost.fusion
#include <boost/fusion/sequence/intrinsic/at_c.hpp>

// ReSharper restore CppUnusedIncludeDirective

#if __has_include(<boost/mp11/concepts.hpp>)
#   include <boost/mp11/concepts.hpp>
#else
#   include <cmn/meta/boost/mp11/concepts.hpp>
#endif

#if __has_include(<boost/mp11/type_traits.hpp>)
#   include <boost/mp11/type_traits.hpp>
#else
#   include <cmn/meta/boost/mp11/type_traits.hpp>
#endif

#if __has_include(<boost/fusion/concepts.hpp>)
#   include <boost/fusion/concepts.hpp>
#else
#   include <cmn/meta/boost/fusion/concepts.hpp>
#endif

#if __has_include(<boost/fusion/type_traits.hpp>)
#   include <boost/fusion/type_traits.hpp>
#else
#   include <cmn/meta/boost/fusion/type_traits.hpp>
#endif

#include <cmn/meta/traits.h>    // underlying_type_t<>
#include <cmn/algorithm/detail/result.h>
#include <cmn/algorithm/detail/visitor.h>

namespace cmn
{

////////////////////////////////////////////////////////////////////////////////
// auto at_mp11(std::size_t idx, Visitor &&vis = detail::empty_visitor{}) ->
//   detail::exec_visitor()
//   
// L - Mp11 list, sequence types must be default constructible
// visitor - callable object visitor(elem), meets boost static visitor
// requirements
// 
// Returns: visitor(elem)
////////////////////////////////////////////////////////////////////////////////
template
<
      boost::c::mp11_list L
    , c::enumerable Idx
    , typename Visitor = detail::empty_visitor
>
    requires !boost::mp11::mp_empty_v<L>
    
constexpr auto at_mp11(Idx idx, Visitor &&vis = detail::empty_visitor{})
{
    using namespace boost::mp11;

    auto const int_idx = static_cast<std::size_t>(idx);
    static constexpr auto size = mp_size_v<L>;

    if (int_idx >= size) throw std::out_of_range { "Index is out of bounds" };

    static auto const tbl = []<std::size_t... Indices>(std::index_sequence<Indices...>) constexpr
    {
        using elem_type = mp_apply<std::variant, L>;
        return std::array<elem_type, sizeof... (Indices)>{ mp_at_c<L, Indices>{}... };
    }
    (std::make_index_sequence<size>());

    decltype(auto) elem = tbl[int_idx];
    return std::visit(detail::result_{ std::forward<Visitor>(vis) }, elem);
}

///////////////////////////////////////////////////////////////////////////////
template
<
      typename Sequence
    , c::enumerable Idx
    , typename Visitor
>
    requires
           boost::c::random_access_fus_sequence<std::remove_reference_t<Sequence>>
        && !boost::fusion::result_of::empty_v<Sequence>

constexpr auto at_fus(Sequence &&seq, Idx idx, Visitor &&vis = detail::empty_visitor{})
{
    using namespace boost::mp11;
    namespace fus = boost::fusion;
    namespace rfus = fus::result_of;

    static constexpr auto size = rfus::size_v<Sequence>;
    if (idx >= size) throw std::out_of_range { "Index is out of bounds" };

    static auto const tbl = []<std::size_t... Indices>
        (Sequence && seq, std::index_sequence<Indices...>)
        {
            using ref_seq_type = mp_rename<mp_transform<std::add_lvalue_reference_t, Sequence>, mp_list>;
            using elem_type = mp_apply<std::variant, ref_seq_type>;

            return std::array<elem_type, sizeof... (Indices)>{ fus::at_c<Indices>(seq)... };
        }
        (
            std::forward<Sequence>(seq), 
            std::make_index_sequence<size>()
        )
    ;

    decltype(auto) elem = tbl[static_cast<std::size_t>(idx)];
    return std::visit(detail::result_{ std::forward<Visitor>(vis) }, elem);
}

} 
