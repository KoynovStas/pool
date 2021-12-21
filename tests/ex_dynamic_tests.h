#ifndef EX_DYNAMIC_TESTS_H
#define EX_DYNAMIC_TESTS_H

#include <utility> //swap

#include "stest.h"
#include "helpers.h"
#include "pool.h"



using namespace pool;



TEST(ex_test_pool_size)
{
    DECLARE_POOL(pool, int, 2, 16, 0)


    TEST_ASSERT(pool.size()     == 0);
    TEST_ASSERT(pool.capacity() == 2);
    TEST_ASSERT(pool.empty()    == true);
    TEST_ASSERT(pool.full()     == false);


    int* i = pool.create();
    TEST_ASSERT(i);
    TEST_ASSERT(pool.size()     == 1);
    TEST_ASSERT(pool.capacity() == 2);
    TEST_ASSERT(pool.empty()    == false);
    TEST_ASSERT(pool.full()     == false);

    int* i2 = pool.create();
    TEST_ASSERT(i2);
    TEST_ASSERT(pool.size()     == 2);
    TEST_ASSERT(pool.capacity() == 2);
    TEST_ASSERT(pool.empty()    == false);
    TEST_ASSERT(pool.full()     == true);

    //create when full (will be add node)
    int* i3 = pool.create();
    TEST_ASSERT(i3);
    TEST_ASSERT(pool.size()     == 3);
    TEST_ASSERT(pool.capacity() >= pool.size());

    auto cur_cap = pool.capacity();


    pool.destroy(i);
    TEST_ASSERT(pool.size()     == 2);
    TEST_ASSERT(pool.capacity() == cur_cap);
    TEST_ASSERT(pool.empty()    == false);
    TEST_ASSERT(pool.full()     == false);

    pool.destroy(i2);
    TEST_ASSERT(pool.size()     == 1);
    TEST_ASSERT(pool.capacity() == cur_cap);
    TEST_ASSERT(pool.empty()    == false);
    TEST_ASSERT(pool.full()     == false);

    pool.destroy(i3);
    TEST_ASSERT(pool.size()     == 0);
    TEST_ASSERT(pool.capacity() == cur_cap);
    TEST_ASSERT(pool.empty()    == true);
    TEST_ASSERT(pool.full()     == false);

    pool.destroy(nullptr);
    TEST_ASSERT(pool.size()     == 0);
    TEST_ASSERT(pool.capacity() == cur_cap);
    TEST_ASSERT(pool.empty()    == true);
    TEST_ASSERT(pool.full()     == false);

    TEST_PASS(nullptr);
}



TEST(ex_test_pool_create)
{
    const size_t N  = 10;
    const size_t N2 = N*2;
    Pool<int, N, 16, 0, IMPL> pool;


    std::array<int*, N2> pint;

    for(size_t i = 0; i < N2; i++)
    {
        pint[i] = pool.create(i);
        TEST_ASSERT(pint[i]     != nullptr);
        TEST_ASSERT(*pint[i]    == (int)i);
        TEST_ASSERT(pool.size() == (i+1));
    }

    for(size_t i = 0; i < N2; i++)
    {
        TEST_ASSERT(*pint[i] == (int)i);
    }

    TEST_ASSERT(pool.size() == N2);


    for(size_t i = 0; i < N2; i++)
    {
        TEST_ASSERT(pool.size() == N2-i);
        pool.destroy(pint[i]);
    }

    TEST_ASSERT(pool.size() == 0);
    TEST_ASSERT(pool.capacity() >= N2);


    TEST_PASS(nullptr);
}



TEST(ex_test_pool_create_except)
{
    const size_t N  = 10;
    const size_t N2 = N*2;
    Pool<int, N, 16, POOL_CREATE_EXCEPTION, IMPL> pool;


    std::array<int*, N2> pint;

    try
    {
        for(size_t i = 0; i < N; i++)
        {
            pint[i] = pool.create(i);
            TEST_ASSERT(pint[i]     != nullptr);
            TEST_ASSERT(*pint[i]    == (int)i);
            TEST_ASSERT(pool.size() == (i+1));
        }
    }
    catch(...)
    {
        TEST_FAIL(nullptr);
    }


    for(size_t i = 0; i < N; i++)
    {
        TEST_ASSERT(*pint[i] == (int)i);
    }

    TEST_ASSERT(pool.size() == N);
    for(size_t i = 0; i < N; i++)
    {
        pool.destroy(pint[i]);
    }


    //auto create nodes
    try
    {
        for(size_t i = 0; i < N2; i++)
        {
            pint[i] = pool.create(i);
            TEST_ASSERT(pint[i]     != nullptr);
            TEST_ASSERT(*pint[i]    == (int)i);
            TEST_ASSERT(pool.size() == (i+1));
        }
    }
    catch(...)
    {
        TEST_FAIL(nullptr);
    }


    for(size_t i = 0; i < N2; i++)
    {
        TEST_ASSERT(pool.size() == N2-i);
        pool.destroy(pint[i]);
    }

    TEST_ASSERT(pool.size() == 0);
    TEST_ASSERT(pool.capacity() >= N2);


    TEST_PASS(nullptr);
}



TEST(ex_test_pool_create_except2) //fixed capacity
{
    const size_t N  = 10;
    const size_t N2 = N*2;
    DECLARE_POOL(pool, int, N, 16, POOL_CREATE_EXCEPTION | POOL_FIXED_CAPACITY);

    std::array<int*, N2> pint;

    TEST_ASSERT(pool.capacity() == N);

    try
    {
        for(size_t i = 0; i < N; i++)
        {
            pint[i] = pool.create(i);
            TEST_ASSERT(pint[i]     != nullptr);
            TEST_ASSERT(*pint[i]    == (int)i);
            TEST_ASSERT(pool.size() == (i+1));
        }
    }
    catch(...)
    {
        TEST_FAIL(nullptr);
    }


    try
    {
        pint[0] = pool.create(123);
    }
    catch(const std::bad_alloc&)
    {
        TEST_PASS(nullptr);
    }
    catch(...)
    {
        TEST_FAIL(nullptr);
    }

    TEST_FAIL(nullptr);
}



TEST(ex_test_pool_reserve)
{
    DECLARE_POOL(pool, int, 2, 16, 0)


    TEST_ASSERT(pool.size()     == 0);
    TEST_ASSERT(pool.capacity() == 2);
    TEST_ASSERT(pool.empty()    == true);
    TEST_ASSERT(pool.full()     == false);

    pool.reserve(2); //no effect
    TEST_ASSERT(pool.size()     == 0);
    TEST_ASSERT(pool.capacity() == 2);
    TEST_ASSERT(pool.empty()    == true);
    TEST_ASSERT(pool.full()     == false);

    pool.reserve(4);
    TEST_ASSERT(pool.size()     == 0);
    TEST_ASSERT(pool.capacity() == 4);
    TEST_ASSERT(pool.empty()    == true);
    TEST_ASSERT(pool.full()     == false);

    int* i = pool.create();
    TEST_ASSERT(i);
    TEST_ASSERT(pool.size()     == 1);
    TEST_ASSERT(pool.capacity() == 4);
    TEST_ASSERT(pool.empty()    == false);
    TEST_ASSERT(pool.full()     == false);

    int* i2 = pool.create();
    TEST_ASSERT(i2);
    TEST_ASSERT(pool.size()     == 2);
    TEST_ASSERT(pool.capacity() == 4);
    TEST_ASSERT(pool.empty()    == false);
    TEST_ASSERT(pool.full()     == false);

    pool.reserve(2); //no effect
    TEST_ASSERT(pool.size()     == 2);
    TEST_ASSERT(pool.capacity() == 4);
    TEST_ASSERT(pool.empty()    == false);
    TEST_ASSERT(pool.full()     == false);

    TEST_PASS(nullptr);
}



TEST(ex_test_pool_shrink_to_fit)
{
    DECLARE_POOL(pool, int, 2, 16, 0)

    TEST_ASSERT(pool.size()     == 0);
    TEST_ASSERT(pool.capacity() == 2);
    TEST_ASSERT(pool.empty()    == true);
    TEST_ASSERT(pool.full()     == false);

    pool.shrink_to_fit(2); //no effect
    TEST_ASSERT(pool.size()     == 0);
    TEST_ASSERT(pool.capacity() == 2);
    TEST_ASSERT(pool.empty()    == true);
    TEST_ASSERT(pool.full()     == false);

    pool.shrink_to_fit(4); //no effect
    TEST_ASSERT(pool.size()     == 0);
    TEST_ASSERT(pool.capacity() == 2);
    TEST_ASSERT(pool.empty()    == true);
    TEST_ASSERT(pool.full()     == false);

    int* i = pool.create();
    TEST_ASSERT(i);
    TEST_ASSERT(pool.size()     == 1);
    TEST_ASSERT(pool.capacity() == 2);
    TEST_ASSERT(pool.empty()    == false);
    TEST_ASSERT(pool.full()     == false);

    int* i2 = pool.create();
    TEST_ASSERT(i2);
    TEST_ASSERT(pool.size()     == 2);
    TEST_ASSERT(pool.capacity() == 2);
    TEST_ASSERT(pool.empty()    == false);
    TEST_ASSERT(pool.full()     == true);

    pool.shrink_to_fit(2); //no effect
    TEST_ASSERT(pool.size()     == 2);
    TEST_ASSERT(pool.capacity() == 2);
    TEST_ASSERT(pool.empty()    == false);
    TEST_ASSERT(pool.full()     == true);

    pool.shrink_to_fit(0); //no effect
    TEST_ASSERT(pool.size()     == 2);
    TEST_ASSERT(pool.capacity() == 2);
    TEST_ASSERT(pool.empty()    == false);
    TEST_ASSERT(pool.full()     == true);

    pool.destroy(i);
    pool.destroy(i2);
    TEST_ASSERT(pool.size()     == 0);
    TEST_ASSERT(pool.capacity() == 2);
    TEST_ASSERT(pool.empty()    == true);
    TEST_ASSERT(pool.full()     == false);

    pool.shrink_to_fit(0);
    TEST_ASSERT(pool.size()     == 0);
    TEST_ASSERT(pool.capacity() == 0);
    TEST_ASSERT(pool.empty()    == true);
    TEST_ASSERT(pool.full()     == true);

    pool.reserve(8);
    TEST_ASSERT(pool.size()     == 0);
    TEST_ASSERT(pool.capacity() == 8);
    TEST_ASSERT(pool.empty()    == true);
    TEST_ASSERT(pool.full()     == false);

    pool.shrink_to_fit(6);
    TEST_ASSERT(pool.size()     == 0);
    TEST_ASSERT(pool.capacity() == 6);
    TEST_ASSERT(pool.empty()    == true);
    TEST_ASSERT(pool.full()     == false);

    pool.shrink_to_fit(4);
    TEST_ASSERT(pool.size()     == 0);
    TEST_ASSERT(pool.capacity() == 4);
    TEST_ASSERT(pool.empty()    == true);
    TEST_ASSERT(pool.full()     == false);

    pool.shrink_to_fit(2);
    TEST_ASSERT(pool.size()     == 0);
    TEST_ASSERT(pool.capacity() == 2);
    TEST_ASSERT(pool.empty()    == true);
    TEST_ASSERT(pool.full()     == false);

    i      = pool.create();
    i2     = pool.create();
    TEST_ASSERT(i);
    TEST_ASSERT(i2);
    TEST_ASSERT(pool.size()     == 2);
    TEST_ASSERT(pool.capacity() == 2);
    TEST_ASSERT(pool.empty()    == false);
    TEST_ASSERT(pool.full()     == true);

    int* i3 = pool.create();
    int* i4 = pool.create();
    TEST_ASSERT(i3);
    TEST_ASSERT(i4);
    TEST_ASSERT(pool.size()     == 4);
    TEST_ASSERT(pool.capacity() == 4);
    TEST_ASSERT(pool.empty()    == false);
    TEST_ASSERT(pool.full()     == true);

    pool.destroy(i);
    pool.destroy(i2);
    pool.destroy(i3);
    pool.destroy(i4);
    TEST_ASSERT(pool.size()     == 0);
    TEST_ASSERT(pool.capacity() == 4);
    TEST_ASSERT(pool.empty()    == true);
    TEST_ASSERT(pool.full()     == false);

    pool.shrink_to_fit(0);
    TEST_ASSERT(pool.size()     == 0);
    TEST_ASSERT(pool.capacity() == 0);
    TEST_ASSERT(pool.empty()    == true);
    TEST_ASSERT(pool.full()     == true);

    TEST_PASS(nullptr);
}



TEST(ex_test_pool_shrink_to_fit2) //fixed_capacity
{
    DECLARE_POOL(pool, int, 2, 16, POOL_FIXED_CAPACITY)

    TEST_ASSERT(pool.size()     == 0);
    TEST_ASSERT(pool.capacity() == 2);
    TEST_ASSERT(pool.empty()    == true);
    TEST_ASSERT(pool.full()     == false);

    pool.shrink_to_fit(2); //no effect
    TEST_ASSERT(pool.size()     == 0);
    TEST_ASSERT(pool.capacity() == 2);
    TEST_ASSERT(pool.empty()    == true);
    TEST_ASSERT(pool.full()     == false);

    pool.shrink_to_fit(4); //no effect
    TEST_ASSERT(pool.size()     == 0);
    TEST_ASSERT(pool.capacity() == 2);
    TEST_ASSERT(pool.empty()    == true);
    TEST_ASSERT(pool.full()     == false);

    int* i = pool.create();
    TEST_ASSERT(i);
    TEST_ASSERT(pool.size()     == 1);
    TEST_ASSERT(pool.capacity() == 2);
    TEST_ASSERT(pool.empty()    == false);
    TEST_ASSERT(pool.full()     == false);

    int* i2 = pool.create();
    TEST_ASSERT(i2);
    TEST_ASSERT(pool.size()     == 2);
    TEST_ASSERT(pool.capacity() == 2);
    TEST_ASSERT(pool.empty()    == false);
    TEST_ASSERT(pool.full()     == true);

    pool.shrink_to_fit(2); //no effect
    TEST_ASSERT(pool.size()     == 2);
    TEST_ASSERT(pool.capacity() == 2);
    TEST_ASSERT(pool.empty()    == false);
    TEST_ASSERT(pool.full()     == true);

    pool.shrink_to_fit(0); //no effect
    TEST_ASSERT(pool.size()     == 2);
    TEST_ASSERT(pool.capacity() == 2);
    TEST_ASSERT(pool.empty()    == false);
    TEST_ASSERT(pool.full()     == true);

    pool.destroy(i);
    pool.destroy(i2);
    TEST_ASSERT(pool.size()     == 0);
    TEST_ASSERT(pool.capacity() == 2);
    TEST_ASSERT(pool.empty()    == true);
    TEST_ASSERT(pool.full()     == false);

    pool.shrink_to_fit(0);
    TEST_ASSERT(pool.size()     == 0);
    TEST_ASSERT(pool.capacity() == 0);
    TEST_ASSERT(pool.empty()    == true);
    TEST_ASSERT(pool.full()     == true);

    pool.reserve(8);
    TEST_ASSERT(pool.size()     == 0);
    TEST_ASSERT(pool.capacity() == 8);
    TEST_ASSERT(pool.empty()    == true);
    TEST_ASSERT(pool.full()     == false);

    pool.shrink_to_fit(6);
    TEST_ASSERT(pool.size()     == 0);
    TEST_ASSERT(pool.capacity() == 6);
    TEST_ASSERT(pool.empty()    == true);
    TEST_ASSERT(pool.full()     == false);

    pool.shrink_to_fit(4);
    TEST_ASSERT(pool.size()     == 0);
    TEST_ASSERT(pool.capacity() == 4);
    TEST_ASSERT(pool.empty()    == true);
    TEST_ASSERT(pool.full()     == false);

    pool.shrink_to_fit(2);
    TEST_ASSERT(pool.size()     == 0);
    TEST_ASSERT(pool.capacity() == 2);
    TEST_ASSERT(pool.empty()    == true);
    TEST_ASSERT(pool.full()     == false);

    i      = pool.create();
    i2     = pool.create();
    TEST_ASSERT(i);
    TEST_ASSERT(i2);
    TEST_ASSERT(pool.size()     == 2);
    TEST_ASSERT(pool.capacity() == 2);
    TEST_ASSERT(pool.empty()    == false);
    TEST_ASSERT(pool.full()     == true);

    int* i3 = pool.create();
    int* i4 = pool.create();
    TEST_ASSERT(i3              == nullptr);
    TEST_ASSERT(i4              == nullptr);
    TEST_ASSERT(pool.size()     == 2);
    TEST_ASSERT(pool.capacity() == 2);
    TEST_ASSERT(pool.empty()    == false);
    TEST_ASSERT(pool.full()     == true);

    pool.destroy(i);
    pool.destroy(i2);
    pool.destroy(i3); //no effect
    pool.destroy(i4); //no effect
    TEST_ASSERT(pool.size()     == 0);
    TEST_ASSERT(pool.capacity() == 2);
    TEST_ASSERT(pool.empty()    == true);
    TEST_ASSERT(pool.full()     == false);

    pool.shrink_to_fit(0);
    TEST_ASSERT(pool.size()     == 0);
    TEST_ASSERT(pool.capacity() == 0);
    TEST_ASSERT(pool.empty()    == true);
    TEST_ASSERT(pool.full()     == true);

    TEST_PASS(nullptr);
}



TEST(ex_test_pool_move)
{
    DECLARE_POOL(pool , int, 2, 16, 0)
    DECLARE_POOL(pool2, int, 2, 16, 0)

    int* i = pool.create();
    TEST_ASSERT(i);
    TEST_ASSERT(pool.size()     == 1);
    TEST_ASSERT(pool.capacity() == 2);
    TEST_ASSERT(pool.empty()    == false);
    TEST_ASSERT(pool.full()     == false);

    int* i2 = pool.create();
    TEST_ASSERT(i2);
    TEST_ASSERT(pool.size()     == 2);
    TEST_ASSERT(pool.capacity() == 2);
    TEST_ASSERT(pool.empty()    == false);
    TEST_ASSERT(pool.full()     == true);

    //move operator=
    pool2 = std::move(pool);
    TEST_ASSERT(pool.size()     == 0);
    TEST_ASSERT(pool.capacity() == 0);
    TEST_ASSERT(pool.empty()    == true);
    TEST_ASSERT(pool.full()     == true);

    TEST_ASSERT(pool2.size()     == 2);
    TEST_ASSERT(pool2.capacity() == 2);
    TEST_ASSERT(pool2.empty()    == false);
    TEST_ASSERT(pool2.full()     == true);

    //move ctor
    auto pool3(std::move(pool2));
    TEST_ASSERT(pool2.size()     == 0);
    TEST_ASSERT(pool2.capacity() == 0);
    TEST_ASSERT(pool2.empty()    == true);
    TEST_ASSERT(pool2.full()     == true);

    TEST_ASSERT(pool3.size()     == 2);
    TEST_ASSERT(pool3.capacity() == 2);
    TEST_ASSERT(pool3.empty()    == false);
    TEST_ASSERT(pool3.full()     == true);

    TEST_PASS(nullptr);
}



TEST(ex_test_pool_swap)
{
    DECLARE_POOL(pool , int, 2, 16, 0)
    DECLARE_POOL(pool2, int, 2, 16, 0)

    int* i = pool.create();
    TEST_ASSERT(i);
    TEST_ASSERT(pool.size()     == 1);
    TEST_ASSERT(pool.capacity() == 2);
    TEST_ASSERT(pool.empty()    == false);
    TEST_ASSERT(pool.full()     == false);

    int* i2 = pool.create();
    TEST_ASSERT(i2);
    TEST_ASSERT(pool.size()     == 2);
    TEST_ASSERT(pool.capacity() == 2);
    TEST_ASSERT(pool.empty()    == false);
    TEST_ASSERT(pool.full()     == true);

    //swap
    std::swap(pool, pool2);
    TEST_ASSERT(pool.size()     == 0);
    TEST_ASSERT(pool.capacity() == 2);
    TEST_ASSERT(pool.empty()    == true);
    TEST_ASSERT(pool.full()     == false);

    TEST_ASSERT(pool2.size()    == 2);
    TEST_ASSERT(pool2.capacity()== 2);
    TEST_ASSERT(pool2.empty()   == false);
    TEST_ASSERT(pool2.full()    == true);

    //again
    std::swap(pool, pool2);
    TEST_ASSERT(pool2.size()    == 0);
    TEST_ASSERT(pool2.capacity()== 2);
    TEST_ASSERT(pool2.empty()   == true);
    TEST_ASSERT(pool2.full()    == false);

    TEST_ASSERT(pool.size()     == 2);
    TEST_ASSERT(pool.capacity() == 2);
    TEST_ASSERT(pool.empty()    == false);
    TEST_ASSERT(pool.full()     == true);

    TEST_PASS(nullptr);
}



static stest_func ex_dynamic_tests[] =
{
    ex_test_pool_size,
    ex_test_pool_create,
    ex_test_pool_create_except,
    ex_test_pool_create_except2,
    ex_test_pool_reserve,
    ex_test_pool_shrink_to_fit,
    ex_test_pool_shrink_to_fit2,
    ex_test_pool_move,
    ex_test_pool_swap,
};





#endif // EX_DYNAMIC_TESTS_H
