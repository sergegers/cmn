
#include <array>

#include <boost/lexical_cast.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/tools/output_test_stream.hpp>

#include <cmn/meta/concepts.h>
#include <cmn/enum/traits.h>
#include <cmn/enum/op.h>
#include <cmn/error/exception.h>
#include <cmn/util/lexical_cast.h>
#include <cmn/enum/manip.h>
#include <cmn/enum/io.h>
#include <cmn/enum/enum.h>
#include <cmn/enum/bitfield.h>
#include <cmn/enum/combo.h>
#include <cmn/enum/traits.h>
#include <cmn/enum/util.h>

#include <cmn/tuple/io.h>

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

enum class cl_cmb_t
{
    zero       = 0x0,
    one        = 0x1,
    two        = 0x2,
    three      = 0x3,

    red        = 0x4,
    green      = 0x8,
    blue       = 0xC,

    digit_mask = 0x3,
    color_mask = 0xC
};

consteval auto adapt_enum_info(cl_cmb_t)
{
    using enum cl_cmb_t;
    return adapt_combo_info_helper(groups_info{ group_::make<zero, one, two, three>(), group_::make<red, green, blue>() },
         default_ops(kind_t::combo) | op_interoperable);
}

CMN_PP_INJECT_ENUM_OPS()

static_assert(std::is_same_v<mask_type_t<cl_cmb_t>, unsigned int>);
static_assert(ops_v<cl_cmb_t> == (op_comparable | op_steppable | op_bitwise | op_io | op_interoperable));
static_assert(std::tuple_size_v<decltype(groups_v<cl_cmb_t>)> == 2);
static_assert(kind_v<cl_cmb_t> == kind_t::combo);

// magic get
static_assert(magic_enum_name_v<cl_cmb_t> == "cmn::enum_::cl_cmb_t");
static_assert(magic_enum_wname_v<cl_cmb_t> == L"cmn::enum_::cl_cmb_t");
static_assert(magic_enum_member_name_v<cl_cmb_t::one> == "cmn::enum_::cl_cmb_t::one");
static_assert(magic_enum_member_wname_v<cl_cmb_t::one> == L"cmn::enum_::cl_cmb_t::one");

// qualified name
static_assert(qualified_name{ cl_cmb_t{} }.m_ns == "cmn::enum_");
static_assert(qualified_name{ cl_cmb_t{} }.m_enum_name == "cl_cmb_t");
// qualified member name
static_assert(qualified_member_name{ make_int_t<cl_cmb_t::green>{} }.m_ns == "cmn::enum_");
static_assert(qualified_member_name{ make_int_t<cl_cmb_t::green>{} }.m_enum_name == "cl_cmb_t");
static_assert(qualified_member_name{ make_int_t<cl_cmb_t::green>{} }.m_enum_member_name == "green");

BOOST_AUTO_TEST_CASE(masks)
{
    using traits_type = traits<cl_cmb_t>;
    using enum cl_cmb_t;

    static_assert
    (
        std::is_same_v<group_::make_t<zero, one, three, two>, 
        group_info<record_info<zero>, record_info<one>, record_info<two>, record_info<three>>>
    );

    constexpr decltype(auto) enum_info = traits_type::enum_info;
    static_assert(std::tuple_size_v<decltype(enum_info.m_groups)> == 2, "Enum group count mismatch");

    BOOST_TEST(boost::lexical_cast<std::string>(std::get<1>(std::get<1>(enum_info.m_groups))) == "green");

    static_assert(group_::get_enum_values(std::get<0>(enum_info.m_groups)) == std::array { zero, one, two, three });
    static_assert(group_::get_enum_values(std::get<1>(enum_info.m_groups)) == std::array { red, green, blue });

    constexpr auto masks = calc_masks(enum_info);
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

BOOST_AUTO_TEST_CASE(enum_class_zero_out)
{
    cl_cmb_t e { cl_cmb_t::zero | cl_cmb_t::red };
    
    output_test_stream tstr;
    tstr << e;
    BOOST_CHECK(tstr.is_equal("[zero red]"));
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

CMN_PP_DEFINE_COMBO
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

CMN_PP_DEFINE_BITFIELD
(
    bf_t,
    (bf_zero,  0x0)
    (bf_one,   0x1)
    (bf_two,   0x2)
    (bf_four,  0x4)
)

static_assert(kind_v<bf_t> == kind_t::bitfield);
static_assert(std::tuple_size_v<decltype(groups_v<bf_t>)> == 4);

using namespace op;

BOOST_AUTO_TEST_CASE(read_bitfield)
{
    using namespace std::string_literals;

    BOOST_TEST(bf_two == boost::lexical_cast<bf_t>("[bf_two]"s));
    BOOST_TEST((bf_two | bf_four) == boost::lexical_cast<bf_t>("[bf_two bf_four]"s));
}

CMN_PP_DEFINE_ENUM
(
    en_t,
    (en_apple, 4)
    (en_banana)
    (en_carrot, 17)
    (en_cherry)
)

// magic get
static_assert(magic_enum_member_name_v<en_apple> == "cmn::enum_::en_apple");
static_assert(magic_enum_member_wname_v<en_apple> == L"cmn::enum_::en_apple");
// qualified member name
static_assert(qualified_member_name{ make_int_t<en_apple>{} }.m_ns == "cmn::enum_");
static_assert(qualified_member_name{ make_int_t<en_apple>{} }.m_enum_name == "en_t");
static_assert(qualified_member_name{ make_int_t<en_apple>{} }.m_enum_member_name == "en_apple");

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

CMN_PP_DEFINE_ENUM_CLASS
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

CMN_PP_DEFINE_ENUM_CLASS
(
    uns_en_t,
    (four, 4)
    (one, 1)
    (minus_one, -1)
    (seventeen, 17)
    (five, 5)
    (six, 6)
)

BOOST_AUTO_TEST_CASE(unsorted_enum)
{
    using enum uns_en_t;

    BOOST_TEST(begin_v<uns_en_t> == uns_en_t::minus_one);
    BOOST_TEST(last_v<uns_en_t> == uns_en_t::seventeen);

    constexpr auto lhs = group_::get_enum_values(std::get<0>(traits<uns_en_t>::enum_info.m_groups));
    constexpr auto rhs = std::array{ minus_one, one, four, five, six, seventeen };
    BOOST_CHECK_EQUAL_COLLECTIONS
    (
        std::begin(lhs), std::end(lhs),
        std::begin(rhs), std::end(rhs)
    );
}

}

}

namespace outer_ns
{

struct outer_t
{
    enum inner_t
    {
        i_a0, i_a1, i_a2
    };
};

CMN_PP_ADAPT_ENUM
(
    outer_t::inner_t,
    (i_a0)
    (i_a1)
    (i_a2)
)

}

namespace cmn
{

namespace enum_
{

static_assert(kind_v<outer_ns::outer_t::inner_t> == kind_t::enum_);

BOOST_AUTO_TEST_CASE(inner_enum)
{
    using namespace outer_ns;
    using enum outer_t::inner_t;

    outer_t::inner_t e { i_a2 };
    
    output_test_stream tstr;
    tstr << e;
    BOOST_CHECK(tstr.is_equal("[i_a2]"));
}

BOOST_AUTO_TEST_SUITE_END() // enum_
BOOST_AUTO_TEST_SUITE_END() // cmn

#pragma warning(pop)