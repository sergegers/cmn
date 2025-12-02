#pragma once

#include <compare>

#include <boost/mp11.hpp>

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

#include <cmn/util/util.h>
#include <cmn/algorithm/detail/visitor.h>
#include <cmn/algorithm/detail/result.h>

namespace cmn
{

namespace detail
{

using namespace boost::mp11;
namespace fus = boost::fusion;
namespace rfus = fus::result_of;

////////////////////////////////////////////////////////////////////////////////
//
// mp11 version
//
////////////////////////////////////////////////////////////////////////////////

template <boost::c::mp11_list L, typename Comparer>
struct binary_find_if_mp11_impl
{
    using itself = binary_find_if_mp11_impl;

    binary_find_if_mp11_impl()               = default;
    binary_find_if_mp11_impl(itself const &) = delete;
    binary_find_if_mp11_impl(itself &&)      = delete;

    auto operator =(itself const &) -> itself& = delete;
    auto operator =(itself &&) -> itself&      = delete;

    // keep begin, end for debug
    template
    <
          std::ptrdiff_t BeginIdx_
        , std::ptrdiff_t EndIdx_
        , typename T
        , typename Visitor
        , typename... Args
    >
    constexpr auto operator ()
    (
          [[maybe_unused]] mp_size_t<BeginIdx_> begin_idx
        , [[maybe_unused]] mp_size_t<EndIdx_> end_idx
        , T &&t
        , Visitor &&visitor
        , Args &&... args
    ) const
    {
        if constexpr (BeginIdx_ == EndIdx_)
        {
            if constexpr (BeginIdx_ == mp_size_v<L>)
            {
                // use the first sequence element to compute a visitor result type
                return
                    result_{ std::forward<Visitor>(visitor) }
                        .template type_args_not_found<mp_front<L>, Args...>()
                        .with_index()
                    ()
                ;
            }
            else
            {
                using cur_type = mp_at_c<L, BeginIdx_>;

                if (cur_type ct; std::is_eq(Comparer{}(t, ct)))
                {
                    return
                        result_{ std::forward<Visitor>(visitor) }
                            .with_args(std::forward<cur_type>(ct), std::forward<Args>(args)...)
                            .with_index(BeginIdx_)
                        ()
                    ;
                }
                else
                {
                    return
                        result_{ std::forward<Visitor>(visitor) }
                            .args_not_found(std::forward<cur_type>(ct), std::forward<Args>(args)...)
                            .with_index()
                        ()
                    ;
                }
            }
        }
        else
        {
            // get the middle index
            static constexpr auto middle_idx = BeginIdx_ + ((EndIdx_ - BeginIdx_) >> 1ul);
            using mid_type = mp_at_c<L, middle_idx>;

            if (mid_type middle{}; std::is_lt(Comparer{}(t, middle)))
            {
                itself const next{};
                return next
                (
                      mp_size_t<BeginIdx_>{}
                    , mp_size_t<middle_idx>{}
                    , std::forward<T>(t)
                    , std::forward<Visitor>(visitor)
                    , std::forward<Args>(args)...
                );
            }
            else if (std::is_eq(Comparer{}(t, middle)))
            {
                return
                    result_{ std::forward<Visitor>(visitor) }
                        .with_args(std::forward<mid_type>(middle), std::forward<Args>(args)...)
                        .with_index(middle_idx)
                    ()
                ;
            }
            else /*greater*/
            {
                static constexpr auto new_begin = middle_idx + 1;
                binary_find_if_mp11_impl<L, Comparer> const next{};
                return next
                (
                      mp_size_t<new_begin>{}
                    , mp_size_t<EndIdx_>{}
                    , std::forward<T>(t)
                    , std::forward<Visitor>(visitor)
                    , std::forward<Args>(args)...
                );
            }
        }
    }

    // keep begin, end for debug
    template
    <
          std::ptrdiff_t BeginIdx_
        , std::ptrdiff_t EndIdx_
        , typename T
        , typename Visitor
        , typename... Args
    >
        requires mp_empty_v<L>
    constexpr auto operator ()
    (
          [[maybe_unused]] mp_size_t<BeginIdx_> begin_idx
        , [[maybe_unused]] mp_size_t<EndIdx_> end_idx
        , T &&t
        , Visitor &&visitor
        , Args &&... args
    ) const
    {
        // empty sequence
        return
            result_{ [](auto &&...){} }
                .args_not_found(std::forward<Args>(args)...)
                .with_index()
            ()
        ;
    }
};

////////////////////////////////////////////////////////////////////////////////
//
// fusion version
// NOTE: fusion operators could have the different types but be equal.
// Compare them by the operator == and "metacompare" them by the equal_to 
// metafunction.
//
////////////////////////////////////////////////////////////////////////////////

// search on [BeginIter, EndIter) range
// if the element is not found search on next range, see comments for details
template 
<
    typename Sequence, 
    typename BeginIter, 
    typename EndIter
>
struct binary_find_fus_impl
{
    using itself = binary_find_fus_impl;

    Sequence       &m_seq;

    explicit binary_find_fus_impl(Sequence &seq) noexcept: m_seq { seq } {}
    binary_find_fus_impl(itself const &) = delete;
    binary_find_fus_impl(itself &&) = delete;

    auto operator =(itself const &) -> itself& = delete;
    auto operator =(itself &&) -> itself&      = delete;

    template <typename T, typename Comparer, typename Visitor, typename... Args>
    constexpr auto operator ()(T &&t, Comparer &&cmp, Visitor &&visitor, Args &&... args) const
    {
        using seq_begin_iter_type = rfus::begin_t<Sequence>;
        using seq_end_iter_type = rfus::end_t<Sequence>;

        if constexpr (rfus::equal_to_v<BeginIter, EndIter>)
        {
            // empty sequence chunk

            if constexpr (rfus::equal_to_v<BeginIter, seq_end_iter_type>)
            {
                // end of search

                // use first sequence element to compute a visitor result type
                using front_type = rfus::deref_t<seq_begin_iter_type>;
                return
                    result_{ std::forward<Visitor>(visitor) }
                        .args_not_found(std::forward<front_type>(fus::front(m_seq)), std::forward<Args>(args)...)
                        .with_index()
                    ()
                ;
            }
            else
            {
                // check the first picked sequence chunk element
                static constexpr std::ptrdiff_t s_new_pos = rfus::distance_v<seq_begin_iter_type, BeginIter>;

                if (decltype(auto) ct = fus::at_c<s_new_pos>(m_seq); std::is_eq(cmp(t, ct)))
                {
                    return result_{ std::forward<Visitor>(visitor) }
                            .with_args(std::forward<decltype(ct)>(ct), std::forward<Args>(args)...)
                            .with_index(s_new_pos)
                        ()
                    ;
                }
                else
                {
                    return result_{ std::forward<Visitor>(visitor) }
                            .args_not_found(std::forward<decltype(ct)>(ct), std::forward<Args>(args)...)
                            .with_index()
                        ()
                    ;
                }
            }
        }
        else
        {
            // get the middle iterator
            static constexpr std::ptrdiff_t s_mid_delta = rfus::distance_v<BeginIter, EndIter> >> 1;
            using mid_iter_type = rfus::advance_c_t<BeginIter, s_mid_delta>;

            constexpr std::ptrdiff_t s_new_pos = rfus::distance_v<seq_begin_iter_type, mid_iter_type>;

            if (decltype(auto) mt = fus::at_c<s_new_pos>(m_seq);std::is_lt(cmp(t, mt)))
            {
                binary_find_fus_impl<Sequence, BeginIter, mid_iter_type> const next_{ m_seq };
                return next_
                (
                      std::forward<T>(t)
                    , std::forward<Comparer>(cmp)
                    , std::forward<Visitor>(visitor)
                    , std::forward<Args>(args)...
                );
            }
            else if (std::is_eq(cmp(t, mt)))
            {
                return result_{ std::forward<Visitor>(visitor) }
                        .with_args(std::forward<decltype(mt)>(mt), std::forward<Args>(args)...)
                        .with_index(s_new_pos)
                    ()
                ;
            }
            else /*greater*/
            {
                using next_iter_type = rfus::next_t<mid_iter_type>;
                binary_find_fus_impl<Sequence, next_iter_type, EndIter> const next_{ m_seq };
                return next_
                (
                      std::forward<T>(t)
                    , std::forward<Comparer>(cmp)
                    , std::forward<Visitor>(visitor)
                    , std::forward<Args>(args)...
                );
            }
        }
    }
};

template 
<
    typename Sequence, 
    typename BeginIter, 
    typename EndIter
>
    requires rfus::empty_v<Sequence>

struct binary_find_fus_impl<Sequence, BeginIter, EndIter>
{
    using itself = binary_find_fus_impl;

    explicit binary_find_fus_impl(Sequence &/*seq*/) {}
    binary_find_fus_impl(itself const &) = delete;
    binary_find_fus_impl(itself &&) = delete;

    auto operator = (itself const &) -> itself & = delete;
    auto operator = (itself &&) -> itself &      = delete;

    template <typename T, typename Comparer, typename Visitor, typename... Args>
    constexpr auto operator ()(T &&/*t*/, Comparer &&/*cmp*/, Visitor &&visitor, Args &&... args) const
    {
        // empty sequence
        return result_{ []{} }.with_index()();
    }
};

}	

// The first operand is always binary_find_mpl parameter
struct integral_comparer
{
    constexpr auto operator ()(auto const &lh, auto const &rh) const
    { 
        return lh.value <=> rh.value; 
    }
};

struct digit_comparer
{
    constexpr auto operator ()(auto const &lh, auto const &rh) const
    {
        return lh <=> rh;
    }
};

////////////////////////////////////////////////////////////////////////////////
//
// auto binary_find_mp11(T t, Visitor visitor = detail::empty_visitor {}) ->
//    std::pair<visitor(), position> | position
//
////////////////////////////////////////////////////////////////////////////////
template
<
      boost::c::mp11_list L
    , typename T
    , typename Visitor = detail::empty_visitor
    , typename... Args
>
    requires std::invocable<Visitor, T &&, Args &&...>

constexpr auto binary_find_mp11
(
      T &&t
    , Visitor &&visitor = detail::empty_visitor {}
    , Args &&... args
)
{
    using namespace boost::mp11;
    
    detail::binary_find_if_mp11_impl<L, integral_comparer> const bs {};
    return bs
    (
          mp_size_t<0>{}
        , mp_size_t<mp_size_v<L>>{}
        , std::forward<T>(t)
        , std::forward<Visitor>(visitor)
        , std::forward<Args>(args)...
    );
}


////////////////////////////////////////////////////////////////////////////////
//
// auto binary_find_if_mp11(T t, Visitor visitor = detail::empty_visitor {}) ->
//    std::pair<visitor(), position> | position
//
////////////////////////////////////////////////////////////////////////////////
template
<
      boost::c::mp11_list L
    , typename Comparer
    , typename T
    , typename Visitor = detail::empty_visitor
    , typename... Args
>
    requires std::invocable<Visitor, T &&, Args &&...>

constexpr auto binary_find_if_mp11
(
      T &&t
    , Visitor &&visitor = detail::empty_visitor {}
    , Args &&... args
)
{
    using namespace boost::mp11;
    
    detail::binary_find_if_mp11_impl<L, Comparer> const bs {};
    return bs
    (
          mp_size_t<0>{}
        , mp_size_t<mp_size_v<L>>{}
        , std::forward<T>(t)
        , std::forward<Visitor>(visitor)
        , std::forward<Args>(args)...
    );
}

////////////////////////////////////////////////////////////////////////////////
//
// auto binary_find_fus(T t, Visitor visitor = detail::empty_visitor {}) -> 
//    std::pair<visitor(), position> | position
//
////////////////////////////////////////////////////////////////////////////////
template
<
      boost::c::random_access_fus_sequence Sequence
    , typename T
    , typename Visitor = detail::empty_visitor
    , typename... Args
>
    requires std::invocable<Visitor, T &&, Args &&...>    

constexpr auto binary_find_fus
(
      Sequence &seq
    , T &&t
    , Visitor &&visitor = detail::empty_visitor {}
    , Args &&... args
)
{
    namespace rfus = boost::fusion::result_of;
    using begin_iter = rfus::begin_t<Sequence>;
    using end_iter = rfus::end_t<Sequence>;

    detail::binary_find_fus_impl
    <
        Sequence,
        begin_iter,
        end_iter
    > const bs { seq };
    return bs
    (
          std::forward<T>(t)
        , digit_comparer{}
        , std::forward<Visitor>(visitor)
        , std::forward<Args>(args)...
    );
}

////////////////////////////////////////////////////////////////////////////////
//
// auto binary_find_if_fus(T t, Visitor visitor = detail::empty_visitor {}) ->
//   std::pair<visitor(), position> | position
//
// Comparer is less equal comparer
//
////////////////////////////////////////////////////////////////////////////////
template
<
      boost::c::random_access_fus_sequence Sequence
    , typename T
    , typename Comparer = digit_comparer
    , typename Visitor = detail::empty_visitor
    , typename... Args
>
constexpr auto binary_find_if_fus
(
      Sequence &seq
    , T &&t
    , Comparer &&cmp = digit_comparer{}
    , Visitor &&visitor = detail::empty_visitor {}
    , Args &&... args
)
{
    namespace rfus = boost::fusion::result_of;
    using begin_iter = rfus::begin_t<Sequence>;
    using end_iter = rfus::end_t<Sequence>;

    detail::binary_find_fus_impl
    <
        Sequence,
        begin_iter,
        end_iter
    > const bs { seq };
    return bs
    (
          std::forward<T>(t)
        , std::forward<Comparer>(cmp)
        , std::forward<Visitor>(visitor)
        , std::forward<Args>(args)...
    );
}

}
