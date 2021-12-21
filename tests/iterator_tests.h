#ifndef ITERATOR_TESTS_H
#define ITERATOR_TESTS_H

#include "stest.h"
#include "helpers.h"
#include "pool.h"
#include <algorithm>



using namespace pool;



TEST(iter_test_for_range)
{
    const size_t N = 10;
    size_t cnt     = 0;
    Pool<int, N, 8, 0, IMPL> pool;


    for(auto item: pool)
    {
        (void)item;
        if(++cnt > N) //guard from infinity loop
            break;
    }
    TEST_ASSERT(cnt == 0); //pool empty


    for(size_t i = 0; i < N/2; i++)
    {
        pool.create(123);
    }

    cnt = 0;
    for(auto item: pool)
    {
        if(++cnt > N) //guard from infinity loop
            break;

        TEST_ASSERT(item == 123);
    }
    TEST_ASSERT(cnt == N/2); //half

    pool.destroy_all();


    for(size_t i = 0; i < N; i++)
    {
        pool.create(-1);
    }

    cnt = 0;
    for(auto item: pool)
    {
        if(++cnt > N) //guard from infinity loop
            break;

        TEST_ASSERT(item == -1);
    }
    TEST_ASSERT(cnt == N); //full


    TEST_PASS(nullptr);
}



TEST(iter_test_for_range_struct)
{
    TEST_ASSERT(Temp_struct::cnt == 0);

    const size_t N = 16;
    size_t cnt     = 0;
    Pool<Temp_struct, N, 8, 0, IMPL> pool;


    for(auto item: pool)
    {
        (void)item;
        if(++cnt > N) //guard from infinity loop
            break;
    }
    TEST_ASSERT(cnt == 0); //pool empty



    for(size_t i = 0; i < N/2; i++)
    {
        pool.create(123);
    }

    cnt = 0;
    for(auto item: pool)
    {
        if(++cnt > N) //guard from infinity loop
            break;

        TEST_ASSERT(item.tag == 123);
    }
    TEST_ASSERT(cnt == N/2); //half

    pool.destroy_all();



    for(size_t i = 0; i < N; i++)
    {
        pool.create(-1);
    }

    cnt = 0;
    for(auto item: pool)
    {
        if(++cnt > N) //guard from infinity loop
            break;

        TEST_ASSERT(item.tag == -1);
    }
    TEST_ASSERT(cnt == N); //full


    TEST_PASS(nullptr);
}



TEST(iter_test_begin_end)
{
    const size_t N = 10;
    size_t cnt     = 0;
    Pool<int, N, 8, 0, IMPL> pool;



    for(auto it = pool.begin(); it != pool.end(); ++it)
    {
        if(++cnt > N) //guard from infinity loop
            break;
    }
    TEST_ASSERT(cnt == 0); //pool empty



    for(size_t i = 0; i < N/2; i++)
    {
        pool.create(123);
    }

    cnt = 0;
    for(auto it = pool.begin(); it != pool.end(); ++it)
    {
        if(++cnt > N) //guard from infinity loop
            break;

        TEST_ASSERT(*it == 123);
    }
    TEST_ASSERT(cnt == N/2); //half

    pool.destroy_all();



    for(size_t i = 0; i < N; i++)
    {
        pool.create(-1);
    }

    cnt = 0;
    for(auto it = pool.begin(); it != pool.end(); ++it)
    {
        if(++cnt > N) //guard from infinity loop
            break;

        TEST_ASSERT(*it == -1);
    }
    TEST_ASSERT(cnt == N); //full


    TEST_PASS(nullptr);
}



TEST(iter_test_cbegin_cend)
{
    const size_t N = 10;
    size_t cnt     = 0;
    Pool<int, N, 8, 0, IMPL> pool;


    for(auto it = pool.cbegin(); it != pool.cend(); ++it)
    {
        if(++cnt > N) //guard from infinity loop
            break;
    }
    TEST_ASSERT(cnt == 0); //pool empty



    for(size_t i = 0; i < N/2; i++)
    {
        pool.create(123);
    }

    cnt = 0;
    for(auto it = pool.cbegin(); it != pool.cend(); ++it)
    {
        if(++cnt > N) //guard from infinity loop
            break;

        TEST_ASSERT(*it == 123);
    }
    TEST_ASSERT(cnt == N/2); //half

    pool.destroy_all();



    for(size_t i = 0; i < N; i++)
    {
        pool.create(-1);
    }

    cnt = 0;
    for(auto it = pool.cbegin(); it != pool.cend(); ++it)
    {
        if(++cnt > N) //guard from infinity loop
            break;

        TEST_ASSERT(*it == -1);
    }
    TEST_ASSERT(cnt == N); //full


    TEST_PASS(nullptr);
}



TEST(iter_test_rbegin_rend)
{
    const size_t N = 10;
    size_t cnt     = 0;
    Pool<int, N, 8, 0, IMPL> pool;


    for(auto it = pool.rbegin(); it != pool.rend(); ++it)
    {
        if(++cnt > N) //guard from infinity loop
            break;
    }
    TEST_ASSERT(cnt == 0); //pool empty



    for(size_t i = 0; i < N/2; i++)
    {
        pool.create(123);
    }

    cnt       = 0;
    auto rend = pool.rend();
    for(auto it = pool.rbegin(); it != rend; ++it)
    {
        if(++cnt > N) //guard from infinity loop
            break;

        TEST_ASSERT(*it == 123);
    }
    TEST_ASSERT(cnt == N/2); //half

    pool.destroy_all();



    for(size_t i = 0; i < N; i++)
    {
        pool.create(-1);
    }

    cnt = 0;
    for(auto it = pool.rbegin(); it != pool.rend(); ++it)
    {
        if(++cnt > N) //guard from infinity loop
            break;

        TEST_ASSERT(*it == -1);
    }
    TEST_ASSERT(cnt == N); //full


    TEST_PASS(nullptr);
}



TEST(iter_test_find)
{
    const size_t N = 10;
    Pool<size_t, N, 8, 0, IMPL> pool;


    for(size_t i = 0; i < N; i++)
    {
        pool.create(i);
    }


    for(size_t i = 0; i < N; i++)
    {
        auto it = std::find(pool.begin(), pool.end(), i);
        TEST_ASSERT(it != pool.end());
        TEST_ASSERT(*it == i);
    }

    for(size_t i = N; i < N*2; i++)
    {
        auto it = std::find(pool.begin(), pool.end(), i);
        TEST_ASSERT(it == pool.end());
    }


    TEST_PASS(nullptr);
}



TEST(iter_test_count)
{
    const size_t N = 10;
    Pool<size_t, N, 8, 0, IMPL> pool;

    size_t TMP_NUMBER = 12345678;
    for(size_t i = 0; i < N; i++)
    {
        pool.create(TMP_NUMBER);
    }

    TEST_ASSERT(std::count(pool.begin(), pool.end(), TMP_NUMBER) == N);

    pool.destroy_all();
    for(size_t i = 0; i < N; i++)
    {
        pool.create(i);
    }

    for(size_t i = 0; i < N; i++)
    {
        TEST_ASSERT(std::count(pool.begin(), pool.end(), i) == 1);
    }

    TEST_PASS(nullptr);
}



TEST(iter_test_fill)
{
    const size_t N = 10;
    Pool<size_t, N, 8, 0, IMPL> pool;

    for(size_t i = 0; i < N; i++)
    {
        pool.create(0);
    }

    TEST_ASSERT(std::count(pool.begin(), pool.end(), 0) == N);


    for(size_t i = 0; i < N; i++)
    {
        std::fill(pool.begin(), pool.end(), i);
        TEST_ASSERT(std::count(pool.begin(), pool.end(), i) == N);
    }

    TEST_PASS(nullptr);
}



TEST(iter_test_reverse)
{
    const size_t N   = 10;
    size_t       cnt = 0;
    std::array<int*, N> pint1, pint2;
    Pool<int, N, 8, 0, IMPL> pool1, pool2;


    for(size_t i = 0; i < N; i++)
    {
        pint1[i] = pool1.create(i);
        pint2[i] = pool2.create(i);
    }


    std::reverse(pool1.begin(), pool1.end());


    auto it1 = pool1.begin();
    auto it2 = pool2.rbegin();
    for(; (it1 != pool1.end()) && (it2 != pool2.rend()); ++it1, ++it2)
    {
        TEST_ASSERT(*it1 == *it2);
        if(++cnt > N) //guard from infinity loop
            break;
    }
    TEST_ASSERT(cnt == N);


    TEST_PASS(nullptr);
}



TEST(iter_test_reverse2) //reverse for reverse_iterator
{
    const size_t N   = 10;
    size_t       cnt = 0;
    std::array<int*, N> pint1, pint2;
    Pool<int, N, 8, 0, IMPL> pool1, pool2;


    for(size_t i = 0; i < N; i++)
    {
        pint1[i] = pool1.create(i);
        pint2[i] = pool2.create(i);
    }


    std::reverse(pool1.rbegin(), pool1.rend());


    auto it1 = pool1.begin();
    auto it2 = pool2.rbegin();
    for(; (it1 != pool1.end()) && (it2 != pool2.rend()); ++it1, ++it2)
    {
        TEST_ASSERT(*it1 == *it2);
        if(++cnt > N) //guard from infinity loop
            break;
    }
    TEST_ASSERT(cnt == N);


    TEST_PASS(nullptr);
}



TEST(iter_test_destroy)
{
    const size_t N   = 10;
    size_t       cnt = 0;
    Pool<int, N, 8, 0, IMPL> pool;


    pool.destroy(pool.cbegin(), pool.cend());  //no effect
    TEST_ASSERT(pool.size() == 0);

    for(size_t i = 0; i < N; i++)
    {
        pool.create(i);
    }

    TEST_ASSERT(pool.size() == N);
    pool.destroy(pool.begin(), pool.end());
    TEST_ASSERT(pool.size() == 0);



    for(size_t i = 0; i < N; i++)
    {
        pool.create(i);
    }
    TEST_ASSERT(pool.size() == N);

    auto it = pool.begin();
    while(it != pool.end())
    {
        it = pool.destroy(it);

        if(++cnt > N) //guard from infinity loop
            break;
    }
    TEST_ASSERT(cnt == N);
    TEST_ASSERT(pool.size() == 0);



    //in reverse
    for(size_t i = 0; i < N; i++)
    {
       pool.create(i);
    }
    TEST_ASSERT(pool.size() == N);


    cnt = 0;
    auto rit = pool.rbegin();
    while(rit != pool.rend())
    {
        //Deletion via reverse iterator see details in:
        //Scott Meyers Effective STL Chapter 4. Iterators

        //Equivalent code:
//        rit = std::reverse_iterator(pool.destroy(--rit.base()));
//        rit = std::reverse_iterator(pool.destroy((++rit).base()));
        rit = std::reverse_iterator(pool.destroy(std::next(rit).base()));

        if(++cnt > N) //guard from infinity loop
            break;
    }
    TEST_ASSERT(cnt == N);
    TEST_ASSERT(pool.size() == 0);


    TEST_PASS(nullptr);
}



TEST(iter_test_remove)
{
    const size_t N = 16;
    size_t     cnt = 0;
    Pool<int, N, 8, 0, IMPL> pool;


    for(size_t i = 0; i < N; i++)
    {
        pool.create(i);
    }

    TEST_ASSERT(pool.size() == N);

    auto is_even = [](int val)->bool{ return (val % 2) == 0; };
    auto is_odd  = [](int val)->bool{ return (val % 2) != 0; };
    auto cnt_if  = std::count_if(pool.begin(), pool.end(), is_even);
    TEST_ASSERT(cnt_if == N/2);

    //remove even number
    auto it_remove = std::remove_if(pool.begin(), pool.end(), is_even);

    TEST_ASSERT(it_remove != pool.end());
    TEST_ASSERT(pool.size() == N); //std::remove_if don't destroy elements in pool

    pool.destroy(it_remove, pool.end()); //real destroy
    TEST_ASSERT(pool.size() == N/2);

    cnt_if = std::count_if(pool.begin(), pool.end(), is_even);
    TEST_ASSERT(cnt_if == 0);


    for(auto it :pool)
    {
        TEST_ASSERT(is_odd(it));

        if(++cnt > N) //guard from infinity loop
            break;
    }
    TEST_ASSERT(cnt == N/2);


    //remove odd number
    it_remove = std::remove_if(pool.begin(), pool.end(), is_odd);

    TEST_ASSERT(it_remove != pool.end());
    TEST_ASSERT(pool.size() == N/2); //std::remove_if don't destroy elements in pool

    it_remove = pool.destroy(it_remove, pool.end()); //real destroy
    TEST_ASSERT(it_remove   == pool.end());
    TEST_ASSERT(pool.size() == 0);

    cnt_if = std::count_if(pool.begin(), pool.end(), is_even);
    TEST_ASSERT(cnt_if == 0);


    TEST_PASS(nullptr);
}



static stest_func iter_tests[] =
{
    iter_test_for_range,
    iter_test_for_range_struct,
    iter_test_begin_end,
    iter_test_cbegin_cend,
    iter_test_rbegin_rend,
    iter_test_find,
    iter_test_count,
    iter_test_fill,
    iter_test_reverse,
    iter_test_reverse2,
    iter_test_destroy,
    iter_test_remove,
};





#endif // ITERATOR_TESTS_H
