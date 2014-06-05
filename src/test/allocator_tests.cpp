#include <boost/test/unit_test.hpp>

#include "init.h"
#include "main.h"
#include "util.h"

BOOST_AUTO_TEST_SUITE(allocator_tests)


static const void *last_lock_addr, *last_unlock_addr;
static size_t last_lock_len, last_unlock_len;
class TestLocker
{
public:
    bool Lock(const void *addr, size_t len)
    {
        last_lock_addr = addr;
        last_lock_len = len;
        return true;
    }
    bool Unlock(const void *addr, size_t len)
    {
        last_unlock_addr = addr;
        last_unlock_len = len;
        return true;
    }
};

BOOST_AUTO_TEST_CASE(test_LockedPageManagerBase)
{
    const size_t test_page_size = 4096;
    LockedPageManagerBase<TestLocker> lpm(test_page_size);
    size_t addr;
    last_lock_addr = last_unlock_addr = 0;
    last_lock_len = last_unlock_len = 0;

    
    addr = 0;
    for(int i=0; i<1000; ++i)
    {
        lpm.LockRange(reinterpret_cast<void*>(addr), 33);
        addr += 33;
    }
    
    addr = test_page_size*100 + 53;
    for(int i=0; i<100; ++i)
    {
        lpm.LockRange(reinterpret_cast<void*>(addr), test_page_size);
        addr += test_page_size;
    }
    
    addr = test_page_size*300;
    for(int i=0; i<100; ++i)
    {
        lpm.LockRange(reinterpret_cast<void*>(addr), test_page_size);
        addr += test_page_size;
    }
    
    lpm.LockRange(reinterpret_cast<void*>(test_page_size*600+1), test_page_size*500);
    BOOST_CHECK(last_lock_addr == reinterpret_cast<void*>(test_page_size*(600+500)));
    
    lpm.LockRange(reinterpret_cast<void*>(test_page_size*1200), test_page_size*500-1);
    BOOST_CHECK(last_lock_addr == reinterpret_cast<void*>(test_page_size*(1200+500-1)));

    BOOST_CHECK(lpm.GetLockedPageCount() == (






    
    addr = 0;
    for(int i=0; i<1000; ++i)
    {
        lpm.UnlockRange(reinterpret_cast<void*>(addr), 33);
        addr += 33;
    }
    addr = test_page_size*100 + 53;
    for(int i=0; i<100; ++i)
    {
        lpm.UnlockRange(reinterpret_cast<void*>(addr), test_page_size);
        addr += test_page_size;
    }
    addr = test_page_size*300;
    for(int i=0; i<100; ++i)
    {
        lpm.UnlockRange(reinterpret_cast<void*>(addr), test_page_size);
        addr += test_page_size;
    }
    lpm.UnlockRange(reinterpret_cast<void*>(test_page_size*600+1), test_page_size*500);
    lpm.UnlockRange(reinterpret_cast<void*>(test_page_size*1200), test_page_size*500-1);

    
    BOOST_CHECK(lpm.GetLockedPageCount() == 0);

    
    addr = 0;
    for(int i=0; i<1000; ++i)
    {
        lpm.LockRange(reinterpret_cast<void*>(addr), 0);
        addr += 1;
    }
    BOOST_CHECK(lpm.GetLockedPageCount() == 0);
    addr = 0;
    for(int i=0; i<1000; ++i)
    {
        lpm.UnlockRange(reinterpret_cast<void*>(addr), 0);
        addr += 1;
    }
    BOOST_CHECK(lpm.GetLockedPageCount() == 0);

}

BOOST_AUTO_TEST_SUITE_END()
