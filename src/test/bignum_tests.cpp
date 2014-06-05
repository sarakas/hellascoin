#include <boost/test/unit_test.hpp>
#include <limits>

#include "bignum.h"
#include "util.h"

BOOST_AUTO_TEST_SUITE(bignum_tests)






#if defined(__GNUC__)


#define NOINLINE __attribute__((noinline))
#elif defined(_MSC_VER)
#define NOINLINE __declspec(noinline)
#else

#warning You should define NOINLINE for your compiler.
#define NOINLINE
#endif
























NOINLINE void mysetint64(CBigNum& num, int64 n)
{
    num.setint64(n);
}



BOOST_AUTO_TEST_CASE(bignum_setint64)
{
    int64 n;

    {
        n = 0;
        CBigNum num(n);
        BOOST_CHECK(num.ToString() == "0");
        num.setulong(0);
        BOOST_CHECK(num.ToString() == "0");
        mysetint64(num, n);
        BOOST_CHECK(num.ToString() == "0");
    }
    {
        n = 1;
        CBigNum num(n);
        BOOST_CHECK(num.ToString() == "1");
        num.setulong(0);
        BOOST_CHECK(num.ToString() == "0");
        mysetint64(num, n);
        BOOST_CHECK(num.ToString() == "1");
    }
    {
        n = -1;
        CBigNum num(n);
        BOOST_CHECK(num.ToString() == "-1");
        num.setulong(0);
        BOOST_CHECK(num.ToString() == "0");
        mysetint64(num, n);
        BOOST_CHECK(num.ToString() == "-1");
    }
    {
        n = 5;
        CBigNum num(n);
        BOOST_CHECK(num.ToString() == "5");
        num.setulong(0);
        BOOST_CHECK(num.ToString() == "0");
        mysetint64(num, n);
        BOOST_CHECK(num.ToString() == "5");
    }
    {
        n = -5;
        CBigNum num(n);
        BOOST_CHECK(num.ToString() == "-5");
        num.setulong(0);
        BOOST_CHECK(num.ToString() == "0");
        mysetint64(num, n);
        BOOST_CHECK(num.ToString() == "-5");
    }
    {
        n = std::numeric_limits<int64>::min();
        CBigNum num(n);
        BOOST_CHECK(num.ToString() == "-9223372036854775808");
        num.setulong(0);
        BOOST_CHECK(num.ToString() == "0");
        mysetint64(num, n);
        BOOST_CHECK(num.ToString() == "-9223372036854775808");
    }
    {
        n = std::numeric_limits<int64>::max();
        CBigNum num(n);
        BOOST_CHECK(num.ToString() == "9223372036854775807");
        num.setulong(0);
        BOOST_CHECK(num.ToString() == "0");
        mysetint64(num, n);
        BOOST_CHECK(num.ToString() == "9223372036854775807");
    }
}


BOOST_AUTO_TEST_CASE(bignum_SetCompact)
{
    CBigNum num;
    num.SetCompact(0);
    BOOST_CHECK_EQUAL(num.GetHex(), "0");
    BOOST_CHECK_EQUAL(num.GetCompact(), 0U);

    num.SetCompact(0x00123456);
    BOOST_CHECK_EQUAL(num.GetHex(), "0");
    BOOST_CHECK_EQUAL(num.GetCompact(), 0U);

    num.SetCompact(0x01123456);
    BOOST_CHECK_EQUAL(num.GetHex(), "12");
    BOOST_CHECK_EQUAL(num.GetCompact(), 0x01120000U);


    num = 0x80;
    BOOST_CHECK_EQUAL(num.GetCompact(), 0x02008000U);

    num.SetCompact(0x01fedcba);
    BOOST_CHECK_EQUAL(num.GetHex(), "-7e");
    BOOST_CHECK_EQUAL(num.GetCompact(), 0x01fe0000U);

    num.SetCompact(0x02123456);
    BOOST_CHECK_EQUAL(num.GetHex(), "1234");
    BOOST_CHECK_EQUAL(num.GetCompact(), 0x02123400U);

    num.SetCompact(0x03123456);
    BOOST_CHECK_EQUAL(num.GetHex(), "123456");
    BOOST_CHECK_EQUAL(num.GetCompact(), 0x03123456U);

    num.SetCompact(0x04123456);
    BOOST_CHECK_EQUAL(num.GetHex(), "12345600");
    BOOST_CHECK_EQUAL(num.GetCompact(), 0x04123456U);

    num.SetCompact(0x04923456);
    BOOST_CHECK_EQUAL(num.GetHex(), "-12345600");
    BOOST_CHECK_EQUAL(num.GetCompact(), 0x04923456U);

    num.SetCompact(0x05009234);
    BOOST_CHECK_EQUAL(num.GetHex(), "92340000");
    BOOST_CHECK_EQUAL(num.GetCompact(), 0x05009234U);

    num.SetCompact(0x20123456);
    BOOST_CHECK_EQUAL(num.GetHex(), "1234560000000000000000000000000000000000000000000000000000000000");
    BOOST_CHECK_EQUAL(num.GetCompact(), 0x20123456U);

    num.SetCompact(0xff123456);
    BOOST_CHECK_EQUAL(num.GetHex(), "123456000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000");
    BOOST_CHECK_EQUAL(num.GetCompact(), 0xff123456U);
}

BOOST_AUTO_TEST_SUITE_END()
