#pragma once

#include <type_traits>



#include <boost/core/ref.hpp>



#include "manip.h"

namespace cmn::io
{

namespace manip
{

////////////////////////////////////////////////////////////////////////////////
// 
// Keep policies
//
template <typename  T>
struct value_keep_type
{
   static constexpr bool const_array = std::is_array_v<std::remove_reference_t<T>> && 
      std::is_const_v<std::remove_all_extents_t<std::remove_reference_t<T>>>;

   using type = std::conditional_t
   <
       const_array,
         T,  // keep constant arrays by reference
         std::remove_reference_t<T>
   >;

   constexpr auto operator ()(T arg) const -> type { return static_cast<type>(arg); }
};

// NOTE: use carefully with boost::format() and boost::phoenix expressions.
// Argument referencies must be alive when the operator << will be called
template <typename  T>
struct ref_keep_type
{
   static constexpr bool by_val = std::is_rvalue_reference_v<T>;
   using type = std::conditional_t
   <
      by_val,
         std::remove_reference_t<T>,
         boost::reference_wrapper<std::remove_reference_t<T> const>
   >;

   constexpr auto operator ()(T arg) const -> type
   {
      if constexpr (by_val)
         return arg;
      else
         return boost::ref(std::as_const(arg));
   }
};

//
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
// Forwarders
//

// Deduces Char, CharTraits manipulator template parameters from output stream
template 
<
    template <typename Char, typename CharTraits> typename ManipT,
    template <typename Arg> typename PolicyT = value_keep_type
>
struct stream_slot_manip_forwarder final
{
    template <typename... Args>
    constexpr auto operator ()(Args &&... args) const
    {
        return detail::stream_slot_manip_caller<ManipT, typename PolicyT<Args &&>::type...> 
        {
            // NOTE: boost::reference_wrapper declares keep type ctor as explicit
            PolicyT<Args &&>{}(std::forward<Args>(args))...
        };
    }
};

template 
<
    typename Manip,
    template <typename Arg> typename PolicyT = value_keep_type
>
struct slot_manip_forwarder final
{
    template <typename... Args>
    constexpr auto operator ()(Args &&... args) const
    {
        return detail::slot_manip_caller<Manip, typename PolicyT<Args &&>::type...>
	    {
                // NOTE: boost::reference_wrapper declares keep type ctor as explicit
                PolicyT<Args &&>{}(std::forward<Args>(args))...
	    };
    }
};

// Deduce type parameter from argument
template 
<
    template <typename T> typename ManipT,
    template <typename Arg> typename PolicyT = value_keep_type
>
struct type_slot_manip_forwarder final
{
    template <typename T>
   constexpr auto operator ()(T &&arg) const
    {
        using keep_arg_type = typename PolicyT<T &&>::type;
        using manip_type = ManipT<keep_arg_type>;
		return detail::slot_manip_caller<manip_type, keep_arg_type>
        {
            PolicyT<T>{}(std::forward<T>(arg))
        };
    }

    template <typename T>
    constexpr auto def() const -> detail::slot_manip_caller<ManipT<T>>
    {
        using keep_arg_type = typename PolicyT<T &&>::type;
        using manip_type = ManipT<keep_arg_type>;
		return detail::slot_manip_caller<manip_type>{};
    }
};

}

using manip::slot_manip_forwarder;
using manip::stream_slot_manip_forwarder;
using manip::type_slot_manip_forwarder;

}
