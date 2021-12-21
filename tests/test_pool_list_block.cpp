
#define IMPL Pool_list_block
#define NEED_RESERVE

#include "base_tests.h"
#include "ex_dynamic_tests.h"


TEST_CASE(base_case_pool_list_block,       base_tests,       NULL, test_init_func, NULL)
TEST_CASE(ex_dinamic_case_pool_list_block, ex_dynamic_tests, NULL, test_init_func, NULL)
