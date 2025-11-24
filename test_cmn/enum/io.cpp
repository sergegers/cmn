
#include <array>
#include <format>
#include <algorithm>

#include <boost/lexical_cast.hpp>
#include <boost/io/ios_state.hpp>

#include <boost/test/unit_test.hpp>
#include <boost/test/tools/output_test_stream.hpp>

#include <cmn/meta/concepts.h>
#include <cmn/error/exception.h>

#include <cmn/io/format.h>

#include <cmn/enum/op.h>
#include <cmn/enum/io.h>

#include "defs.h"

BOOST_AUTO_TEST_SUITE(cmn)

BOOST_AUTO_TEST_SUITE(enum_)
BOOST_AUTO_TEST_SUITE(io)

using boost::test_tools::output_test_stream;
using boost::test_tools::per_element;

using namespace io;

BOOST_AUTO_TEST_CASE(enum_class_zero_out)
{
    cl_cmb_2_t e { cl_cmb_2_t::zero | cl_cmb_2_t::red };
    
    output_test_stream tstr;
    tstr << e;
    BOOST_CHECK(tstr.is_equal("[zero red]"));
}

BOOST_AUTO_TEST_CASE(enum_class_out)
{
    cl_cmb_2_t e { cl_cmb_2_t::one | cl_cmb_2_t::green };

    {
        output_test_stream tstr;
        tstr << e;
        BOOST_CHECK(tstr.is_equal("[one green]"));
    }

    {
        output_test_stream tstr;
        tstr << bitfield_mask(cl_cmb_2_t::digit_mask) << e;
        BOOST_CHECK(tstr.is_equal("[one]"));
    }

    {
        output_test_stream tstr;
        tstr << bitfield_mask(cl_cmb_2_t::color_mask) << e;
        BOOST_CHECK(tstr.is_equal("[green]"));
    }
}

BOOST_AUTO_TEST_CASE(enum_out)
{
    auto const e = static_cast<zero_cmb_t>(one | green);

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

BOOST_AUTO_TEST_CASE(double_zero)
{
    auto const e = static_cast<zero_cmb_t>(one | red);

    {
        output_test_stream tstr;
        tstr << e;
        BOOST_CHECK(tstr.is_equal("[one red]"));
    }

    {
        output_test_stream tstr;
        tstr << bitfield_mask(digit_mask) << e;
        BOOST_CHECK(tstr.is_equal("[one]"));
    }

    {
        output_test_stream tstr;
        tstr << bitfield_mask(color_mask) << e;
        BOOST_CHECK(tstr.is_equal("[red]"));
    }
}

BOOST_AUTO_TEST_CASE(open_close)
{
    using namespace std::string_literals;

    std::ostringstream ostr;
    auto const val = static_cast<zero_cmb_t>(two | blue);
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
    constexpr auto val = static_cast<zero_cmb_t>(two | blue);

    BOOST_TEST(open_manip::value(ostr) == "[");
    BOOST_TEST(close_manip::value(ostr) == "]");

    {        
        boost::io::ios_iword_saver eos { ostr, open_manip::index(ostr) };
        boost::io::ios_iword_saver ecs { ostr, close_manip::index(ostr) };

        ostr << eopen() << eclose();
        BOOST_TEST(open_manip::value(ostr) == "");
        BOOST_TEST(close_manip::value(ostr) == "");

        ostr << val;
        BOOST_TEST(ostr.str() == "two blue");
    
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
    using enum cl_cmb_2_t;
    using namespace std::string_literals;

    BOOST_TEST(boost::lexical_cast<std::string>(one | green) == "[one green]"s);
}

BOOST_AUTO_TEST_CASE(read_combo)
{
    using enum cl_cmb_2_t;
    using namespace std::string_literals;

    {
        std::istringstream sstr{ "[red three]" };
        cl_cmb_2_t e{};
        sstr >> std::noskipws >> e;
        BOOST_TEST(e == (red | three));
    }

    {
        std::istringstream sstr{ " [ blue one ] " };
        cl_cmb_2_t e{};
        sstr >> std::noskipws >> e;
        BOOST_TEST(e == (blue | one));
    }

    {
        std::istringstream sstr{ "red three" };
        sstr >> std::noskipws >> eopen() >> eclose();
        cl_cmb_2_t e{};
        sstr >> e;
        BOOST_TEST(e == (red | three));
    }

    BOOST_TEST((green | two) == boost::lexical_cast<cl_cmb_2_t>("[green two]"s));
    BOOST_TEST(two == boost::lexical_cast<cl_cmb_2_t>("[two]"s));
}

using namespace op;

BOOST_AUTO_TEST_CASE(read_bitfield)
{
    using namespace std::string_literals;

    BOOST_TEST(bf_two == boost::lexical_cast<bf_t>("[bf_two]"s));
    BOOST_TEST((bf_two | bf_four) == boost::lexical_cast<bf_t>("[bf_two bf_four]"s));
}

BOOST_AUTO_TEST_CASE(read_enum)
{
    using namespace std::string_literals;

    BOOST_TEST(en_apple == boost::lexical_cast<en_2_t>("[en_2_apple]"s));
    BOOST_CHECK_THROW(boost::lexical_cast<en_2_t>("en_2_apple"s), cmn::io_error);

    std::stringstream sstr;

    sstr << eopen() << eclose();
    sstr << en_2_carrot;

    en_2_t out = static_cast<en_2_t>(0);
    sstr >> std::skipws >> out;
    BOOST_TEST(out == en_2_carrot);
}

BOOST_AUTO_TEST_CASE(class_prefix_)
{
    using enum print_t;
    {
        using enum cl_cmb_2_t;
        std::stringstream sstr;
        sstr << eprint(class_prefix);

        constexpr auto in = two | green;
        sstr << in;
        BOOST_TEST(sstr.str() == "[cl_cmb_2_t::two cl_cmb_2_t::green]");

        cl_cmb_2_t out;
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

BOOST_AUTO_TEST_CASE(unsorted_enum)
{
    using enum uns_en_t;

    BOOST_TEST(begin_v<uns_en_t> == uns_en_t::minus_one);
    BOOST_TEST(last_v<uns_en_t> == uns_en_t::seventeen);

    constexpr auto lhs = std::get<0>(groups_v<uns_en_t>).get_values();
    constexpr auto rhs = std::array{ minus_one, one, four, five, six, seventeen };
    BOOST_CHECK_EQUAL_COLLECTIONS
    (
        std::begin(lhs), std::end(lhs),
        std::begin(rhs), std::end(rhs)
    );
}

BOOST_AUTO_TEST_CASE(inner_enum)
{
    using namespace outer_ns;
    using enum outer_t::inner_t;

    outer_t::inner_t e { i_a2 };
    
    output_test_stream tstr;
    tstr << e;
    BOOST_CHECK(tstr.is_equal("[i_a2]"));
}

BOOST_AUTO_TEST_CASE(large_enum)
{
    using enum large_enum_t;

    {
        std::stringstream sstr;
        sstr << e35;
        BOOST_TEST(sstr.str() == "[e35]");
    }

    {
        std::stringstream sstr;
        sstr << e63;
        BOOST_TEST(sstr.str() == "[e63]");
    }

    {
        std::stringstream sstr;
        sstr << e127;
        BOOST_TEST(sstr.str() == "[e127]");
    }

    {
        std::stringstream sstr;
        sstr << e255;
        BOOST_TEST(sstr.str() == "[e255]");
    }

    {
        std::stringstream sstr;
        sstr << e303;
        BOOST_TEST(sstr.str() == "[e303]");
    }

    {
        std::stringstream sstr;
        sstr << e335;
        BOOST_TEST(sstr.str() == "[e335]");
    }

    {
        std::stringstream sstr;
        sstr << e447;
        BOOST_TEST(sstr.str() == "[e447]");
    }

    {
        std::stringstream sstr;
        sstr << e511;
        BOOST_TEST(sstr.str() == "[e511]");
    }

    {
        std::stringstream sstr;
        sstr << e623;
        BOOST_TEST(sstr.str() == "[e623]");
    }

    {
        std::stringstream sstr;
        sstr << e735;
        BOOST_TEST(sstr.str() == "[e735]");
    }

    {
        std::stringstream sstr;
        sstr << e879;
        BOOST_TEST(sstr.str() == "[e879]");
    }

    {
        std::stringstream sstr;
        sstr << e1023;
        BOOST_TEST(sstr.str() == "[e1023]");
    }

    {
        std::stringstream sstr;
        sstr << e1071;
        BOOST_TEST(sstr.str() == "[e1071]");
    }
}

BOOST_AUTO_TEST_CASE(anonymous_enum)
{
    output_test_stream tstr;
    tstr << foo::ae_1;
    BOOST_CHECK(tstr.is_equal("[ae_1]"));

    // TODO: what about using print_t::class_prefix option?
}

BOOST_AUTO_TEST_CASE(tail)
{
    output_test_stream tstr;
    tstr << eprint(print_t::tail) << static_cast<zero_cmb_t>(one | green | 0x1000);
    BOOST_CHECK(tstr.is_equal("[one green 0X1000]"));

}

BOOST_AUTO_TEST_CASE(adapt_global_ns)
{
    static_assert(kind_v< ::drc_t> == kind_t::enum_);
    static_assert(ops_v< ::drc_t> == op_io);

    std::ostringstream ostr;
    ostr << ::DRC_NOCHG;
    BOOST_TEST(ostr.str() == "[DRC_NOCHG]");
}

using enum cmn::io::list_options_t;

static_assert(cmn::io::traits<cl_en_t>::fmt_options == (lo_brackers | lo_separator));
static_assert(std::formattable<cl_en_t, char>);
static_assert(std::formattable<cmn::io::list<cl_en_t>, char>);

BOOST_AUTO_TEST_CASE(format_enum)
{
    using enum cl_en_t;
    using cmn::io::list;

    BOOST_TEST(std::format("{}", apple) == "[apple]");
    BOOST_TEST(std::format("{0:<: :>}", list{ apple }) == "<apple>");
    BOOST_TEST(std::format("{0:[[: :>>}", list{ apple }) == "[[apple>>");
    BOOST_TEST(std::format("{0:<<: :>>}", list{ apple }) == "<<apple>>");
    BOOST_TEST(std::format("{0:: :>>}", list{ apple }) == "apple>>");
    BOOST_TEST(std::format("{0:::}", list{ apple }) == "apple");
}

static_assert(cmn::io::traits<cl_cmb_2_t>::fmt_options == (lo_brackers | lo_separator));
static_assert(std::formattable<cl_cmb_2_t, char>);
static_assert(std::formattable<cmn::io::list<cl_cmb_2_t>, char>);

BOOST_AUTO_TEST_CASE(format_combo)
{
    using enum cl_cmb_2_t;
    using cmn::io::list;

    BOOST_TEST(std::format("{}", red) == "[zero red]");
    BOOST_TEST(std::format("{}", red | two) == "[two red]");
    BOOST_TEST(std::format("{:<: :>}", list{ red | two }) == "<two red>");
    BOOST_TEST(std::format("{:: :>}", list{ red | two }) == "two red>");
    BOOST_TEST(std::format("{:::>}", list{ red | two }) == "twored>");
    BOOST_TEST(std::format("{:::}", list{ red | two }) == "twored");
    BOOST_TEST(std::format("{:::}", list<zero_cmb_t>{ zero_cmb_t::two | zero_cmb_t::green }) == "twogreen");
    BOOST_TEST(std::format("{:{{: :}}}", list{ red | two }) == "{two red}");
}

BOOST_AUTO_TEST_CASE(format_tail)
{
    using enum cl_cmb_2_t;
    using cmn::io::list;

    BOOST_TEST(std::format("{}", red | 0x1000) == "[zero red 0X1000]");
}

BOOST_AUTO_TEST_SUITE_END() // io
BOOST_AUTO_TEST_SUITE_END() // enum_
BOOST_AUTO_TEST_SUITE_END() // cmn
