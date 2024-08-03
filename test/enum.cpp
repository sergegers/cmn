
#include <ranges>
#include <array>

#include <boost/lexical_cast.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/tools/output_test_stream.hpp>

#include <cmn/error/exception.h>
#include <cmn/enum/manip.h>
#include <cmn/enum/io.h>
#include <cmn/util/util.h>
#include <cmn/enum/enum.h>
#include <cmn/enum/bitfield.h>
#include <cmn/enum/combo.h>
#include <cmn/enum/traits.h>
#include <cmn/enum/util.h>

#pragma warning(push)
#pragma warning(disable: 4002)

BOOST_AUTO_TEST_SUITE(cmn)

static_assert(enum_::next_on_mask(0b0111) == 0b1000);
static_assert(enum_::next_on_mask(0b0111) != 0b1001);
static_assert(enum_::next_on_mask(0b0100000100) == 0b1000000000);

BOOST_AUTO_TEST_SUITE(enum_)

using boost::test_tools::output_test_stream;
using boost::test_tools::per_element;

using namespace io;

CMN_PP_DECLARE_COMBO_CLASS
(
    cl_cmb_t,
    (
        (one,       0x0)
        (two,       0x1)
        (three,     0x2)
    )
    (
        (red,       0x0)
        (green,     0x4)
        (blue,      0x8)
    ),

    (digit_mask,    0x3)
    (color_mask,    0xC)
)

BOOST_AUTO_TEST_CASE(masks)
{
    using traits_type = traits<cl_cmb_t>;
    using enum cl_cmb_t;

    constexpr decltype(auto) groups = traits_type::groups;
    static_assert(std::tuple_size_v<decltype(groups)> == 2, "Enum group count mismatch");

    BOOST_TEST(traits_type::get_str<char>(std::get<1>(groups), green) == "green");

    BOOST_TEST
    (
        (std::get<0>(groups) | std::views::transform([](auto const &rec) { return rec.m_val; })) ==
        (std::array { one, two, three }),
        per_element{}
    );
    BOOST_TEST
    (
        (std::get<1>(groups) | std::views::transform([](auto const &rec) { return rec.m_val; })) ==
        (std::array { red, green, blue }),
        per_element{}
    );

    constexpr auto masks = utils<cl_cmb_t>::calc_masks(groups);
    static_assert(masks.size() == 2, "size mismatch");
    static_assert(std::get<0>(masks) == digit_mask, "digit mask mismatch");
    static_assert(std::get<1>(masks) == color_mask, "color mask mismatch");
}

BOOST_AUTO_TEST_CASE(enum_class_ops)
{
    using enum cl_cmb_t;

    cl_cmb_t e{ two };
    e |= blue;
    BOOST_TEST(e == (cl_cmb_t::two | cl_cmb_t::blue));

    e &= digit_mask;
    BOOST_TEST(e == two);
}

BOOST_AUTO_TEST_CASE(enum_class_out)
{
    cl_cmb_t e { cl_cmb_t::one | cl_cmb_t::green };

    {
        output_test_stream tstr;
        tstr << e;
        BOOST_CHECK(tstr.is_equal("[one green]"));
    }

    {
        output_test_stream tstr;
        tstr << bitfield_mask(cl_cmb_t::digit_mask) << e;
        BOOST_CHECK(tstr.is_equal("[one]"));
    }

    {
        output_test_stream tstr;
        tstr << bitfield_mask(cl_cmb_t::color_mask) << e;
        BOOST_CHECK(tstr.is_equal("[green]"));
    }
}

CMN_PP_DECLARE_COMBO
(
    cmb_t,
    (
        (one,       0x0)
        (two,       0x1)
        (three,     0x2)
    )
    (
        (red,       0x0)
        (green,     0x4)
        (blue,      0x8)
    ),

    (digit_mask,    0x3)
    (color_mask,    0xC)
)

BOOST_AUTO_TEST_CASE(enum_out)
{
    cmb_t e { one | green };

    {
        output_test_stream tstr;
        tstr << e;
        BOOST_CHECK(tstr.is_equal("[one green]"));
    }

    {
        output_test_stream tstr;
        tstr << bitfield_mask(digit_mask) << e;
        BOOST_CHECK(tstr.is_equal("[one]"));
    }

    {
        output_test_stream tstr;
        tstr << bitfield_mask(color_mask) << e;
        BOOST_CHECK(tstr.is_equal("[green]"));
    }
}

BOOST_AUTO_TEST_CASE(open_close)
{
    using namespace std::string_literals;

    std::ostringstream ostr;
    constexpr auto val = two | blue;
    ostr << val;
    BOOST_CHECK_MESSAGE(ostr.str() == "[two blue]", ostr.str());
    BOOST_TEST(open_manip::value(ostr) == "[");
    BOOST_TEST(close_manip::value(ostr) == "]");

    ostr.str(""s);
    
    ostr << eopen() << eclose();
    BOOST_TEST(open_manip::value(ostr) == "");
    BOOST_TEST(close_manip::value(ostr) == "");

    ostr << val;
    BOOST_CHECK_MESSAGE(ostr.str() == "two blue", ostr.str());

    ostr.str(""s);

    ostr << eopen("{ ") << eclose(" }");
    BOOST_TEST(open_manip::value(ostr) == "{ ");
    BOOST_TEST(close_manip::value(ostr) == " }");

    ostr << val;
    BOOST_CHECK_MESSAGE(ostr.str() == "{ two blue }", ostr.str());

    ostr.str(""s);

    ostr << eopen() << eclose();
    BOOST_TEST(open_manip::value(ostr) == "");
    BOOST_TEST(close_manip::value(ostr) == "");

    ostr << val;
    BOOST_CHECK_MESSAGE(ostr.str() == "two blue", ostr.str());
}

BOOST_AUTO_TEST_CASE(save_flags)
{
    using namespace std::string_literals;

    std::ostringstream ostr;
    constexpr auto val = two | blue;

    BOOST_TEST(open_manip::value(ostr) == "[");
    BOOST_TEST(close_manip::value(ostr) == "]");

    {        
        boost::io::ios_iword_saver eos { ostr, open_manip::index() };
        boost::io::ios_iword_saver ecs { ostr, close_manip::index() };

        ostr << eopen() << eclose();
        BOOST_TEST(open_manip::value(ostr) == "");
        BOOST_TEST(close_manip::value(ostr) == "");

        ostr << val;
        BOOST_CHECK_MESSAGE(ostr.str() == "two blue", ostr.str());
    
        ostr.str(""s);
    }

    BOOST_TEST(open_manip::value(ostr) == "[");
    BOOST_TEST(close_manip::value(ostr) == "]");

    {    
        ostr << eopen("<") << eclose(">");
        BOOST_TEST(open_manip::value(ostr) == "<");
        BOOST_TEST(close_manip::value(ostr) == ">");

        ostr << val;
        BOOST_CHECK_MESSAGE(ostr.str() == "<two blue>", ostr.str());
    
        ostr.str(""s);
    }

    using cmn::io::reset_;
    ostr << eopen(reset_) << eclose(reset_);
    BOOST_TEST(open_manip::value(ostr) == "[");
    BOOST_TEST(close_manip::value(ostr) == "]");

    ostr << val;
    BOOST_CHECK_MESSAGE(ostr.str() == "[two blue]", ostr.str());
}

BOOST_AUTO_TEST_CASE(lex_cast)
{
    using enum cl_cmb_t;
    using namespace std::string_literals;

    BOOST_TEST(boost::lexical_cast<std::string>(one | green) == "[one green]"s);
}

BOOST_AUTO_TEST_CASE(read_combo)
{
    using enum cl_cmb_t;
    using namespace std::string_literals;

    BOOST_TEST((green | two) == boost::lexical_cast<cl_cmb_t>("[green two]"s));
    BOOST_TEST(two == boost::lexical_cast<cl_cmb_t>("[two]"s));

    {
        std::istringstream sstr{ "[red three]" };
        cl_cmb_t e{};
        sstr >> std::noskipws >> e;
        BOOST_TEST(e == (red | three));
    }

    {
        std::istringstream sstr{ " [ blue one ] " };
        cl_cmb_t e{};
        sstr >> std::noskipws >> e;
        BOOST_TEST(e == (blue | one));
    }

    {
        std::istringstream sstr{ "red three" };
        sstr >> std::noskipws >> eopen() >> eclose();
        cl_cmb_t e{};
        sstr >> e;
        BOOST_TEST(e == (red | three));
    }
}

CMN_PP_DECLARE_BITFIELD
(
    bf_t,
    (bf_zero,  0x0)
    (bf_one,   0x1)
    (bf_two,   0x2)
    (bf_four,  0x4)
)

BOOST_AUTO_TEST_CASE(read_bitfield)
{
    using namespace std::string_literals;

    BOOST_TEST(bf_two == boost::lexical_cast<bf_t>("[bf_two]"s));
    BOOST_TEST((bf_two | bf_four) == boost::lexical_cast<bf_t>("[ bf_two bf_four ]"s));
}

CMN_PP_DECLARE_ENUM
(
    en_t,
    (en_apple, 4)
    (en_banana)
    (en_carrot, 17)
    (en_cherry)
)

BOOST_AUTO_TEST_CASE(read_enum)
{
    using namespace std::string_literals;

    BOOST_TEST(en_apple == boost::lexical_cast<en_t>("[en_apple]"s));
    BOOST_CHECK_THROW(boost::lexical_cast<en_t>("en_apple"s), cmn::io_error);

    std::stringstream sstr;

    sstr << eopen() << eclose();
    sstr << en_carrot;

    en_t out = static_cast<en_t>(0);
    sstr >> std::skipws >> out;
    BOOST_TEST(out == en_carrot);
}

CMN_PP_DECLARE_ENUM_CLASS
(
    cl_en_t,
    (apple, 4)
    (banana)
    (carrot, 17)
    (cherry)
)

BOOST_AUTO_TEST_CASE(class_prefix_)
{
    using enum print_t;
    {
        using enum cl_cmb_t;
        std::stringstream sstr;
        sstr << eprint(class_prefix);

        constexpr auto in = two | green;
        sstr << in;
        BOOST_TEST(sstr.str() == "[cl_cmb_t::two cl_cmb_t::green]");

        cl_cmb_t out;
        sstr >> std::noskipws >> out;
        BOOST_TEST(out == in);
    }

    {
        std::stringstream sstr;
        sstr << eprint(class_prefix);

        using enum cl_en_t;

        constexpr auto in = banana;
        sstr << in;
        BOOST_TEST(sstr.str() == "[cl_en_t::banana]");

        cl_en_t out;
        sstr >> std::noskipws >> out;
        BOOST_TEST(out == in);
    }
}

CMN_PP_DECLARE_ENUM_CLASS
(
    uns_en_t,
    (four, 4)
    (one, 1)
    (minus_one, -1)
    (seventeen, 17)
    (five, 5)
    (six, 6)
)

static_assert(begin_v<uns_en_t> == uns_en_t::minus_one);
static_assert(last_v<uns_en_t> == uns_en_t::seventeen);

BOOST_AUTO_TEST_CASE(unsorted_enum)
{
    using record_type = utils<uns_en_t>::record_type;
    using enum uns_en_t;

    auto const lhs = std::get<0>(traits<uns_en_t>::groups) | std::views::transform(&record_type::m_val);
    constexpr auto rhs = std::array{ minus_one, one, four, five, six, seventeen };
    BOOST_CHECK_EQUAL_COLLECTIONS
    (
        std::begin(lhs), std::end(lhs),
        std::begin(rhs), std::end(rhs)
    );
}

BOOST_AUTO_TEST_SUITE_END() // enum_
BOOST_AUTO_TEST_SUITE_END() // cmn

#pragma warning(pop)