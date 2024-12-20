
#include <boost/test/unit_test.hpp>
#include <boost/test/tools/output_test_stream.hpp>

#include <cmn/util/fixed_string.h>

#include <cmn/io/manip/slot/fwd.h>
#include <cmn/io/manip/slot/forwarder.h>
#include <cmn/io/manip/slot/manip.h>

#include "test_accessor.h"

BOOST_AUTO_TEST_SUITE(cmn)
BOOST_AUTO_TEST_SUITE(io)
BOOST_AUTO_TEST_SUITE(manip)

using namespace std::string_literals;

BOOST_AUTO_TEST_CASE(decoder)
{
    using manip_type = basic_string_slot_manip<struct storage_, "1"_fs>;
    using ta = test_accessor_t;

    using keep_type = int_keep_type;
    using chars_type = char[sizeof(keep_type) / sizeof(char)];
    // slot emulator
    union 
    {
        chars_type  encode_;
        keep_type   decode_;
    };
    BOOST_TEST(reinterpret_cast<void *>(&decode_) == reinterpret_cast<void *>(&encode_));

    decode_ = {};  // clear slot
    constexpr auto a = "a";
    std::strncpy(encode_, a,  std::strlen(a) + 1);
    BOOST_TEST(ta::decode<manip_type>(encode_) == decode_);
    BOOST_TEST(ta::encode<manip_type>(decode_) == encode_);

    BOOST_TEST(ta::decode<manip_type>("1"s) == 0);
    BOOST_TEST(ta::encode<manip_type>(0) == "1"s);
}

using imanip = int_slot_manip
<                   
      struct int_manip_     // TagOrStorage                 
    , 4                     // DefaultInit_
    , 7                     // Default_
>;

inline constexpr slot_manip_forwarder<imanip> int_ {};

BOOST_AUTO_TEST_CASE(int_manip)
{
    boost::test_tools::output_test_stream ostr {};
    BOOST_TEST(imanip::value(ostr) == 4);

    ostr << int_(6);
    BOOST_TEST(imanip::value(ostr) == 6);

    ostr << int_();
    BOOST_TEST(imanip::value(ostr) == 7);
}

using bmanip = int_slot_manip
<
      struct bool_manip_    // TagOrStorage
    , false                 // DefaultInit_
    , true                  // Default_
>;
constexpr slot_manip_forwarder<bmanip> bool_ {};

BOOST_AUTO_TEST_CASE(bool_manip)
{
    boost::test_tools::output_test_stream ostr {};
    BOOST_TEST(!bmanip::value(ostr));

    ostr << bool_();
    BOOST_TEST(bmanip::value(ostr));

    ostr << bool_(false);
    BOOST_TEST(!bmanip::value(ostr));

    ostr << bool_(true);
    BOOST_TEST(bmanip::value(ostr));
}

using ls_manip = basic_large_string_slot_manip
<
      struct large_string_manip_
    , "default init"_fs
    , "default manip"_fs
>;
constexpr slot_manip_forwarder<ls_manip> string_ {};

BOOST_AUTO_TEST_CASE(large_string_manip)
{
    boost::test_tools::output_test_stream ostr {};
    BOOST_TEST(ls_manip::value(ostr) == "default init");

    ostr << string_();
    BOOST_TEST(ls_manip::value(ostr) == "default manip");

    static_assert(c::decoded_by<decltype("my str"), ls_manip::decoder_type>);
    ostr << string_("my str");
    BOOST_TEST(ls_manip::value(ostr) == "my str");
}

BOOST_AUTO_TEST_SUITE_END() // manip
BOOST_AUTO_TEST_SUITE_END() // io
BOOST_AUTO_TEST_SUITE_END() // cmn
