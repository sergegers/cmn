#pragma once

#include <type_traits>

#if __has_include(<boost/mp11/type_traits.hpp>)
#   include <boost/mp11/type_traits.hpp>
#else
#   include <cmn/meta/boost/mp11/type_traits.hpp>
#endif

#if __has_include(<boost/fusion/type_traits.hpp>)
#   include <boost/fusion/type_traits.hpp>
#else
#   include <cmn/meta/boost/fusion/type_traits.hpp>
#endif

#include <cmn/meta/concepts.h>

#include <cmn/algorithm/detail/visitor.h>
#include <cmn/algorithm/detail/result.h>

namespace cmn
{

namespace detail
{

namespace mpl = boost::mpl;
namespace fus = boost::fusion;

////////////////////////////////////////////////////////////////////////////////
//
// find_if_fus_impl()
// NOTE: fusion iterators could have the different types but be equal.
// Compare them by the operator == and "metacompare" them by the equal_to 
// metafunction.
//
////////////////////////////////////////////////////////////////////////////////
template
<
      typename BeginIter
    , typename Iterator
    , typename EndIter
>
struct find_if_fus_impl
{
    using itself = find_if_fus_impl;

    std::ptrdiff_t	const   m_index;
    BeginIter               m_begin;
    Iterator                m_it;
    EndIter                 m_end;

    find_if_fus_impl
    (
        std::ptrdiff_t const index, 
        BeginIter begin,
        Iterator it,
        EndIter end
    ) noexcept : 
        m_index { index }, 
        m_begin{ begin },
        m_it{ it },
        m_end{ end }
    {}
    find_if_fus_impl(itself const &) = delete;
    find_if_fus_impl(itself &&) = delete;

    auto operator =(itself const &) -> itself& = delete;
    auto operator =(itself &&) -> itself&      = delete;

    template <typename Pred, typename Visitor>
    auto operator ()(Pred &&pred, Visitor &&visitor) const
    {
        auto &&cur = *m_it;
        if (pred(cur))
        {
            return
                result_{ std::forward<Visitor>(visitor) }
                    .with_args(std::forward<decltype(cur)>(cur))
                    .with_index(m_index)
                ()
            ;
        }
        else
        {
            auto const next_it = fus::next(m_it);
            find_if_fus_impl<BeginIter, decltype(next_it), EndIter> const
                next_ { m_index + 1, m_begin, next_it, m_end };
            return next_ (std::forward<Pred>(pred), std::forward<Visitor>(visitor));
        }
    }
};

template
<
      typename BeginIter
    , typename Iterator
    , typename EndIter
>
    requires fus::result_of::equal_to_v<Iterator, EndIter>

struct find_if_fus_impl<BeginIter, Iterator, EndIter>
{
    using itself = find_if_fus_impl;

    find_if_fus_impl
    (
        std::ptrdiff_t /*index*/,
        BeginIter /*begin*/,
        Iterator /*it*/,
        EndIter /*end*/
    ) noexcept
    {}
    find_if_fus_impl(itself const &) = delete;
    find_if_fus_impl(itself &&) = delete;

    auto operator = (itself const &) -> itself& = delete;
    auto operator = (itself &&) -> itself&      = delete;

    template <typename Pred, typename Visitor>
    auto operator ()(Pred &&, Visitor &&visitor) const noexcept
    {
        // use first sequence element to compute a visitor
        // result type or use a part of the result_of protocol
        // if the sequence is empty
        using any_type = typename mpl::eval_if_c
        <
            std::is_same_v<BeginIter, EndIter>,
            vis_result<std::decay_t<Visitor>>,
            fus::result_of::deref<BeginIter>
        >::type;
        return
            result_{ std::forward<Visitor>(visitor) }
                .template type_args_not_found<any_type>()
                .with_index()
            ()
        ;
    }
};

////////////////////////////////////////////////////////////////////////////////
//
// iter_find_if_fus_impl()
// NOTE: fusion iterators could have the different types but be equal.
// Compare them by the operator == and "metacompare" them by the equal_to 
// metafunction.
//
////////////////////////////////////////////////////////////////////////////////
template
<
	typename BeginIter,
	typename Iterator,
	typename EndIter
>
struct iter_find_if_fus_impl
{
    using itself = iter_find_if_fus_impl;
    
    std::ptrdiff_t const    m_index;
    BeginIter               m_begin;
    Iterator                m_it;
    EndIter                 m_end;
    
    iter_find_if_fus_impl
    (
        std::ptrdiff_t const index,
        BeginIter begin,
        Iterator it,
        EndIter end
    ) :
        m_index{ index },
        m_begin{ begin },
        m_it{ it },
        m_end{ end }
    {}
    iter_find_if_fus_impl(itself const &) = delete;
    iter_find_if_fus_impl(itself &&) = delete;

    auto operator =(itself const &) -> itself& = delete;
    auto operator =(itself &&) -> itself&      = delete;
    
    template <typename Pred, typename Visitor>
    auto operator ()(Pred &&pred, Visitor &&visitor) const
    {
        if (pred(m_it))
        {
            return result_{ std::forward<Visitor>(visitor) }
                    .with_args(m_it)
                    .with_index(m_index)
                ()
            ;
        }
        else
        {
            auto next_it = fus::next(m_it);
            iter_find_if_fus_impl<BeginIter, decltype(next_it), EndIter>
                next_{ m_index + 1, m_begin, next_it, m_end };
            return next_(std::forward<Pred>(pred), std::forward<Visitor>(visitor));
        }
    }

    template <typename PredVisitor>
    auto operator ()(PredVisitor &&pred_vis) const
    {
        if (pred_vis(m_it))
        {
            return result_{ std::forward<PredVisitor>(pred_vis) }
                    .with_args(algo::visit, std::forward<PredVisitor>(pred_vis))
                    .with_index(m_index)
                ()
            ;
        }
        else
        {
            auto next_it = fus::next(m_it);
            iter_find_if_fus_impl<BeginIter, decltype(next_it), EndIter>
                next_{ m_index + 1, m_begin, next_it, m_end };
            return next_(std::forward<PredVisitor>(pred_vis));
        }
    }
};

template
<
	typename BeginIter,
	typename Iterator,
	typename EndIter
>
    requires fus::result_of::equal_to_v<Iterator, EndIter>

struct iter_find_if_fus_impl<BeginIter, Iterator, EndIter>
{
    using itself = iter_find_if_fus_impl;
    
    EndIter                 m_end;
    
    iter_find_if_fus_impl
    (
        std::ptrdiff_t const /*index*/,
        BeginIter /*begin*/,
        Iterator /*it*/,
        EndIter end
    ):
        m_end{ end }
    {}
    iter_find_if_fus_impl(itself const &) = delete;
    iter_find_if_fus_impl(itself &&) = delete;

    auto operator = (itself const &) -> itself & = delete;
    auto operator = (itself &&) -> itself &      = delete;
    
    template <typename Pred, typename Visitor>
    auto operator ()(Pred &&, Visitor &&visitor) const
    {
        return result_{ std::forward<Visitor>(visitor) }
                .with_args(m_end)
                .with_index()
            ()
        ;
    }
    
    template <typename PredVisitor>
    auto operator ()(PredVisitor &&pred_vis) const
    {
        return result_{ std::forward<PredVisitor>(pred_vis) }
                .with_args(algo::visit, m_end)
                .with_index()
            ()
        ;
    }
};

template <typename T>
struct find_pred
{
    T m_t;

    template <typename Arg>
    find_pred(Arg &&t) : m_t { std::forward<Arg>(t) } {}

    template <typename Current>
    auto operator ()(Current const &current) const -> bool
    {
        return m_t == current;
    }
};

////////////////////////////////////////////////////////////////////////////////
//
// find_if_mp11_impl
//
////////////////////////////////////////////////////////////////////////////////
template <boost::c::mp11_list L>
struct find_if_mp11_impl final
{
    using itself = find_if_mp11_impl;

    std::ptrdiff_t const		m_index;

    explicit find_if_mp11_impl(std::ptrdiff_t index) : m_index { index } {}
    find_if_mp11_impl(itself const &) = delete;
    find_if_mp11_impl(itself &&)      = delete;

    auto operator =(itself const &) -> itself & = delete;
    auto operator =(itself &&) -> itself &      = delete;

    template <boost::c::mp11_list Rest, typename Pred, typename Visitor>
    auto operator ()(std::type_identity<Rest>, Pred &&pred, Visitor &&visitor) const
    {
        using namespace boost::mp11;

        using current_type = mp_first<Rest>;
        current_type ct{};
        if (pred(ct))
        {
            return result_{ std::forward<Visitor>(visitor) }
                        .with_args(std::forward<current_type>(ct))
                        .with_index(m_index)
                ()
            ;
        }
        else
        {
            using next_rest_id = std::type_identity<mp_rest<Rest>>;
            find_if_mp11_impl<L> next_{ m_index + 1 };
            return next_(next_rest_id{}, std::forward<Pred>(pred), std::forward<Visitor>(visitor));
        }
    }


    template <boost::c::mp11_list Rest, typename Pred, typename Visitor>
        requires mp_empty_v<Rest>

    auto operator ()(std::type_identity<Rest>, Pred &&/*pred*/, Visitor &&visitor) const
    {
        using namespace boost::mp11;

        // use first sequence element to compute a visitor
        // result type or use a part of the result_of protocol
        // if the sequence is empty
        if constexpr (mp_empty_v<L>)
        {
            return result_{ std::forward<Visitor>(visitor) }
                    .vis_result_not_found()
                    .with_index()
                ()
            ;
        }
        else
        {
            using result_type = mp_front<L>;
            return result_{ std::forward<Visitor>(visitor) }
                    .template type_args_not_found<result_type>()
                    .with_index()
                ()
            ;
        }
    }
};

///////////////////////////////////////////////////////////////////////////////
//
// find_if_noctor_mp11_impl
//
///////////////////////////////////////////////////////////////////////////////
template <boost::c::mp11_list L>
struct find_if_noctor_mp11_impl final
{
    using itself = find_if_noctor_mp11_impl;

    std::ptrdiff_t const		m_index;

    find_if_noctor_mp11_impl(std::ptrdiff_t const index) noexcept : m_index { index } {}
    find_if_noctor_mp11_impl(itself const &) = delete;
    find_if_noctor_mp11_impl(itself &&)      = delete;

    auto operator = (itself const &) -> itself & = delete;
    auto operator = (itself &&) -> itself&       = delete;

    template <boost::c::mp11_list Rest, typename Pred, typename Visitor>
    auto operator ()(std::type_identity<Rest>, Pred &&pred, Visitor &&visitor) const
    {
        using namespace boost::mp11;

        using current_type = mp_first<Rest>;
        if (pred.template operator ()<current_type>())
        {
            return
                type_result_{ std::forward<Visitor>(visitor) }
                    .template with_args<current_type>()
                    .with_index(m_index)
                ()
            ;
        }
        else
        {
            using next_rest_id = std::type_identity<mp_rest<Rest>>;
            itself next_{ m_index + 1 };
            return next_(next_rest_id{}, std::forward<Pred>(pred), std::forward<Visitor>(visitor));
        }
    }

    template <boost::c::mp11_list Rest, typename Pred, typename Visitor>
        requires mp_empty_v<Rest>
    auto operator ()(std::type_identity<Rest>, Pred &&/*pred*/, Visitor &&visitor) const
    {
        using namespace boost::mp11;

        // use first sequence element to compute a visitor
        // result type or use a part of the result_of protocol
        // if the sequence is empty
        if constexpr (mp_empty_v<L>)
        {
            return type_result_{ std::forward<Visitor>(visitor) }
                    .vis_result_not_found()
                    .with_index()
                ()
            ;
        }
        else
        {
            using result_type = mp_front<L>;
            return type_result_ { std::forward<Visitor>(visitor) }
                    .template args_not_found<result_type>()
                    .with_index()
                ()
            ;
        }
    }
};

}	

////////////////////////////////////////////////////////////////////////////////
//
// auto find_if_fus(Sequence &&seq, Pred &&pred, Visitor &&visitor = detail::empty_visitor {}) ->
//   detail::exec_ctor_visitor()
//   
// seq - Fusion sequence
// pred - callable object pred(elem)
// visitor - callable object visitor(elem)
// 
// Stops the search if pred(at_c<Pos>(seq)) == true.
// 
// Returns: std::pair<decltype(visitor(elem)), int> where int is poisition or
// int position if the decltype(visitor(elem)) == void
//
////////////////////////////////////////////////////////////////////////////////
template <typename Sequence, typename Pred, typename Visitor = detail::empty_visitor>
    requires boost::c::fus_sequence<std::remove_reference_t<Sequence>>

auto find_if_fus(Sequence &&seq, Pred &&pred, Visitor &&visitor = detail::empty_visitor {})
{
    namespace fus = boost::fusion;

    auto begin = fus::begin(seq);
    auto end = fus::end(seq);

    detail::find_if_fus_impl<decltype(begin), decltype(begin), decltype(end)>
        const fi { 0, begin, begin, end };
    return fi(std::forward<Pred>(pred), std::forward<Visitor>(visitor));
}

////////////////////////////////////////////////////////////////////////////////
//
// auto find_fus(Sequence &&seq, Visitor &&visitor = detail::empty_visitor {}) ->
//   detail::exec_ctor_visitor()
//   
// seq - Fusion sequence
// t - element
// visitor - callable object visitor(elem)
// 
// Stops the search if at_c<Pos>(seq) == t.
//
// Returns: std::pair<decltype(visitor(elem)), int> where int is poisition or
// int position if the decltype(visitor(elem)) == void
//
////////////////////////////////////////////////////////////////////////////////
template <typename Sequence, typename T, typename Visitor = detail::empty_visitor>
    requires boost::c::fus_sequence<std::remove_reference_t<Sequence>>

auto find_fus(Sequence &&seq, T &&t, Visitor &&visitor = detail::empty_visitor {})
{
    return find_if_fus
    (
        std::forward<Sequence>(seq), 
        detail::find_pred<T>{ std::forward<T>(t) }, 
        std::forward<Visitor>(visitor)
    );
}

////////////////////////////////////////////////////////////////////////////////
//
// auto iter_find_if_fus(Sequence &&seq, Pred &&pred, Visitor &&visitor = detail::empty_visitor {}) ->
//   detail::exec_ctor_visitor()
//   
// seq - Fusion sequence
// pred - callable object pred(iter)
// visitor - callable object visitor(iter)
// 
// Stops the search if pred(at_c<Pos>(seq)) == true.
// 
// Returns: std::pair<decltype(visitor(iter)), int> where int is poisition or
// int position if the decltype(visitor(iter)) == void
//
////////////////////////////////////////////////////////////////////////////////
template <typename Sequence, typename Pred, typename Visitor = detail::empty_visitor>
    requires boost::c::fus_sequence<std::remove_reference_t<Sequence>>

auto iter_find_if_fus(Sequence &&seq, Pred &&pred, Visitor &&visitor = detail::empty_visitor {})
{
    namespace fus = boost::fusion;

    auto begin = fus::begin(seq);
    auto end = fus::end(seq);

    detail::iter_find_if_fus_impl<decltype(begin), decltype(begin), decltype(end)>
        fi { 0, begin, begin, end };
    return fi(std::forward<Pred>(pred), std::forward<Visitor>(visitor));
}

///////////////////////////////////////////////////////////////////////////////
//
// united predicate & visitor
// 
// Returns: std::pair<decltype(visitor(cmn::algo::visit, iter)), int> where int is position or
// int position if the decltype(visitor(cmn::algo::visit, iter)) == void
//
////////////////////////////////////////////////////////////////////////////////
template <typename Sequence, typename Pred>
    requires boost::c::fus_sequence<std::remove_reference_t<Sequence>>

auto iter_find_if_fus(Sequence &&seq, Pred &&pred, algo::visit_tag)
{
    namespace fus = boost::fusion;

    auto begin = fus::begin(seq);
    auto end = fus::end(seq);

    detail::iter_find_if_fus_impl<decltype(begin), decltype(begin), decltype(end)>
        fi { 0, begin, begin, end };
    return fi(std::forward<Pred>(pred));
}

////////////////////////////////////////////////////////////////////////////////
//
// auto find_if_mp11(Pred pred, Visitor visitor = detail::empty_visitor {}) ->
//   detail::exec_ctor_visitor()
//   
// L - MP11 list
// pred - callable object pred(elem)
// visitor - callable object visitor(elem)
// 
// Stops the search if pred(elem) == true
// 
// Returns: std::pair<decltype(visitor(elem)), int> where int is position or
// int position if the decltype(visitor(elem)) == void
// position == -1 - item is not found
//
////////////////////////////////////////////////////////////////////////////////
template
<
      boost::c::mp11_list L
    , typename Pred
    , typename Visitor = detail::empty_visitor
>
auto find_if_mp11(Pred &&pred, Visitor &&visitor = detail::empty_visitor {})
{
    detail::find_if_mp11_impl<L> fi{ 0 };
    using list_id = std::type_identity<L>;

    return fi(list_id{}, std::forward<Pred>(pred), std::forward<Visitor>(visitor));
}

////////////////////////////////////////////////////////////////////////////////
//
// auto find_noctor_if_mp11(Pred pred, Visitor visitor = detail::empty_visitor {}) ->
//   detail::exec_noctor_visitor()
// w/o ctor version
// 
// Sequence - MPL sequence
// pred - functor struct Pred { template <typename Elem> auto operator ()(Elem) {} };
// visitor - callable object visitor(elem)
// 
// Stops the search if pred.operator<decltype(elem)>() == true
// 
// Returns: std::pair<decltype(visitor(elem)), int> where int is position or
// int position if the decltype(visitor(elem)) == void
// position == -1 - item is not found
//
////////////////////////////////////////////////////////////////////////////////
template
<
      boost::c::mp11_list L
    , typename Pred
    , typename Visitor = detail::empty_type_visitor
>
auto find_if_noctor_mp11(Pred &&pred, Visitor &&visitor = detail::empty_type_visitor {})
{
    detail::find_if_noctor_mp11_impl<L> const fi { 0 };
    using list_id = std::type_identity<L>;

    return fi(list_id{}, std::forward<Pred>(pred), std::forward<Visitor>(visitor));
}

} 
