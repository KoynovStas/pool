
#define IMPL SPool_list

#include "base_tests.h"
#include "ex_tests.h"



int Temp_struct::cnt = 0;



TEST_CASE(base_case_spool_list, base_tests, NULL, test_init_func, NULL)
TEST_CASE(ex_case_spool_list,   ex_tests,   NULL, test_init_func, NULL)
