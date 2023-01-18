#ifndef EX_TESTS_H
#define EX_TESTS_H

#include "stest.h"
#include "helpers.h"
#include "pool.h"




using namespace pool;




TEST(test_pool_dtor_auto)
{
    TEST_ASSERT(Temp_struct::cnt == 0);

    {
        //Temp_struct have dtor, Flags == 0 -> we must have self dtor
        Pool<Temp_struct, 4, 16, 0, IMPL> pool;

        Temp_struct* t1 = pool.create(1);

        TEST_ASSERT(t1 != nullptr);
        TEST_ASSERT(t1->tag == 1);
        TEST_ASSERT(t1->cnt == 1);

        Temp_struct* t2 = pool.create(2);

        TEST_ASSERT(t2 != nullptr);
        TEST_ASSERT(t2->tag == 2);
        TEST_ASSERT(t1->tag == 1);
        TEST_ASSERT(t1->cnt == 2);
        TEST_ASSERT(t2->cnt == 2);
    }

    TEST_ASSERT(Temp_struct::cnt == 0);

    TEST_PASS(nullptr);
}



TEST(test_pool_destroy_all)
{
    const size_t N = 10;
    Pool<int, N, 16, 0, IMPL> pool;

    std::array<int*, N> pint;

    for(size_t i = 0; i < N; i++)
    {
        pint[i] = pool.create(i);
        TEST_ASSERT(pint[i]     != nullptr);
        TEST_ASSERT(*pint[i]    == (int)i);
        TEST_ASSERT(pool.size() == (i+1));
    }

    TEST_ASSERT(pool.size() == N);

    pool.destroy_all();

    TEST_ASSERT(pool.size() == 0);

    //create after destroy_all();
    for(size_t i = 0; i < N; i++)
    {
        pint[i] = pool.create(i);
        TEST_ASSERT(pint[i]     != nullptr);
        TEST_ASSERT(*pint[i]    == (int)i);
        TEST_ASSERT(pool.size() == (i+1));
    }

    TEST_ASSERT(pool.size() == N);

    pool.destroy_all();

    TEST_ASSERT(pool.size() == 0);

    pool.destroy_all();//now pool is empty

    TEST_ASSERT(pool.size() == 0);

    TEST_PASS(nullptr);
}



[[maybe_unused]]
TEST(test_pool_dtor_off)
{
    TEST_ASSERT(Temp_struct::cnt == 0);

    {
        Pool<Temp_struct, 4, 16, POOL_DTOR_OFF, IMPL> pool;

        Temp_struct* t1 = pool.create(1);

        TEST_ASSERT(t1 != nullptr);
        TEST_ASSERT(t1->tag == 1);
        TEST_ASSERT(t1->cnt == 1);

        Temp_struct* t2 = pool.create(2);

        TEST_ASSERT(t2 != nullptr);
        TEST_ASSERT(t2->tag == 2);
        TEST_ASSERT(t1->tag == 1);
        TEST_ASSERT(t1->cnt == 2);
        TEST_ASSERT(t2->cnt == 2);
    } //no destructor for pool

    TEST_ASSERT(Temp_struct::cnt == 2);

    TEST_PASS(nullptr);
}



[[maybe_unused]]
TEST(test_pool_dtor_off_destroy_all)
{
    const size_t N = 10;
    Pool<int, N, 16, POOL_DTOR_OFF, IMPL> pool;

    std::array<int*, N> pint;

    for(size_t i = 0; i < N; i++)
    {
        pint[i] = pool.create(i);
        TEST_ASSERT(pint[i]     != nullptr);
        TEST_ASSERT(*pint[i]    == (int)i);
        TEST_ASSERT(pool.size() == (i+1));
    }

    TEST_ASSERT(pool.size() == N);

    pool.destroy_all();

    TEST_ASSERT(pool.size() == 0);

    //create after destroy_all();
    for(size_t i = 0; i < N; i++)
    {
        pint[i] = pool.create(i);
        TEST_ASSERT(pint[i]     != nullptr);
        TEST_ASSERT(*pint[i]    == (int)i);
        TEST_ASSERT(pool.size() == (i+1));
    }

    TEST_ASSERT(pool.size() == N);

    pool.destroy_all();

    TEST_ASSERT(pool.size() == 0);


    TEST_PASS(nullptr);
}



TEST(test_pool_for_each)
{
    const size_t N = 10;
    Pool<int, N, 16, 0, IMPL> pool;
    std::array<int*, N> pint;


    int cnt = 0;
    pool.for_each([&cnt](int *){ cnt++; }); //no effect pool is empty
    TEST_ASSERT(cnt == 0);


    for(size_t i = 0; i < N/2; i++)
    {
        pint[i] = pool.create(i);
        TEST_ASSERT(pint[i]     != nullptr);
        TEST_ASSERT(*pint[i]    == (int)i);
        TEST_ASSERT(pool.size() == (i+1));
    }

    pool.for_each([&cnt](int *){ cnt++; });
    TEST_ASSERT(cnt == N/2);
    pool.destroy_all();
    TEST_ASSERT(pool.size() == 0);



    for(size_t i = 0; i < N; i++)
    {
        pint[i] = pool.create(i);
        TEST_ASSERT(pint[i]     != nullptr);
        TEST_ASSERT(*pint[i]    == (int)i);
        TEST_ASSERT(pool.size() == (i+1));
    }


    for(size_t i = 0; i < N; i++)
    {
        pool.for_each([i](int *obj){ *obj = i; });
        for(auto obj: pint)
            TEST_ASSERT((size_t)*obj == i);
    }

    TEST_PASS(nullptr);
}



static stest_func ex_tests[] =
{
    test_pool_dtor_auto,
    test_pool_destroy_all,
    #ifdef RUN_DTOR_OFF_TESTS
    test_pool_dtor_off,
    test_pool_dtor_off_destroy_all,
    #endif
    test_pool_for_each,
};





#endif // EX_TESTS_H
