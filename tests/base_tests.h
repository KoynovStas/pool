#ifndef BASE_TESTS_H
#define BASE_TESTS_H

#include "stest.h"
#include "helpers.h"
#include "pool.h"




using namespace pool;




#define TEST_ALIGN(T, N, Align) {                      \
    Pool<T, N, Align, 0, IMPL> pool;                   \
    for(int i = 0; i < N; i++){                        \
        T* x = pool.create(Align);                     \
        TEST_ASSERT2(x != nullptr, "Can't create");    \
        TEST_ASSERT2(*x == Align,  "Bad init");        \
        TEST_ASSERT2(pool.ALIGN == Align, "Bad const");\
        TEST_ASSERT2((std::uintptr_t)(x)%Align == 0, "Bad align"); } }


TEST(test_pool_align)
{
    TEST_ALIGN(char, 1, 1)
    TEST_ALIGN(char, 2, 1)
    TEST_ALIGN(char, 3, 1)

    TEST_ALIGN(char, 1, 2)
    TEST_ALIGN(char, 2, 2)
    TEST_ALIGN(char, 3, 2)

    TEST_ALIGN(int, 1, 4)
    TEST_ALIGN(int, 2, 4)
    TEST_ALIGN(int, 3, 4)

    TEST_ALIGN(int, 3, 8)
    TEST_ALIGN(int, 3, 16)
    TEST_ALIGN(int, 3, 32)
    TEST_ALIGN(int, 3, 64)
    TEST_ALIGN(int, 3, 128)
    TEST_ALIGN(int, 3, 256)
    TEST_ALIGN(int, 3, 512)
    TEST_ALIGN(int, 3, 1024)
    TEST_ALIGN(int, 3, 2048)
    TEST_ALIGN(int, 3, 4096)

    TEST_PASS(nullptr);
}



TEST(test_pool_size)
{
    //use flag POOL_FIXED_CAPACITY it's no effect for Static
    //But for dynamics, it gives behavior like static Pool method size()
    DECLARE_POOL(pool, int, 2, 16, POOL_FIXED_CAPACITY)


    TEST_ASSERT(pool.size()     == 0);
    TEST_ASSERT(pool.capacity() == 2);
    TEST_ASSERT(pool.empty()    == true);
    TEST_ASSERT(pool.full()     == false);


    int* i = pool.create();
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

    //create when full
    int* i3 = pool.create();
    TEST_ASSERT(i3 == nullptr);
    TEST_ASSERT(pool.size()     == 2);
    TEST_ASSERT(pool.capacity() == 2);
    TEST_ASSERT(pool.empty()    == false);
    TEST_ASSERT(pool.full()     == true);


    pool.destroy(i);
    TEST_ASSERT(pool.size()     == 1);
    TEST_ASSERT(pool.capacity() == 2);
    TEST_ASSERT(pool.empty()    == false);
    TEST_ASSERT(pool.full()     == false);

    pool.destroy(i2);
    TEST_ASSERT(pool.size()     == 0);
    TEST_ASSERT(pool.capacity() == 2);
    TEST_ASSERT(pool.empty()    == true);
    TEST_ASSERT(pool.full()     == false);

    pool.destroy(nullptr);
    TEST_ASSERT(pool.size()     == 0);
    TEST_ASSERT(pool.capacity() == 2);
    TEST_ASSERT(pool.empty()    == true);
    TEST_ASSERT(pool.full()     == false);

    TEST_PASS(nullptr);
}



TEST(test_pool_create)
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

    for(size_t i = 0; i < N; i++)
    {
        TEST_ASSERT(*pint[i] == (int)i);
    }

    TEST_ASSERT(pool.size() == N);


    for(size_t i = 0; i < N; i++)
    {
        TEST_ASSERT(pool.size() == N-i);
        pool.destroy(pint[i]);
    }

    TEST_ASSERT(pool.size() == 0);

    TEST_PASS(nullptr);
}



TEST(test_pool_destroy)
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


    for(size_t i = 0; i < N; i += 2)
    {
        pool.destroy(pint[i]);
    }


    TEST_ASSERT(pool.size() == N/2);


    //create after destroy;
    for(size_t i = 0; i < N; i += 2)
    {
        pint[i] = pool.create(i);
        TEST_ASSERT(pint[i]  != nullptr);
        TEST_ASSERT(*pint[i] == (int)i);
    }

    TEST_ASSERT(pool.size() == N);


    for(size_t i = 0; i < N; i++)
    {
        TEST_ASSERT(*pint[i] == (int)i);
    }

    for(size_t i = 0; i < N; i++)
    {
        pool.destroy(pint[i]);
    }

    TEST_ASSERT(pool.size() == 0);


    TEST_PASS(nullptr);
}



TEST(test_pool_struct)
{
    TEST_ASSERT(Temp_struct::cnt == 0);

    //use flag POOL_FIXED_CAPACITY it's no effect for Static
    //But for dynamics, it gives behavior like static Pool method size()
    //no create when full()
    DECLARE_POOL(pool, Temp_struct, 2, 16, POOL_FIXED_CAPACITY)

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

    //create when full
    Temp_struct* t3 = pool.create(3);

    TEST_ASSERT(t3 == nullptr);
    TEST_ASSERT(t2->tag == 2);
    TEST_ASSERT(t1->tag == 1);
    TEST_ASSERT(t1->cnt == 2);
    TEST_ASSERT(t2->cnt == 2);


    pool.destroy(t1);
    TEST_ASSERT(Temp_struct::cnt == 1);
    TEST_ASSERT(Temp_struct::cnt == 1);

    pool.destroy(t2);
    TEST_ASSERT(Temp_struct::cnt == 0);
    TEST_ASSERT(Temp_struct::cnt == 0);


    TEST_PASS(nullptr);
}



TEST(test_pool_dtor_off_struct)
{
    TEST_ASSERT(Temp_struct::cnt == 0);

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

    pool.destroy(t1);
    TEST_ASSERT(Temp_struct::cnt == 1);
    TEST_ASSERT(Temp_struct::cnt == 1);

    pool.destroy(t2);
    TEST_ASSERT(Temp_struct::cnt == 0);
    TEST_ASSERT(Temp_struct::cnt == 0);


    TEST_PASS(nullptr);
}




static stest_func base_tests[] =
{
    test_pool_align,
    test_pool_size,
    test_pool_create,
    test_pool_destroy,
    test_pool_struct,
    test_pool_dtor_off_struct,
};





#endif // BASE_TESTS_H
