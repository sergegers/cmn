#pragma once

#include <type_traits>

// boost.variant
#include <boost/variant/static_visitor.hpp>

namespace cmn
{

namespace detail
{

struct empty_visitor: boost::static_visitor<void>
{
    empty_visitor() = default;

    constexpr auto operator ()(auto &&...) const noexcept -> void {}
};

struct empty_type_visitor: boost::static_visitor<void>
{
    empty_type_visitor() = default;

    template <typename... Args>
    constexpr auto operator ()() const noexcept -> void {}
};


// lazy get visitor result type by old result_of protocol
template <typename Visitor>
struct vis_result
{
    using type = typename Visitor::result_type;
};

template <typename VisitorRef>
using vis_result_t  = typename vis_result<std::remove_cvref_t<VisitorRef>>::type;

}

//-----------------------------------------------------------------------------
namespace algo
{

struct visit_tag {};
constexpr visit_tag visit{};

}

}

