#pragma once

#include <vector>
#include <string>

#include <boost/variant.hpp>
#include <boost/optional.hpp>
#include <boost/fusion/adapted/struct/define_struct_inline.hpp>

// https://stackoverflow.com/a/23553773/8452129
#define CMN_PP_TYPE(...) ::cmn::remove_rvalue_reference_t<decltype( ::std::declval<__VA_ARGS__>())>

namespace cmn::name_
{

enum class keyword_t: char
{
    enum_,
    class_,
    struct_
};

struct enum_t;	// enum or scoped enum
struct class_t;	// class or struct

//-----------------------------------------------------------------------------
using type_t = boost::variant
<
	  enum_t
    , class_t
>;


//-----------------------------------------------------------------------------
//
// namespace
//

using namespace_fragment_t = std::string;
using namespace_t = std::vector<namespace_fragment_t>;

//-----------------------------------------------------------------------------
//
// enum or scoped enum
//
BOOST_FUSION_DEFINE_STRUCT_INLINE
(
    enum_t,
    (boost::optional<namespace_t>,      ns)
    (std::string,                       name)
);

//-----------------------------------------------------------------------------
// class or struct
BOOST_FUSION_DEFINE_STRUCT_INLINE
(
    class_t,
    (keyword_t,                         kwd)
    (boost::optional<namespace_t>,      ns)
    (std::string,                       name)
);

}
