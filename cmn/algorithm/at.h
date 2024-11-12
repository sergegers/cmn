#pragma once

#include <type_traits>

// boost.mp11
#include <boost/mp11.hpp>
#include <boost/mp11/mpl_list.hpp>

// boost.fusion
#include <boost/fusion/sequence/intrinsic/at_c.hpp>
#include <boost/fusion/sequence/intrinsic/empty.hpp>
#include <boost/fusion/concepts.hpp>
// boost.variant
#include <boost/variant.hpp>

#if __has_include(<boost/mp11/concepts.hpp>)
#   include <boost/mp11/concepts.hpp>
#else
#   include <cmn/meta/concepts.h>
#endif

#include <cmn/meta/type_traits.h>    // underlying_type_t<>
#include <cmn/shared/util/assert.h>
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
    requires !boost::mp11::mp_empty<L>::value
    
constexpr auto at_mp11(Idx idx, Visitor &&vis = detail::empty_visitor{})
{
    using namespace boost::mp11;
    using int_type = underlying_type_t<Idx>;

    static constexpr auto size = mp_size<L>::value;
    asserte_msg(idx < size, "Index %1% is out of bounds [0, %2%)", idx, size);

    static auto const tbl = []<int_type... Indices>(std::integer_sequence<int_type, Indices...>)
    {
        using elem_type = mp_apply<boost::variant, L>;
        return std::array<elem_type, sizeof... (Indices)>{ mp_at_c<L, Indices>{}... };
    }
    (std::make_integer_sequence<int_type, size>());

    decltype(auto) elem = tbl[static_cast<int_type>(idx)];
    return boost::apply_visitor(detail::result_{ std::forward<Visitor>(vis) }, elem);
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
        && !boost::fusion::result_of::empty<Sequence>::value

constexpr auto at_fus(Sequence &&seq, Idx idx, Visitor &&vis = detail::empty_visitor{})
{
    namespace fus = boost::fusion;
    using namespace boost::mp11;
    using int_type = underlying_type_t<Idx>;

    static constexpr auto size = boost::fusion::result_of::size<Sequence>::value;
    ASSERTE_MSG_A(idx < size, "Index " << idx << " is out of bounds [0, " << size << ")");

    static auto const tbl = []<int_type... Indices>
        (Sequence && seq, std::integer_sequence<int_type, Indices...>)
        {
            using ref_seq_type = mp_rename<mp_transform<std::add_lvalue_reference_t, Sequence>, mp_list>;
            using elem_type = typename boost::make_variant_over<ref_seq_type>::type;

            return std::array<elem_type, sizeof... (Indices)>{ fus::at_c<Indices>(seq)... };
        }
        (
            std::forward<Sequence>(seq), 
            std::make_integer_sequence<int_type, size>()
        )
    ;

    decltype(auto) elem = tbl[static_cast<int_type>(idx)];
    return  boost::apply_visitor(detail::result_{ std::forward<Visitor>(vis) }, elem);
}

} 
