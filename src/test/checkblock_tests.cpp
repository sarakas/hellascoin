


#include <algorithm>


#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/foreach.hpp>

#include "main.h"
#include "wallet.h"
#include "net.h"
#include "util.h"

BOOST_AUTO_TEST_SUITE(CheckBlock_tests)

bool
read_block(const std::string& filename, CBlock& block)
{
    namespace fs = boost::filesystem;
    fs::path testFile = fs::current_path() / "test" / "data" / filename;
#ifdef TEST_DATA_DIR
    if (!fs::exists(testFile))
    {
        testFile = fs::path(BOOST_PP_STRINGIZE(TEST_DATA_DIR)) / filename;
    }
#endif
    FILE* fp = fopen(testFile.string().c_str(), "rb");
    if (!fp) return false;



    CAutoFile filein = CAutoFile(fp, SER_DISK, CLIENT_VERSION);
    if (!filein) return false;

    filein >> block;

    return true;
}

BOOST_AUTO_TEST_CASE(May15)
{




    unsigned int tMay15 = 1368576000;


    CBlock forkingBlock;
    if (read_block("Mar12Fork.dat", forkingBlock))
    {
        CValidationState state;

        BOOST_CHECK(!forkingBlock.CheckBlock(state, false, false));



        BOOST_CHECK(forkingBlock.CheckBlock(state, false, false));
    }

    SetMockTime(0);
}

BOOST_AUTO_TEST_SUITE_END()
