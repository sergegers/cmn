#pragma once

#include <algorithm>
#include <format>

#include <boost/core/ref.hpp>
#include <boost/bind/bind.hpp>

// ReSharper disable CppUnusedIncludeDirective
#include <boost/fusion/adapted/std_tuple.hpp>
// ReSharper restore CppUnusedIncludeDirective
#if __has_include(<boost/fusion/type_traits.hpp>)
#include <boost/fusion/type_traits.hpp>
#else
#include <cmn/meta/boost/fusion/type_traits.hpp>
#endif

#include <cmn/meta/concepts.h>
#include <cmn/meta/traits.h>
#include <cmn/util/feature.h>
#include <cmn/util/symbols.h>

#include <cmn/enum/detail/record_info.h>

#include "print.h"

namespace cmn::enum_::io
{

namespace detail
{

template 
<
      c::adapted_enum E
    , c::list_sink_format_options FmtOpt
    , typename OutIt
>
constexpr auto out_record
(
      record_info<E> const &rec
    , mask_type_t<E>
    , FmtOpt const &fmt_opt
    , bool &first
    , OutIt out_it
) -> void
{
    using enum print_t;
    using char_type = FmtOpt::char_type;
    using char_traits_type = FmtOpt::char_traits_type;

    print_t options = fmt_opt.options;
    auto const &scope_resulution = fmt_opt.scope_resolution;
    auto const & delimiter = fmt_opt.delimiter;

    auto const &name = rec.template name<char_type, char_traits_type>();

    if (!first)
        std::ranges::copy(delimiter, out_it);
    else
        first = false;

    if (has_feature(options, ns))
    {
        std::ranges::copy(name.m_ns, out_it);
        std::ranges::copy(scope_resulution, out_it);
    }

    if (has_feature(options, class_prefix))
    {
        std::ranges::copy(name.m_enum_name, out_it);
        std::ranges::copy(scope_resulution, out_it);
    }

    std::ranges::copy(name.m_enum_member_name, out_it);
}

//-----------------------------------------------------------------------------
template
<
      c::list_sink_format_options FmtOpt
    , typename OutIt
>
constexpr auto out_tail(std::integral auto remain, FmtOpt const &fmt_opt, bool first, OutIt out_it) -> void
{
    if (has_feature(fmt_opt.options, print_t::tail) && !empty(remain))
    {
        using char_type = FmtOpt::char_type;
        using char_traits_type = FmtOpt::char_traits_type;

        if (!first)
            std::ranges::copy(fmt_opt.delimiter, out_it);

        // TODO: use C array for fixed_string container
        std::format_to(out_it, sym::def_fmt_1_arg.value<char_type, char_traits_type>().m_data._Elems, remain);
    }
}

///////////////////////////////////////////////////////////////////////////////
template 
<
      c::list_sink_format_options FmtOpt
    , c::adapted_enum E
    , typename OutIt
>
constexpr auto out_enum(E en, FmtOpt const &fmt_opt, OutIt out_it) -> void
{
    namespace fus = boost::fusion;
    namespace rfus = fus::result_of;
    using namespace boost::placeholders;

    using mask_type = mask_type_t<E>;

    static auto groups = groups_v<E>;
#if CMN_STATIC_TEST
    static_assert(rfus::size_v<decltype(groups)> == 1, "Enum must have the one and only one group");
#endif
    static decltype(auto) group = std::get<0>(groups);

    auto const mask = static_cast<mask_type>(fmt_opt.mask);
    auto const &open = fmt_opt.open;
    auto const &close = fmt_opt.close;

    std::ranges::copy(open, out_it);

    bool first = true;
    auto const remain = group_::find_if
    (
          group
        , en
        , boost::bind
          (
                &out_record<E, FmtOpt, OutIt>
              , _1
              , _2
              , boost::cref(fmt_opt)
              , boost::ref(first)
              , out_it
          )
        , {}
        , mask
    );

    out_tail(interop_cast(remain), fmt_opt, first, out_it);

    std::ranges::copy(close, out_it);
}

//-----------------------------------------------------------------------------
template
<
      c::list_sink_format_options FmtOpt
    , c::adapted_enum E
    , typename OutIt
>
constexpr auto out_bitfield(E en, FmtOpt const &fmt_opt, OutIt out_it) -> void
{
    using namespace boost::placeholders;

    using mask_type = mask_type_t<E>;

    auto const &groups = groups_v<E>;

    auto const mask = static_cast<mask_type>(fmt_opt.mask);
    auto const &open = fmt_opt.open;
    auto const &close = fmt_opt.close;

    std::ranges::copy(open, out_it);

    bool first = true;
    auto const remain = fold
    (
          groups
        , en
        , boost::bind
            (
                  &out_record<E, FmtOpt, OutIt>
                , _1
                , _2
                , boost::cref(fmt_opt)
                , boost::ref(first)
                , out_it
            )
        , {}
        , mask
    );

    out_tail(interop_cast(remain), fmt_opt, first, out_it);

    std::ranges::copy(close, out_it);
}

}
///////////////////////////////////////////////////////////////////////////////
///
/// redirect to appropriate out_...() function
///
///////////////////////////////////////////////////////////////////////////////
struct out_fn
{
    constexpr auto operator ()
    (
          int_<kind_t::enum_>
        , c::adapted_enum auto en
        , c::list_sink_format_options auto const &fmt_opt
        , auto out_it
    ) 
        const noexcept -> void
    {
        detail::out_enum(en, fmt_opt, out_it);
    }

    constexpr auto operator ()
    (
          int_<kind_t::bitfield>
        , c::adapted_enum auto en
        , c::list_sink_format_options auto const &fmt_opt
        , auto out_it
    )
        const noexcept -> void
    {
        detail::out_bitfield(en, fmt_opt, out_it);
    }

    constexpr auto operator ()
    (
          int_<kind_t::combo>
        , c::adapted_enum auto en
        , c::list_sink_format_options auto const &fmt_opt
        , auto out_it
    )
        const noexcept -> void
    {
        detail::out_bitfield(en, fmt_opt, out_it);
    }
}

inline constexpr out{};

}

