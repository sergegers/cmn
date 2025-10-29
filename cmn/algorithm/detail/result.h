#pragma once

#include <concepts>
#include <utility>
#include <cstddef>

// ReSharper disable CppUnusedIncludeDirective

#include <boost/mp11.hpp>
// boost.fusion
#include <boost/fusion/sequence/intrinsic/begin.hpp>
#include <boost/fusion/sequence/intrinsic/end.hpp>
#include <boost/fusion/sequence/intrinsic/at_c.hpp>
#include <boost/fusion/sequence/intrinsic/empty.hpp>
#include <boost/fusion/iterator/advance.hpp>
#include <boost/fusion/iterator/distance.hpp>
#include <boost/fusion/iterator/deref.hpp>
#include <boost/fusion/iterator/next.hpp>
#include <boost/fusion/iterator/equal_to.hpp>

// ReSharper restore CppUnusedIncludeDirective

#include <boost/none.hpp>

#include <cmn/meta/concepts.h>
#include <cmn/algorithm/detail/visitor.h>

namespace cmn::detail
{

using namespace boost::mp11;

//-----------------------------------------------------------------------------

// true if the callable object returns void
template <typename Fn, typename... Args>
constexpr bool is_proc_v = std::is_void_v<decltype(std::declval<Fn>()(std::forward<Args>(std::declval<Args>())...))>;

// true if the functor returns void
template <typename Fn, typename... Args>
constexpr bool is_type_proc_v = std::is_void_v<decltype(std::declval<Fn>().template operator()<Args...>())>;

// the algorithm returns std::pair<visitor(), position> or position if
// the visitor result is void

//-----------------------------------------------------------------------------
template <typename Result, typename... Args>
concept callable_ = requires (Result &&r, Args &&... args)
{
    std::forward<Result>(r).m_visitor(std::forward<Args>(args)...);
};

template <typename Result, typename... Args>
concept procedure_ =
    callable_<Result, Args...>
 && requires (Result &&r, Args &&... args)
    {
        { std::forward<Result>(r).m_visitor(std::forward<Args>(args)...) } -> std::same_as<void>;
    }
;

template <typename Result, typename... Args>
concept type_callable_ = requires (Result &&r)
{
    std::forward<Result>(r).m_visitor.template operator ()<Args...>();
};

template <typename Result, typename... Args>
concept type_procedure_ =
    type_callable_<Result, Args...>
 && requires (Result &&r)
    {
        { std::forward<Result>(r).m_visitor.template operator ()<Args...>() } -> std::same_as<void>;
    }
;

///////////////////////////////////////////////////////////////////////////////
template <typename Visitor>
struct result_base_
{
    // NOTE: don't use protected to use aggregate initialization
    
    Visitor m_visitor;

    //-----------------------------------------------------------------------------
    template <typename Self, c::enumerable Index = std::ptrdiff_t>
    [[nodiscard]] constexpr auto with_index(this Self &&self_, Index index = -1) requires callable_<Self>
    {
        return self_.make_result
        (
            [&self_, index]{ return std::pair { std::forward<Self>(self_).m_visitor(), index }; }
        );
    }

    template <typename Self, c::enumerable Index = std::ptrdiff_t>
    [[nodiscard]] constexpr auto with_index(this Self &&self_, Index index = -1) requires procedure_<Self>
    {
        return self_.make_result
        (
            [&self_, index]
            {
                std::forward<Self>(self_).m_visitor();
                return index;
            }
        );
    }

    //-----------------------------------------------------------------------------
    template <typename Self>
    constexpr [[nodiscard]] auto vis_result_not_found(this Self &&self_)
    {
        return self_.make_result([]() -> vis_result_t<Visitor> { return {}; });
    }

    template <typename Self>
    constexpr [[nodiscard]] auto vis_result_not_found(this Self &&self_) requires std::is_void_v<vis_result_t<Visitor>>
    {
        return self_.make_result([]{});
    }
};

//-----------------------------------------------------------------------------
template <typename Visitor>
struct result_ final: result_base_<Visitor>
{
private:
    using inherited = result_base_<Visitor>;
    friend inherited;

    template <typename NewVisitor>
    [[nodiscard]] static constexpr auto make_result(NewVisitor &&vis) -> result_<NewVisitor>
    {
        return { std::forward<NewVisitor>(vis) };
    }
public:
    template <typename VisArg>
    constexpr result_(VisArg &&visitor):
        inherited{ std::forward<VisArg>(visitor) }
    {}

    //-----------------------------------------------------------------------------
    //
    // compatible boost::apply_visitor()
    // NOTE: (for debugging purposes) set breakpoint here
    //
    template <typename Self, typename... Args>
    [[nodiscard]] constexpr auto operator ()(this Self &&self_, Args &&... args) ->
        decltype(std::forward<Self>(self_).m_visitor(std::forward<Args>(args)...))
    {
        return std::forward<Self>(self_).m_visitor(std::forward<Args>(args)...);
    }

    //-----------------------------------------------------------------------------
    template <typename Self, typename... Args>
    [[nodiscard]] constexpr auto with_args(this Self &&self_, Args &&... args) requires callable_<Self, Args...>
    {
        return make_result
        (
            [&self_, &args...] //() mutable // to properly capture args
            { return std::forward<Self>(self_).m_visitor(std::forward<Args>(args)...); }
        );
    }

    //-----------------------------------------------------------------------------
    template <typename... Args, typename Self>
    [[nodiscard]] constexpr auto with_type_args(this Self &&self_) requires type_callable_<Self, Args...>
    {
        return make_result
        (
            [&self_]
            { return std::forward<Self>(self_).m_visitor.template operator ()<Args...>(); }
        );
    }

    //-----------------------------------------------------------------------------
    template <typename Self, typename... Args>
    [[nodiscard]] constexpr auto args_not_found(this Self &&self_, Args &&... args) requires callable_<Self, Args...>
    {
        using visitor_result_type = decltype(std::forward<Self>(self_).m_visitor(std::forward<Args>(args)...));
        return make_result
        (
            []() -> visitor_result_type { return {}; }
        );
    }

    // void result
    template <typename Self, typename... Args>
    [[nodiscard]] constexpr auto args_not_found(this Self &&self_, Args &&... args) requires procedure_<Self, Args...>
    {
        return make_result([]{});
    }

    //-----------------------------------------------------------------------------
    template <typename... Args, typename Self>
    [[nodiscard]] constexpr auto type_args_not_found(this Self &&self_) requires callable_<Self, Args...>
    {
        using visitor_result_type = decltype(std::forward<Self>(self_).m_visitor(std::declval<Args>()...));
        return make_result
        (
            []() -> visitor_result_type { return {}; }
        );
    }

    template <typename... Args, typename Self>
    [[nodiscard]] constexpr auto type_args_not_found(this Self &&self_) requires procedure_<Self, Args...>
    {
        return make_result([]{});
    }
};

template <typename Visitor> result_(Visitor &&) -> result_<Visitor>;

//-----------------------------------------------------------------------------
template <typename Visitor>
struct type_result_ final: result_base_<Visitor>
{
private:
    using inherited = result_base_<Visitor>;
    friend inherited;

    template <typename NewVisitor>
    [[nodiscard]] static constexpr auto make_result(NewVisitor &&vis) -> result_<NewVisitor>
    {
        return { std::forward<NewVisitor>(vis) };
    }
public:
    template <typename VisArg>
    constexpr type_result_(VisArg &&visitor):
        inherited{ std::forward<VisArg>(visitor) }
    {}

    //-----------------------------------------------------------------------------
    //
    // compatible boost::apply_visitor()
    // NOTE: (for debugging purposes) set breakpoint here
    //
    template <typename... Args, typename Self>
    [[nodiscard]] constexpr auto operator ()(this Self &&self_) ->
        decltype(std::forward<Self>(self_).m_visitor.template operator ()<Args...>())
    {
        return std::forward<Self>(self_).m_visitor.template operator ()<Args...>();
    }

    //-----------------------------------------------------------------------------
    template <typename... Args, typename Self>
    [[nodiscard]] constexpr auto with_args(this Self &&self_) requires type_callable_<Self, Args...>
    {
        return make_result
        (
            [&self_]
            { return std::forward<Self>(self_).m_visitor.template operator ()<Args...>(); }
        );
    }

    //-----------------------------------------------------------------------------
    template <typename... Args, typename Self>
    [[nodiscard]] constexpr auto args_not_found(this Self &&self_) requires type_callable_<Self, Args...>
    {
        using visitor_result_type = decltype(std::forward<Self>(self_).m_visitor.template operator ()<Args...>());
        return make_result
        (
            []() -> visitor_result_type { return {}; }
        );
    }

    template <typename... Args, typename Self>
    [[nodiscard]] constexpr auto args_not_found(this Self &&self_) requires type_procedure_<Self, Args...>
    {
        return make_result([]{});
    }

    //-----------------------------------------------------------------------------
    template <boost::c::mp11_list L, typename Self, std::size_t Idx_>
    [[nodiscard]] constexpr auto with_index_arg(this Self &&self_, std::integral_constant<std::size_t, Idx_>)
        requires type_callable_<Self, mp_at_c<L, Idx_>>
    {
        return make_result
        (
            [&self_] 
            { return std::forward<Self>(self_).m_visitor.template operator ()<mp_at_c<L, Idx_>>(); }
        );
    }
};

template <typename Visitor> type_result_(Visitor &&) -> type_result_<Visitor>;

}
