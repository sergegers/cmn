
#include <array>
#include <format>

#include <boost/lexical_cast.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/tools/output_test_stream.hpp>

#include <cmn/meta/concepts.h>
#include <cmn/error/exception.h>
#include <cmn/util/lexical_cast.h>

#include <cmn/enum/traits.h>
#include <cmn/enum/op.h>
#include <cmn/enum/enum.h>
#include <cmn/enum/bitfield.h>
#include <cmn/enum/combo.h>
#include <cmn/enum/io.h>

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


BOOST_AUTO_TEST_SUITE(cmn)
BOOST_AUTO_TEST_SUITE(enum_)

static_assert(kind_v< ::drc_t> == kind_t::enum_);
static_assert(ops_v< ::drc_t> == op_io);

static_assert(next_on_mask(0b0111) == 0b1000);
static_assert(next_on_mask(0b0111) != 0b1001);
static_assert(next_on_mask(0b0100000100) == 0b1000000000);

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

static_assert(std::same_as<interop_type_t<cl_cmb_t>, int>);
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

BOOST_AUTO_TEST_CASE(adapt_global_ns)
{
    std::ostringstream ostr;
    ostr << ::DRC_NOCHG;
    BOOST_TEST(ostr.str() == "[DRC_NOCHG]");
}

BOOST_AUTO_TEST_SUITE_END() // enum_
BOOST_AUTO_TEST_SUITE_END() // cmn
