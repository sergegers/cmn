
#include <concepts>
#include <format>
#include <ios>

#include <boost/test/unit_test.hpp>

#include <cmn/meta/concepts.h>
#include <cmn/strong_typedef/strong_typedef.h>
#include <cmn/util/symbols.h>

#include <cmn/io/format/mix/parse_arg.h>
#include <cmn/io/format/mix/mixins.h>
#include <cmn/io/format/util.h>

namespace cmn::io
{

using my_int = strong_typedef<int, struct my_int_>;

}

namespace std
{
    
template <typename Char>
struct formatter<cmn::io::my_int, Char>:
    cmn::io::mix::parse_arg<Char>,
    cmn::io::mix::out_to_stream<cmn::io::my_int, Char>
{
    using parse_arg_type = cmn::io::mix::parse_arg<Char>;
    using ostream_type = std::basic_ostream<Char>;

    bool m_hex = false;

    constexpr auto prepare_stream(this auto const &self_, ostream_type &ostr) -> ostream_type &
    {
        if (self_.m_hex)
            ostr << std::hex << std::showbase << std::uppercase;
        else
            ostr << std::dec << std::noshowbase;

        return ostr;
    }

    template<typename ParseContext>
    constexpr auto parse(this auto &self_, ParseContext &ctx) -> cmn::io::context_iterator_t<ParseContext>
    {
        for
        (
            auto range_ = parse_arg_type::try_parse_arg(ctx); 
            range_ != std::nullopt; 
            range_ = parse_arg_type::try_parse_arg(ctx))
        {
            using string_view_type = std::basic_string_view<Char>;
            using symbols_type = cmn::symbols<Char>;

            string_view_type const chunk { *range_ };
            if (0 == symbols_type::x.compare_as_cstr(chunk))         // x
                self_.m_hex = true;
            else
                throw std::format_error(std::format("Unknown format specifier {}", chunk));
        }

        return ctx.begin();
    }    
};

}

BOOST_AUTO_TEST_SUITE(cmn)
BOOST_AUTO_TEST_SUITE(io)

BOOST_AUTO_TEST_CASE(find_symbol_)
{
    using namespace std::string_view_literals;

    std::format_parse_context ctx{ "{:a:b:c}", 3 };

    auto end = find_symbol(ctx, ctx.begin(), scroll_to_sep_<>);
    auto begin = ctx.begin();

    std::advance(begin, 1);
    BOOST_CHECK(begin == end);

    std::advance(begin, 1);
    end = find_symbol(ctx, begin, scroll_to_sep_<>);
    std::string_view a{ begin, end };
    BOOST_TEST(a == "a"sv);

    std::advance(begin, 1);
    BOOST_CHECK(begin == end);

    std::advance(begin, 1);
    end = find_symbol(ctx, begin, scroll_to_sep_<>);
    std::string_view b{ begin, end };
    BOOST_TEST(b == "b"sv);

    std::advance(begin, 1);
    BOOST_CHECK(begin == end);

    std::advance(begin, 1);
    end = find_symbol(ctx, begin, scroll_to_close_<>);
    std::string_view c{ begin, end };
    BOOST_TEST(c == "c"sv);
}

BOOST_AUTO_TEST_CASE(parse_arg)
{
    using namespace std::string_view_literals;

    std::format_parse_context ctx{ "{:a:b:c}", 3 };
    using formatter_type = std::formatter<my_int>;

    auto it = ctx.begin();
    std::advance(it, 2);
    ctx.advance_to(it);

    std::string_view a { formatter_type::parse_arg_(ctx) };
    BOOST_TEST(a == "a"sv);

    it = ctx.begin();
    std::advance(it, 1);
    ctx.advance_to(it);

    std::string_view b{ formatter_type::parse_arg_(ctx) };
    BOOST_TEST(b == "b"sv);

    it = ctx.begin();
    std::advance(it, 1);
    ctx.advance_to(it);

    std::string_view c{ formatter_type::parse_last_arg(ctx) };
    BOOST_TEST(c == "c"sv);

    it = ctx.begin();
    std::advance(it, 1);
    BOOST_CHECK(it == ctx.end());
}

BOOST_AUTO_TEST_CASE(try_parse_arg)
{
    using namespace std::string_view_literals;

    std::format_parse_context ctx{ "{:a:b:c}", 3 };
    using formatter_type = std::formatter<my_int>;

    auto it = ctx.begin();
    std::advance(it, 2);
    ctx.advance_to(it);

    auto const a_opt = formatter_type::try_parse_arg(ctx);
    BOOST_REQUIRE(a_opt);
    std::string_view a { *a_opt };
    BOOST_TEST(a == "a"sv);

    it = ctx.begin();
    std::advance(it, 1);
    ctx.advance_to(it);

    auto const b_opt = formatter_type::try_parse_arg(ctx);
    BOOST_REQUIRE(b_opt);
    std::string_view b{ *b_opt };
    BOOST_TEST(b == "b"sv);

    it = ctx.begin();
    std::advance(it, 1);
    ctx.advance_to(it);

    auto const c_opt = formatter_type::try_parse_arg(ctx);
    BOOST_REQUIRE(c_opt);
    std::string_view c{ *c_opt };
    BOOST_TEST(c == "c"sv);

    it = ctx.begin();
    std::advance(it, 1);
    BOOST_CHECK(it == ctx.end());
}

BOOST_AUTO_TEST_CASE(parse_arg_formatter)
{
    std::formatter<my_int> f;
    std::format_parse_context ctx{ "{:x}", 1 };

    auto it = ctx.begin();
    ++it;
    ctx.advance_to(it);

    f.parse(ctx);

    BOOST_TEST(std::format("{:x}", my_int{ 11 }) == "0XB");
}

BOOST_AUTO_TEST_SUITE_END() // io
BOOST_AUTO_TEST_SUITE_END() // cmn

