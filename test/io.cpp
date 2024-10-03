
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>

#include <boost/test/unit_test.hpp>

#include <cmn/meta/concepts.h>
#include <cmn/util/strong_typedef.h>
#include <cmn/util/strong_typedef_io.h>
#include <boost/test/tools/output_test_stream.hpp>

BOOST_AUTO_TEST_SUITE(cmn)
BOOST_AUTO_TEST_SUITE(io)

// strong_typedef
using test_0 = strong_typedef<int, struct test_0_>;
using test_1 = strong_typedef<int, struct test_1_>;

static_assert(!std::is_convertible_v<test_0, test_1>, "Strong typedef test 0 failed");

using my_int = strong_typedef<int, struct mmy_int_>;

template <>
struct strong_typedef_fmt_traits<my_int>
{
    static constexpr bool enable_luxury_io = true;        
    static constexpr auto default_ = int_fmt_t::default_;
};

BOOST_AUTO_TEST_CASE(int_fmt_)
{
    boost::test_tools::output_test_stream ostr;

    //BOOST_TEST_MESSAGE(int_fmt_slot_manip::value(ostr));
    {
        boost::io::ios_iword_saver _{ ostr, manip::int_fmt_slot_manip::index() };
        ostr << uhex << ushowbase << uupercase << uasm << ulong_ << usign << my_int{ 6789 };
        //BOOST_TEST_MESSAGE(int_fmt_slot_manip::value(ostr));
        //BOOST_TEST_MESSAGE(ostr.str());
        BOOST_TEST(ostr.is_equal("+00001A85h"));
    }

    ostr.str("");
    BOOST_TEST(manip::int_fmt_slot_manip::value(ostr) == int_fmt_t::empty);
    {
        boost::io::ios_iword_saver _{ ostr, manip::int_fmt_slot_manip::index() };
        ostr << my_int{ 6789 };
        //BOOST_TEST_MESSAGE(int_fmt_slot_manip::value(ostr));
        //BOOST_TEST_MESSAGE(ostr.str());        
        BOOST_TEST(ostr.is_equal("6789"));
    }
}

using my_int_2 = strong_typedef<int, struct my_int_2_>;
using my_int_3 = strong_typedef<int, struct my_int_3_>;

template <>
struct strong_typedef_fmt_traits<my_int_2>
{
    static constexpr bool enable_luxury_io = true;        
    static constexpr auto default_ = int_fmt_t::sshort_asm_up_hex;    
};

template <>
struct strong_typedef_fmt_traits<my_int_3>
{
    static constexpr bool enable_luxury_io = true;        
    static constexpr auto default_ = int_fmt_t::hex | int_fmt_t::showbase | 
        int_fmt_t::c | int_fmt_t::long_ | int_fmt_t::lowercase | int_fmt_t::nosign;
};

BOOST_AUTO_TEST_CASE(trait_int_fmt)
{
    boost::test_tools::output_test_stream ostr;

    //BOOST_TEST_MESSAGE(int_fmt_slot_manip::value(ostr));
    ostr << my_int_2{ 0x16AF };
    BOOST_TEST(ostr.is_equal("+16AFh"));

    ostr.str("");
    //BOOST_TEST_MESSAGE(int_fmt_slot_manip::value(ostr));
    ostr << my_int_3{ 0x16AF };
    BOOST_TEST(ostr.is_equal("0x000016af"));
}

BOOST_AUTO_TEST_CASE(xml_serialize_int_fmt)
{
    {
        std::stringstream sstr;
        my_int_2 const in{ 0x1234 };
        {
            boost::archive::xml_oarchive oa{ sstr };
            oa << in;
        }

        my_int_2 out;
        {
            boost::archive::xml_iarchive ia{ sstr };
            ia >> out;
        }
        BOOST_TEST(in == out);        
        //BOOST_TEST_MESSAGE(out);
    }
    {
        std::stringstream sstr;
        my_int_3 const in{ 0x1234 };
        {
            boost::archive::xml_oarchive oa{ sstr };
            oa << in;
        }

        my_int_3 out;
        {
            boost::archive::xml_iarchive ia{ sstr };
            ia >> out;
        }
        BOOST_TEST(in == out);        
        //BOOST_TEST_MESSAGE(out);
    }
}

BOOST_AUTO_TEST_SUITE_END() // util
BOOST_AUTO_TEST_SUITE_END() // cmn

