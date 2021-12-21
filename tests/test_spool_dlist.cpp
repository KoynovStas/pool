
#define IMPL SPool_dlist

#include "base_tests.h"
#include "ex_tests.h"
#include "iterator_tests.h"



TEST_CASE(base_case_spool_dlist, base_tests, NULL, test_init_func, NULL)
TEST_CASE(ex_case_spool_dlist,   ex_tests,   NULL, test_init_func, NULL)
TEST_CASE(iter_case_spool_dlist, iter_tests, NULL, test_init_func, NULL)
