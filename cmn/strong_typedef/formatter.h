#pragma once

#include <format>
#include <concepts>

#include <cmn/io/formatter_mixins.h>

#include "strong_typedef.h"

namespace std
{

template <typename U, std::integral T, T Default_, typename Char>
struct formatter<cmn::strong_typedef_impl<U, T, Default_>, Char>:
    cmn::io::mix::out_to_stream
    <
          formatter<cmn::strong_typedef_impl<U, T, Default_>, Char>
        , cmn::strong_typedef_impl<U, T, Default_>
        , Char
    >,
    cmn::io::mix::skip_parse
    <
          formatter<cmn::strong_typedef_impl<U, T, Default_>, Char>
        , cmn::strong_typedef_impl<U, T, Default_>
        , Char
    >
{
    using strong_typedef_impl_type = cmn::strong_typedef_impl<U, T, Default_>;
    using out_to_stream_mix_type = cmn::io::mix::out_to_stream<formatter, strong_typedef_impl_type, Char>;
    using ostream_type = typename out_to_stream_mix_type::ostream_type;

    constexpr auto prepare_stream(ostream_type &ostr) const -> ostream_type & { return ostr; }
};

template <std::integral T, typename Tag, T Default_, typename Char>
struct formatter<cmn::strong_typedef<T, Tag, Default_>, Char>:
    cmn::io::mix::out_to_stream
    <
          formatter<cmn::strong_typedef<T, Tag, Default_>, Char>
        , cmn::strong_typedef<T, Tag, Default_>
        , Char
    >,
    cmn::io::mix::skip_parse
    <
          formatter<cmn::strong_typedef<T, Tag, Default_>, Char>
        , cmn::strong_typedef<T, Tag, Default_>
        , Char
    >
{
    using strong_typedef_type = cmn::strong_typedef<T, Tag, Default_>;
    using out_to_stream_mix_type = cmn::io::mix::out_to_stream<formatter, strong_typedef_type, Char>;
    using ostream_type = typename out_to_stream_mix_type::ostream_type;

    constexpr auto prepare_stream(ostream_type &ostr) const -> ostream_type & { return ostr; }
};

}
