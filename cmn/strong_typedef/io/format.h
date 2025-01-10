#pragma once

#include <format>
#include <concepts>

#include <cmn/io/format.h>

#include <cmn/strong_typedef/strong_typedef.h>

namespace cmn::io
{

template <typename StrongTypedef, typename Char>
struct strong_typedef_formatter:
    mix::out_to_stream
    <
          strong_typedef_formatter<StrongTypedef, Char>
        , StrongTypedef
        , Char
    >,
    mix::skip_parse
    <
          strong_typedef_formatter<StrongTypedef, Char>
        , StrongTypedef
        , Char
    >
{
    using out_to_stream_mix_type = mix::out_to_stream<strong_typedef_formatter, StrongTypedef, Char>;
    using ostream_type = typename out_to_stream_mix_type::ostream_type;

    /* CRTP override */ constexpr auto prepare_stream(ostream_type &ostr) const -> ostream_type & { return ostr; }
};

}

namespace std
{

template <typename U, std::integral T, T Default_, typename Char>
struct formatter<cmn::strong_typedef_impl<U, T, Default_>, Char>: cmn::io::strong_typedef_formatter<U, Char> {};

template <std::integral T, typename Tag, T Default_, typename Char>
struct formatter<cmn::strong_typedef<T, Tag, Default_>, Char>:
    cmn::io::strong_typedef_formatter<cmn::strong_typedef<T, Tag, Default_>, Char> {};

}
