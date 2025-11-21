
#include <format>
#include <string_view>
#include <fstream>

#include <boost/io/ios_state.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>

#include <boost/test/unit_test.hpp>

#include <cmn/meta/concepts.h>
#include <cmn/strong_typedef.h>
#include <cmn/strong_typedef/io.h>
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

BOOST_AUTO_TEST_CASE(int_fmt_manip)
{
    using enum int_fmt_t;

    auto const val = c | hex | showbase;

    std::stringstream sstr;
    sstr << int_fmt(val);
    BOOST_TEST(val == manip::int_fmt_slot_manip::value(sstr));
}

BOOST_AUTO_TEST_CASE(int_fmt_)
{
    boost::test_tools::output_test_stream ostr;

    using enum int_fmt_t;

    //BOOST_TEST_MESSAGE(manip::int_fmt_slot_manip::value(ostr));
    {
        boost::io::ios_iword_saver _{ ostr, manip::int_fmt_slot_manip::index(ostr) };

        ostr << uhex << ushowbase << uupercase << uasm << ulong_ << usign << my_int{ 6789 };
        //BOOST_TEST_MESSAGE(manip::int_fmt_slot_manip::value(ostr));
        //BOOST_TEST_MESSAGE(ostr.str());
        BOOST_TEST(ostr.is_equal("+00001A85h"));
    }

    ostr.str("");
    BOOST_TEST(manip::int_fmt_slot_manip::value(ostr) == int_fmt_t::empty);
    {
        boost::io::ios_iword_saver _{ ostr, manip::int_fmt_slot_manip::index(ostr) };
        ostr << my_int{ 6789 };
        //BOOST_TEST_MESSAGE(manip::int_fmt_slot_manip::value(ostr));
        //BOOST_TEST_MESSAGE(ostr.str());        
        BOOST_TEST(ostr.is_equal("6789"));
    }
}

using my_int_2 = strong_typedef<int, struct my_int_2_>;

template <>
struct strong_typedef_fmt_traits<my_int_2>
{
    static constexpr bool enable_luxury_io = true;        
    static constexpr auto default_ = int_fmt_t::sshort_asm_up_hex;    
};

BOOST_AUTO_TEST_CASE(trait_int_2_fmt)
{
    std::stringstream sstr;
    sstr << my_int_2{0x16AF};
    BOOST_TEST(sstr.str() == "+16AFh");

    my_int_2 dst;
    sstr >> dst;
    BOOST_TEST(dst == 0x16AF);
    BOOST_CHECK_MESSAGE(sstr.eof(), "Partial reading");
}

using my_int_3 = strong_typedef<int, struct my_int_3_>;

template <>
struct strong_typedef_fmt_traits<my_int_3>
{
    static constexpr bool enable_luxury_io = true;
    static constexpr auto default_ = int_fmt_t::hex | int_fmt_t::showbase | int_fmt_t::c | int_fmt_t::long_ |
        int_fmt_t::lowercase | int_fmt_t::nosign;
};

BOOST_AUTO_TEST_CASE(trait_int_3_fmt)
{
    std::stringstream sstr;

    sstr << my_int_3{0x16AF};
    BOOST_TEST(sstr.str()  == "0x000016af");

    my_int_3 dst;
    sstr >> dst;
    BOOST_TEST(dst == 0x000016AF);
    BOOST_CHECK_MESSAGE(sstr.eof(), "Partial reading");
}


BOOST_AUTO_TEST_CASE(xml_serialize_int_fmt)
{
    // TODO:
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
    //{
    //    std::stringstream sstr;
    //    my_int_3 const in{ 0x1234 };
    //    {
    //        boost::archive::xml_oarchive oa{ sstr };
    //        oa << in;
    //    }
    //    BOOST_TEST_MESSAGE(sstr.str());

    //    //{
    //    //    std::fstream fstr{};
    //    //    fstr.exceptions(std::ios_base::failbit | std::ios_base::badbit);
    //    //    fstr.open("c:\\tmp\\tmp.xml", std::ios_base::in | std::ios_base::out | std::ios_base::trunc);
    //    //    fstr << sstr.rdbuf();
    //    //}

    //    my_int_3 out;
    //    {
    //        boost::archive::xml_iarchive ia{ sstr };
    //        ia >> out;
    //    }
    //    BOOST_TEST(in == out);        
    //    //BOOST_TEST_MESSAGE(out);
    //}
}

using my_ptr = strong_typedef_ptr<unsigned, struct my_strong_typedef_ptr_>;
static_assert(c::ptr_ariphmetic<my_ptr>);

using my_bitfield = strong_typedef_flag<unsigned int, struct my_strong_bitfield_>;
static_assert(c::bitfield<my_bitfield>);

BOOST_AUTO_TEST_CASE(strong_typedef_format)
{
    using namespace std::string_view_literals;
    using enum int_fmt_t;

    std::basic_format_parse_context ctx{ ":#x}"sv, 3 };
    std::formatter<my_int> fmt;
    fmt.parse(ctx);
    BOOST_TEST(fmt.m_fmt_opt == (hex | showbase | c | short_ | lowercase | nosign));

    BOOST_TEST(std::format("{}", my_int{ 4 }) == "4");
    BOOST_TEST(std::format("{:#x}", my_int{ 4 }) == "0x4");
    BOOST_TEST(std::format("{:#xu}", my_int{ 0x4A }) == "0X4A");
}

BOOST_AUTO_TEST_SUITE_END() // io
BOOST_AUTO_TEST_SUITE_END() // cmn

