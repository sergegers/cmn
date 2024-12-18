
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

//====================================================================
/// Debugger return codes.
/// Success if positive (> DRC_NONE).
enum drc_t
{
  DRC_EVENTS = 3,   ///< success, there are pending events
  DRC_CRC    = 2,   ///< success, but the input file crc does not match
  DRC_OK     = 1,   ///< success
  DRC_NONE   = 0,   ///< reaction to the event not implemented
  DRC_FAILED = -1,  ///< failed or false
  DRC_NETERR = -2,  ///< network error
  DRC_NOFILE = -3,  ///< file not found
  DRC_IDBSEG = -4,  ///< use idb segmentation
  DRC_NOPROC = -5,  ///< the process does not exist anymore
  DRC_NOCHG  = -6,  ///< no changes
  DRC_ERROR  = -7,  ///< unclassified error, may be complemented by errbuf
};

CMN_ENUM_ADAPT_ENUM
(
    ::drc_t,
    (DRC_ERROR)     // = -7,  ///< unclassified error, may be complemented by errbuf
    (DRC_NOCHG)     // = -6,  ///< no changes
    (DRC_NOPROC)    // = -5,  ///< the process does not exist anymore
    (DRC_IDBSEG)    // = -4,  ///< use idb segmentation
    (DRC_NOFILE)    // = -3,  ///< file not found
    (DRC_NETERR)    // = -2,  ///< network error
    (DRC_FAILED)    // = -1,  ///< failed or false
    (DRC_NONE)      // = 0,   ///< reaction to the event not implemented
    (DRC_OK)        // = 1,   ///< success
    (DRC_CRC)       // = 2,   ///< success, but the input file crc does not match
    (DRC_EVENTS)    // = 3,   ///< success, there are pending events
);

static_assert(cmn::enum_::kind_v< ::drc_t> == cmn::enum_::kind_t::enum_);
static_assert(cmn::enum_::ops_v< ::drc_t> == cmn::enum_::op_io);

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

    digit_mask = zero | one | two | three,
    color_mask = red | green | blue
};

consteval auto adapt_enum_info(cl_cmb_t en)
{
    using enum cl_cmb_t;
    return enum_info
    {
          default_ops(en, kind_t::combo) | op_interoperable
        , group_::make<zero, one, two, three>(), group_::make<red, green, blue>()
    };
}

CMN_ENUM_INJECT_OPS()

static_assert(std::is_same_v<interop_type_t<cl_cmb_t>, int>);
static_assert(c::adapted_enum<cl_cmb_t>);
static_assert(ops_v<cl_cmb_t> == (op_comparable | op_steppable | op_bitwise | op_io | op_interoperable));
static_assert(std::tuple_size_v<decltype(groups_v<cl_cmb_t>)> == 2);
static_assert(kind_v<cl_cmb_t> == kind_t::combo);
static_assert(c::bitfield<cl_cmb_t>);

// magic get
static_assert(magic_enum_name_v<cl_cmb_t> == "cmn::enum_::cl_cmb_t");
static_assert(magic_enum_wname_v<cl_cmb_t> == L"cmn::enum_::cl_cmb_t");
static_assert(magic_enum_member_name_v<cl_cmb_t::one> == "cmn::enum_::cl_cmb_t::one");
static_assert(magic_enum_member_wname_v<cl_cmb_t::one> == L"cmn::enum_::cl_cmb_t::one");

// qualified name
static_assert(qualified_name{ cl_cmb_t{} }.m_ns == "cmn::enum_");
static_assert(qualified_name{ cl_cmb_t{} }.m_enum_name == "cl_cmb_t");
// qualified member name
static_assert(qualified_member_name{ int_<cl_cmb_t::green>{} }.m_ns == "cmn::enum_");
static_assert(qualified_member_name{ int_<cl_cmb_t::green>{} }.m_enum_name == "cl_cmb_t");
static_assert(qualified_member_name{ int_<cl_cmb_t::green>{} }.m_enum_member_name == "green");

BOOST_AUTO_TEST_CASE(masks)
{
    using enum cl_cmb_t;

    static_assert
    (
        group_::make<zero, one, three, two>() ==
        group_info{ int_<zero>{}, int_<one>{}, int_<two>{}, int_<three>{} }
    );

    constexpr decltype(auto) enum_info_ = enum_info_v<cl_cmb_t>;
    static_assert(std::tuple_size_v<decltype(enum_info_.m_groups)> == 2, "Enum group count mismatch");

    BOOST_TEST(boost::lexical_cast<std::string>(std::get<1>(enum_info_.m_groups).m_records[1]) == "green");

    static_assert(std::get<0>(enum_info_.m_groups).get_values() == std::array { zero, one, two, three });
    static_assert(std::get<1>(enum_info_.m_groups).get_values() == std::array { red, green, blue });

    constexpr auto masks = enum_info_.m_masks;
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

enum cmb_t
{
    one = 0x0,
    two = 0x1,
    three = 0x2,

    red = 0x0,
    green   = 0x4,
    blue = 0x8,

    digit_mask = 0x3,
    color_mask = 0xC
};

consteval auto adapt_enum_info(cmb_t en)
{
    return enum_info
    {
          default_ops(en, kind_t::combo)
        , group_info{ CMN_ENUM_RECORD(one), record_::make<two>(), record_::make<three>() }
        , group_info{ CMN_ENUM_RECORD(red), record_::make<green>(), record_::make<blue>() }
    };
}

static_assert(c::adapted_enum<cmb_t>);
static_assert(c::bitfield<cmb_t>);
static_assert(kind_v<cmb_t> == kind_t::combo);
static_assert(ops_v<cmb_t> == op_io);

BOOST_AUTO_TEST_CASE(enum_out)
{
    auto const e = static_cast<cmb_t>(one | green);

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
    auto const e = static_cast<cmb_t>(one | red);

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
    auto const val = static_cast<cmb_t>(two | blue);
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
    constexpr auto val = static_cast<cmb_t>(two | blue);

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

CMN_ENUM_DEFINE_BITFIELD
(
    bf_t,
    (bf_zero,  0x0)
    (bf_one,   0x1)
    (bf_two,   0x2)
    (bf_four,  0x4)
)

static_assert(kind_v<bf_t> == kind_t::bitfield);
static_assert(std::tuple_size_v<decltype(groups_v<bf_t>)> == 4);
static_assert(c::bitfield<bf_t>);
//static_assert(!c::strong_bitfield<bf_t>);

using namespace op;

BOOST_AUTO_TEST_CASE(read_bitfield)
{
    using namespace std::string_literals;

    BOOST_TEST(bf_two == boost::lexical_cast<bf_t>("[bf_two]"s));
    BOOST_TEST((bf_two | bf_four) == boost::lexical_cast<bf_t>("[bf_two bf_four]"s));
}

CMN_ENUM_DEFINE_ENUM
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
static_assert(qualified_member_name{ int_<en_apple>{} }.m_ns == "cmn::enum_");
static_assert(qualified_member_name{ int_<en_apple>{} }.m_enum_name == "en_t");
static_assert(qualified_member_name{ int_<en_apple>{} }.m_enum_member_name == "en_apple");
static_assert(kind_v<en_t> == kind_t::enum_);
static_assert(ops_v<en_t> == (op_io));

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

CMN_ENUM_DEFINE_ENUM_CLASS
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

CMN_ENUM_DEFINE_ENUM_CLASS
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

    constexpr auto lhs = std::get<0>(groups_v<uns_en_t>).get_values();
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

CMN_ENUM_ADAPT_ENUM
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

CMN_ENUM_DEFINE_BITFIELD_CLASS
(
    bf2_t,
    (bf2_1,     0x1)
    (bf2_2,     0x2)
    (bf2_4,     0x4)
);

static_assert(c::strong_bitfield<bf2_t>);
static_assert(c::bitfield<bf2_t>);

BOOST_AUTO_TEST_CASE(adapt_global_ns)
{
    std::ostringstream ostr;
    ostr << ::DRC_NOCHG;
    BOOST_TEST(ostr.str() == "[DRC_NOCHG]");
}

enum class large_enum_t
{
    e0, e1, e2, e3, e4, e5, e6, e7, e8, e9, e10, e11, e12, e13, e14, e15,
    e16, e17, e18, e19, e20, e21, e22, e23, e24, e25, e26, e27, e28, e29, e30, e31,
    e32, e33, e34, e35, e36, e37, e38, e39, e40, e41, e42, e43, e44, e45, e46, e47,
    e48, e49, e50, e51, e52, e53, e54, e55, e56, e57, e58, e59, e60, e61, e62, e63,
    e64, e65, e66, e67, e68, e69, e70, e71, e72, e73, e74, e75, e76, e77, e78, e79,
    e80, e81, e82, e83, e84, e85, e86, e87, e88, e89, e90, e91, e92, e93, e94, e95,
    e96, e97, e98, e99, e100, e101, e102, e103, e104, e105, e106, e107, e108, e109, e110, e111,
    e112, e113, e114, e115, e116, e117, e118, e119, e120, e121, e122, e123, e124, e125, e126, e127,
    e128, e129, e130, e131, e132, e133, e134, e135, e136, e137, e138, e139, e140, e141, e142, e143,
    e144, e145, e146, e147, e148, e149, e150, e151, e152, e153, e154, e155, e156, e157, e158, e159,
    e160, e161, e162, e163, e164, e165, e166, e167, e168, e169, e170, e171, e172, e173, e174, e175,
    e176, e177, e178, e179, e180, e181, e182, e183, e184, e185, e186, e187, e188, e189, e190, e191,
    e192, e193, e194, e195, e196, e197, e198, e199, e200, e201, e202, e203, e204, e205, e206, e207,
    e208, e209, e210, e211, e212, e213, e214, e215, e216, e217, e218, e219, e220, e221, e222, e223,
    e224, e225, e226, e227, e228, e229, e230, e231, e232, e233, e234, e235, e236, e237, e238, e239,
    e240, e241, e242, e243, e244, e245, e246, e247, e248, e249, e250, e251, e252, e253, e254, e255,
    e256, e257, e258, e259, e260, e261, e262, e263, e264, e265, e266, e267, e268, e269, e270, e271,
    e272, e273, e274, e275, e276, e277, e278, e279, e280, e281, e282, e283, e284, e285, e286, e287,
    e288, e289, e290, e291, e292, e293, e294, e295, e296, e297, e298, e299, e300, e301, e302, e303,
    e304, e305, e306, e307, e308, e309, e310, e311, e312, e313, e314, e315, e316, e317, e318, e319,
    e320, e321, e322, e323, e324, e325, e326, e327, e328, e329, e330, e331, e332, e333, e334, e335,
    e336, e337, e338, e339, e340, e341, e342, e343, e344, e345, e346, e347, e348, e349, e350, e351,
    e352, e353, e354, e355, e356, e357, e358, e359, e360, e361, e362, e363, e364, e365, e366, e367,
    e368, e369, e370, e371, e372, e373, e374, e375, e376, e377, e378, e379, e380, e381, e382, e383,
    e384, e385, e386, e387, e388, e389, e390, e391, e392, e393, e394, e395, e396, e397, e398, e399,
    e400, e401, e402, e403, e404, e405, e406, e407, e408, e409, e410, e411, e412, e413, e414, e415,
    e416, e417, e418, e419, e420, e421, e422, e423, e424, e425, e426, e427, e428, e429, e430, e431,
    e432, e433, e434, e435, e436, e437, e438, e439, e440, e441, e442, e443, e444, e445, e446, e447,
    e448, e449, e450, e451, e452, e453, e454, e455, e456, e457, e458, e459, e460, e461, e462, e463,
    e464, e465, e466, e467, e468, e469, e470, e471, e472, e473, e474, e475, e476, e477, e478, e479,
    e480, e481, e482, e483, e484, e485, e486, e487, e488, e489, e490, e491, e492, e493, e494, e495,
    e496, e497, e498, e499, e500, e501, e502, e503, e504, e505, e506, e507, e508, e509, e510, e511,
    e512, e513, e514, e515, e516, e517, e518, e519, e520, e521, e522, e523, e524, e525, e526, e527,
    e528, e529, e530, e531, e532, e533, e534, e535, e536, e537, e538, e539, e540, e541, e542, e543,
    e544, e545, e546, e547, e548, e549, e550, e551, e552, e553, e554, e555, e556, e557, e558, e559,
    e560, e561, e562, e563, e564, e565, e566, e567, e568, e569, e570, e571, e572, e573, e574, e575,
    e576, e577, e578, e579, e580, e581, e582, e583, e584, e585, e586, e587, e588, e589, e590, e591,
    e592, e593, e594, e595, e596, e597, e598, e599, e600, e601, e602, e603, e604, e605, e606, e607,
    e608, e609, e610, e611, e612, e613, e614, e615, e616, e617, e618, e619, e620, e621, e622, e623,
    e624, e625, e626, e627, e628, e629, e630, e631, e632, e633, e634, e635, e636, e637, e638, e639,
    e640, e641, e642, e643, e644, e645, e646, e647, e648, e649, e650, e651, e652, e653, e654, e655,
    e656, e657, e658, e659, e660, e661, e662, e663, e664, e665, e666, e667, e668, e669, e670, e671,
    e672, e673, e674, e675, e676, e677, e678, e679, e680, e681, e682, e683, e684, e685, e686, e687,
    e688, e689, e690, e691, e692, e693, e694, e695, e696, e697, e698, e699, e700, e701, e702, e703,
    e704, e705, e706, e707, e708, e709, e710, e711, e712, e713, e714, e715, e716, e717, e718, e719,
    e720, e721, e722, e723, e724, e725, e726, e727, e728, e729, e730, e731, e732, e733, e734, e735,
    e736, e737, e738, e739, e740, e741, e742, e743, e744, e745, e746, e747, e748, e749, e750, e751,
    e752, e753, e754, e755, e756, e757, e758, e759, e760, e761, e762, e763, e764, e765, e766, e767,
    e768, e769, e770, e771, e772, e773, e774, e775, e776, e777, e778, e779, e780, e781, e782, e783,
    e784, e785, e786, e787, e788, e789, e790, e791, e792, e793, e794, e795, e796, e797, e798, e799,
    e800, e801, e802, e803, e804, e805, e806, e807, e808, e809, e810, e811, e812, e813, e814, e815,
    e816, e817, e818, e819, e820, e821, e822, e823, e824, e825, e826, e827, e828, e829, e830, e831,
    e832, e833, e834, e835, e836, e837, e838, e839, e840, e841, e842, e843, e844, e845, e846, e847,
    e848, e849, e850, e851, e852, e853, e854, e855, e856, e857, e858, e859, e860, e861, e862, e863,
    e864, e865, e866, e867, e868, e869, e870, e871, e872, e873, e874, e875, e876, e877, e878, e879,
    e880, e881, e882, e883, e884, e885, e886, e887, e888, e889, e890, e891, e892, e893, e894, e895,
    e896, e897, e898, e899, e900, e901, e902, e903, e904, e905, e906, e907, e908, e909, e910, e911,
    e912, e913, e914, e915, e916, e917, e918, e919, e920, e921, e922, e923, e924, e925, e926, e927,
    e928, e929, e930, e931, e932, e933, e934, e935, e936, e937, e938, e939, e940, e941, e942, e943,
    e944, e945, e946, e947, e948, e949, e950, e951, e952, e953, e954, e955, e956, e957, e958, e959,
    e960, e961, e962, e963, e964, e965, e966, e967, e968, e969, e970, e971, e972, e973, e974, e975,
    e976, e977, e978, e979, e980, e981, e982, e983, e984, e985, e986, e987, e988, e989, e990, e991,
    e992, e993, e994, e995, e996, e997, e998, e999, e1000, e1001, e1002, e1003, e1004, e1005, e1006, e1007,
    e1008, e1009, e1010, e1011, e1012, e1013, e1014, e1015, e1016, e1017, e1018, e1019, e1020, e1021, e1022, e1023,
    e1024, e1025, e1026, e1027, e1028, e1029, e1030, e1031, e1032, e1033, e1034, e1035, e1036, e1037, e1038, e1039,
    e1040, e1041, e1042, e1043, e1044, e1045, e1046, e1047, e1048, e1049, e1050, e1051, e1052, e1053, e1054, e1055,
    e1056, e1057, e1058, e1059, e1060, e1061, e1062, e1063, e1064, e1065, e1066, e1067, e1068, e1069, e1070, e1071
};

consteval auto adapt_enum_info(large_enum_t en)
{
    using enum large_enum_t;
    return adapt_enum_info_helper
    <
        e0, e1, e2, e3, e4, e5, e6, e7, e8, e9, e10, e11, e12, e13, e14, e15,
        e16, e17, e18, e19, e20, e21, e22, e23, e24, e25, e26, e27, e28, e29, e30, e31,
        e32, e33, e34, e35, e36, e37, e38, e39, e40, e41, e42, e43, e44, e45, e46, e47,
        e48, e49, e50, e51, e52, e53, e54, e55, e56, e57, e58, e59, e60, e61, e62, e63,
        e64, e65, e66, e67, e68, e69, e70, e71, e72, e73, e74, e75, e76, e77, e78, e79,
        e80, e81, e82, e83, e84, e85, e86, e87, e88, e89, e90, e91, e92, e93, e94, e95,
        e96, e97, e98, e99, e100, e101, e102, e103, e104, e105, e106, e107, e108, e109, e110, e111,
        e112, e113, e114, e115, e116, e117, e118, e119, e120, e121, e122, e123, e124, e125, e126, e127,
        e128, e129, e130, e131, e132, e133, e134, e135, e136, e137, e138, e139, e140, e141, e142, e143,
        e144, e145, e146, e147, e148, e149, e150, e151, e152, e153, e154, e155, e156, e157, e158, e159,
        e160, e161, e162, e163, e164, e165, e166, e167, e168, e169, e170, e171, e172, e173, e174, e175,
        e176, e177, e178, e179, e180, e181, e182, e183, e184, e185, e186, e187, e188, e189, e190, e191,
        e192, e193, e194, e195, e196, e197, e198, e199, e200, e201, e202, e203, e204, e205, e206, e207,
        e208, e209, e210, e211, e212, e213, e214, e215, e216, e217, e218, e219, e220, e221, e222, e223,
        e224, e225, e226, e227, e228, e229, e230, e231, e232, e233, e234, e235, e236, e237, e238, e239,
        e240, e241, e242, e243, e244, e245, e246, e347, e248, e249, e250, e251, e252, e253, e254, e255,
        e256, e257, e258, e259, e260, e261, e262, e263, e264, e265, e266, e267, e268, e269, e270, e271,
        e272, e273, e274, e275, e276, e277, e278, e279, e280, e281, e282, e283, e284, e285, e286, e287,
        e288, e289, e290, e291, e292, e293, e294, e295, e296, e297, e298, e299, e300, e301, e302, e303,
        e304, e305, e306, e307, e308, e309, e310, e311, e312, e313, e314, e315, e316, e317, e318, e319,
        e320, e321, e322, e323, e324, e325, e326, e327, e328, e339, e330, e331, e332, e333, e334, e335,
        e336, e337, e338, e339, e340, e341, e342, e343, e344, e345, e346, e347, e348, e349, e350, e351,
        e352, e353, e354, e355, e356, e357, e358, e359, e360, e361, e362, e363, e364, e365, e366, e367,
        e368, e369, e370, e371, e372, e373, e374, e375, e376, e377, e378, e379, e380, e381, e382, e383,
        e384, e385, e386, e387, e388, e389, e390, e391, e392, e393, e394, e395, e396, e397, e398, e399,
        e400, e401, e402, e403, e404, e405, e406, e407, e408, e409, e410, e411, e412, e413, e414, e415,
        e416, e417, e418, e419, e420, e421, e422, e423, e424, e425, e426, e427, e428, e429, e430, e431,
        e432, e433, e434, e435, e436, e437, e438, e439, e440, e441, e442, e443, e444, e445, e446, e447,
        e448, e449, e450, e451, e452, e453, e454, e455, e456, e457, e458, e459, e460, e461, e462, e463,
        e464, e465, e466, e467, e468, e469, e470, e471, e472, e473, e474, e475, e476, e477, e478, e479,
        e480, e481, e482, e483, e484, e485, e486, e487, e488, e489, e490, e491, e492, e493, e494, e495,
        e496, e497, e498, e499, e500, e501, e502, e503, e504, e505, e506, e507, e508, e509, e510, e511,
        e512, e513, e514, e515, e516, e517, e518, e519, e520, e521, e522, e523, e524, e525, e526, e527,
        e528, e529, e530, e531, e532, e533, e534, e535, e536, e537, e538, e539, e540, e541, e542, e543,
        e544, e545, e546, e547, e548, e549, e550, e551, e552, e553, e554, e555, e556, e557, e558, e559,
        e560, e561, e562, e563, e564, e565, e566, e567, e568, e569, e570, e571, e572, e573, e574, e575,
        e576, e577, e578, e579, e580, e581, e582, e583, e584, e585, e586, e587, e588, e589, e590, e591,
        e592, e593, e594, e595, e596, e597, e598, e599, e600, e601, e602, e603, e604, e605, e606, e607,
        e608, e609, e610, e611, e612, e613, e614, e615, e616, e617, e618, e619, e620, e621, e622, e623,
        e624, e625, e626, e627, e628, e629, e630, e631, e632, e633, e634, e635, e636, e637, e638, e639,
        e640, e641, e642, e643, e644, e645, e646, e647, e648, e649, e650, e651, e652, e653, e654, e655,
        e656, e657, e658, e659, e660, e661, e662, e663, e664, e665, e666, e667, e668, e669, e670, e671,
        e672, e673, e674, e675, e676, e677, e678, e679, e680, e681, e682, e683, e684, e685, e686, e687,
        e688, e689, e690, e691, e692, e693, e694, e695, e696, e697, e698, e699, e700, e701, e702, e703,
        e704, e705, e706, e707, e708, e709, e710, e711, e712, e713, e714, e715, e716, e717, e718, e719,
        e720, e721, e722, e723, e724, e725, e726, e727, e728, e729, e730, e731, e732, e733, e734, e735,
        e736, e737, e738, e739, e740, e741, e742, e743, e744, e745, e746, e747, e748, e749, e750, e751,
        e752, e753, e754, e755, e756, e757, e758, e759, e760, e761, e762, e763, e764, e765, e766, e767,
        e768, e769, e770, e771, e772, e773, e774, e775, e776, e777, e778, e779, e780, e781, e782, e783,
        e784, e785, e786, e787, e788, e789, e790, e791, e792, e793, e794, e795, e796, e797, e798, e799,
        e800, e801, e802, e803, e804, e805, e806, e807, e808, e809, e810, e811, e812, e813, e814, e815,
        e816, e817, e818, e819, e820, e821, e822, e823, e824, e825, e826, e827, e828, e829, e830, e831,
        e832, e833, e834, e835, e836, e837, e838, e839, e840, e841, e842, e843, e844, e845, e846, e847,
        e848, e849, e850, e851, e852, e853, e854, e855, e856, e857, e858, e859, e860, e861, e862, e863,
        e864, e865, e866, e867, e868, e869, e870, e871, e872, e873, e874, e875, e876, e877, e878, e879,
        e880, e881, e882, e883, e884, e885, e886, e887, e888, e889, e890, e891, e892, e893, e894, e895,
        e896, e897, e898, e899, e900, e901, e902, e903, e904, e905, e906, e907, e908, e909, e910, e911,
        e912, e913, e914, e915, e916, e917, e918, e919, e920, e921, e922, e923, e924, e925, e926, e927,
        e928, e929, e930, e931, e932, e933, e934, e935, e936, e937, e938, e939, e940, e941, e942, e943,
        e944, e945, e946, e947, e948, e949, e950, e951, e952, e953, e954, e955, e956, e957, e958, e959,
        e960, e961, e962, e963, e964, e965, e966, e967, e968, e969, e970, e971, e972, e973, e974, e975,
        e976, e977, e978, e979, e980, e981, e982, e983, e984, e985, e986, e987, e988, e989, e990, e991,
        e992, e993, e994, e995, e996, e997, e998, e999, e1000, e1001, e1002, e1003, e1004, e1005, e1006, e1007,
        e1008, e1009, e1010, e1011, e1012, e1013, e1014, e1015, e1016, e1017, e1018, e1019, e1020, e1021, e1022, e1023,
        e1024, e1025, e1026, e1027, e1028, e1029, e1030, e1031, e1032, e1033, e1034, e1035, e1036, e1037, e1038, e1039,
        e1040, e1041, e1042, e1043, e1044, e1045, e1046, e1047, e1048, e1049, e1050, e1051, e1052, e1053, e1054, e1055,
        e1056, e1057, e1058, e1059, e1060, e1061, e1062, e1063, e1064, e1065, e1066, e1067, e1068, e1069, e1070, e1071
    >();
}

CMN_ENUM_INJECT_OPS()

// only 256 enum items supported through preprocessor

//CMN_ADAPT_ENUM_CLASS
//(
//    large_enum_t,
//    (e0 ) (e1  ) (e2  ) (e3  ) (e4  ) (e5  ) (e6  ) (e7  ) (e8  ) (e9  ) (e10 ) (e11 ) (e12 ) (e13 ) (e14 ) (e15 )
//    (e16) (e17 ) (e18 ) (e19 ) (e20 ) (e21 ) (e22 ) (e23 ) (e24 ) (e25 ) (e26 ) (e27 ) (e28 ) (e29 ) (e30 ) (e31 )
//    (e32) (e33 ) (e34 ) (e35 ) (e36 ) (e37 ) (e38 ) (e39 ) (e40 ) (e41 ) (e42 ) (e43 ) (e44 ) (e45 ) (e46 ) (e47 )
//    (e48) (e49 ) (e50 ) (e51 ) (e52 ) (e53 ) (e54 ) (e55 ) (e56 ) (e57 ) (e58 ) (e59 ) (e60 ) (e61 ) (e62 ) (e63 )
//    (e64) (e65 ) (e66 ) (e67 ) (e68 ) (e69 ) (e70 ) (e71 ) (e72 ) (e73 ) (e74 ) (e75 ) (e76 ) (e77 ) (e78 ) (e79 )
//    (e80) (e81 ) (e82 ) (e83 ) (e84 ) (e85 ) (e86 ) (e87 ) (e88 ) (e89 ) (e90 ) (e91 ) (e92 ) (e93 ) (e94 ) (e95 )
//    (e96) (e97 ) (e98 ) (e99 ) (e100) (e101) (e102) (e103) (e104) (e105) (e106) (e107) (e108) (e109) (e110) (e111)
//    (e11) (e113) (e114) (e115) (e116) (e117) (e118) (e119) (e120) (e121) (e122) (e123) (e124) (e125) (e126) (e127)
//    (e12) (e129) (e130) (e131) (e132) (e133) (e134) (e135) (e136) (e137) (e138) (e139) (e140) (e141) (e142) (e143)
//    (e14) (e145) (e146) (e147) (e148) (e149) (e150) (e151) (e152) (e153) (e154) (e155) (e156) (e157) (e158) (e159)
//    (e16) (e161) (e162) (e163) (e164) (e165) (e166) (e167) (e168) (e169) (e170) (e171) (e172) (e173) (e174) (e175)
//    (e17) (e177) (e178) (e179) (e180) (e181) (e182) (e183) (e184) (e185) (e186) (e187) (e188) (e189) (e190) (e191)
//    (e19) (e193) (e194) (e195) (e196) (e197) (e198) (e199) (e200) (e201) (e202) (e203) (e204) (e205) (e206) (e207)
//    (e20) (e209) (e210) (e211) (e212) (e213) (e214) (e215) (e216) (e217) (e218) (e219) (e220) (e221) (e222) (e223)
//    (e22) (e225) (e226) (e227) (e228) (e229) (e230) (e231) (e232) (e233) (e234) (e235) (e236) (e237) (e238) (e239)
//    (e24) (e241) (e242) (e243) (e244) (e245) (e246) (e347) (e248) (e249) (e250) (e251) (e252) (e253) (e254) (e255)
//    //(e25) (e257) (e258) (e259) (e260) (e261) (e262) (e263) (e264) (e265) (e266) (e267) (e268) (e269) (e270) (e271)
//    //(e27) (e273) (e274) (e275) (e276) (e277) (e278) (e279) (e280) (e281) (e282) (e283) (e284) (e285) (e286) (e287)
//    //(e28) (e289) (e290) (e291) (e292) (e293) (e294) (e295) (e296) (e297) (e298) (e299) (e300) (e301) (e302) (e303)
//    //(e30) (e305) (e306) (e307) (e308) (e309) (e310) (e311) (e312) (e313) (e314) (e315) (e316) (e317) (e318) (e319)
//    //(e32) (e321) (e322) (e323) (e324) (e325) (e326) (e327) (e328) (e339) (e330) (e331) (e332) (e333) (e334) (e335)
//    //(e33) (e337) (e338) (e339) (e340) (e341) (e342) (e343) (e344) (e345) (e346) (e347) (e348) (e349) (e350) (e351)
//    //(e35) (e353) (e354) (e355) (e356) (e357) (e358) (e359) (e360) (e361) (e362) (e363) (e364) (e365) (e366) (e367)
//    //(e36) (e369) (e370) (e371) (e372) (e373) (e374) (e375) (e376) (e377) (e378) (e379) (e380) (e381) (e382) (e383)
//    //(e38) (e385) (e386) (e387) (e388) (e389) (e390) (e391) (e392) (e393) (e394) (e395) (e396) (e397) (e398) (e399)
//    //(e40) (e401) (e402) (e403) (e404) (e405) (e406) (e407) (e408) (e409) (e410) (e411) (e412) (e413) (e414) (e415)
//    //(e41) (e417) (e418) (e419) (e420) (e421) (e422) (e423) (e424) (e425) (e426) (e427) (e428) (e429) (e430) (e431)
//    //(e43) (e433) (e434) (e435) (e436) (e437) (e438) (e439) (e440) (e441) (e442) (e443) (e444) (e445) (e446) (e447)
//    //(e44) (e449) (e450) (e451) (e452) (e453) (e454) (e455) (e456) (e457) (e458) (e459) (e460) (e461) (e462) (e463)
//    //(e46) (e465) (e466) (e467) (e468) (e469) (e470) (e471) (e472) (e473) (e474) (e475) (e476) (e477) (e478) (e479)
//    //(e48) (e481) (e482) (e483) (e484) (e485) (e486) (e487) (e488) (e489) (e490) (e491) (e492) (e493) (e494) (e495)
//    //(e49) (e497) (e498) (e499) (e500) (e501) (e502) (e503) (e504) (e505) (e506) (e507) (e508) (e509) (e510) (e511)
//)

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

//BOOST_AUTO_TEST_CASE(local_enum)
//{
//    enum class loc_en_t { e0, e1, e2 };
//
//    struct adapted
//    {
//        consteval auto operator () (loc_en_t) const
//        {
//            using enum loc_en_t;
//            return adapt_enum_info_helper<e0, e1, e2>();
//        };
//    };
//
//    //adapt_enum_info(loc_en_t::e0);
//    //static_assert(c::adapted_enum<loc_en_t>);
//}

enum { ae_0, ae_1, ae_2 };

consteval auto adapt_enum_info(decltype(ae_0))
{
    return adapt_enum_info_helper<ae_0, ae_1, ae_2>();
}

BOOST_AUTO_TEST_CASE(anonymous_enum)
{
    output_test_stream tstr;
    tstr << ae_1;
    BOOST_CHECK(tstr.is_equal("[ae_1]"));

    // NOTE: what about using print_t::class_prefix option?
}

BOOST_AUTO_TEST_SUITE_END() // enum_
BOOST_AUTO_TEST_SUITE_END() // cmn
