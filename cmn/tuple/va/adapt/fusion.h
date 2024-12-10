#pragma once

#include <cmn/meta/concepts.h>
#include <cmn/tuple/va/concepts.h>
#include <cmn/tuple/va/tuple_view.h>

////////////////////////////////////////////////////////////////////////////////
//
// adapt to FUSION and MPL algorithms
// NOTE: dispatch operations except at<> through std_tuple_tag
// at<> operation returns by va_tuple element by value because there is a 
// conversion float -> double is performed during float element access. So
// the va_tuple/va_tuple_view are 'read-only' ones.
//
////////////////////////////////////////////////////////////////////////////////
namespace boost
{

namespace fusion
{

using va_tuple_tag = cmn::va::tuple_tag;
struct fusion_sequence_tag;

namespace traits
{

template <cmn::c::function Sig> struct tag_of<cmn::va::tuple_view<Sig>> { using type = va_tuple_tag; };
template <cmn::c::function Sig> struct tag_of<cmn::va::tuple<Sig>> { using type = va_tuple_tag; };

}   

namespace extension
{
	
template <> struct is_view_impl<va_tuple_tag> : is_view_impl<std_tuple_tag> {};
template <> struct is_sequence_impl<va_tuple_tag> : is_sequence_impl<std_tuple_tag> {};
template <> struct category_of_impl<va_tuple_tag>: category_of_impl<std_tuple_tag> {};
template <> struct begin_impl<va_tuple_tag> : begin_impl<std_tuple_tag> {};
template <> struct end_impl<va_tuple_tag> : end_impl<std_tuple_tag> {};
template <> struct size_impl<va_tuple_tag> : size_impl<std_tuple_tag> {};

// return by value
template <>
struct at_impl<va_tuple_tag>
{
    template <typename Sequence, typename N>
    struct apply
    {
        using seq_type = std::remove_const_t<Sequence>;
        using element = std::tuple_element_t<N::value, seq_type>;
        using type = element;

        static auto call(Sequence &seq) noexcept -> type
        {
            return std::get<N::value>(seq);
        }
    };
};

template <> struct value_at_impl<va_tuple_tag> : value_at_impl<std_tuple_tag> {};
template <> struct convert_impl<va_tuple_tag> : convert_impl<std_tuple_tag> {};

}

namespace detail
{

template <>
struct clear<va_tuple_tag> : std::type_identity<cmn::va::tuple_view<auto () -> void>> {};

}

}

}
