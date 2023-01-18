
#define IMPL Pool_dlist_block
#define NEED_RESERVE

#include "base_tests.h"
#include "ex_tests.h"
#include "ex_dynamic_tests.h"
#include "iterator_tests.h"



TEST_CASE(base_case_pool_dlist_block,       base_tests,       NULL, test_init_func, NULL)
TEST_CASE(ex_case_pool_dlist_block,         ex_tests,         NULL, test_init_func, NULL)
TEST_CASE(ex_dinamic_case_pool_dlist_block, ex_dynamic_tests, NULL, test_init_func, NULL)
TEST_CASE(iter_case_pool_dlist_block,       iter_tests,       NULL, test_init_func, NULL)
