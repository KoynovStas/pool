#ifndef HELPERS_H
#define HELPERS_H

#include "stest.h"





#ifndef IMPL
    #error "Need defined IMPL"
    #define IMPL Pool_dlist  //for disable warning for next code
#endif




struct Temp_struct
{
    Temp_struct(int val):tag(val)  {
        cnt++;
//        std::cout << "constr " << tag << "\n";
    }
    ~Temp_struct() {
        cnt--;
//        std::cout << "destr " << tag << "\n";
    }
    static int cnt;
    int tag;
};



static void test_init_func(struct test_case_t *test_case)
{
    (void)test_case;
    Temp_struct::cnt = 0; //clean value
}


//Dynamic pool, by default does not contain nodes.
//To use the same tests for a static and dynamic pool,
//you need to call the reserve method for the dynamic pool.
//Then the dynamic pool will contain N nodes,
//according to the template parameters, as in a static pool.
//Macro DECLARE_POOL do it for us.
#ifdef NEED_RESERVE
    #define DECLARE_POOL(Name, Type, N, Align, Flags) \
        Pool<Type, N, Align, Flags, IMPL> Name; Name.reserve(N);
#else
    #define DECLARE_POOL(Name, Type, N, Align, Flags) \
        Pool<Type, N, Align, Flags, IMPL> Name;
#endif





#endif // HELPERS_H
