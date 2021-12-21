#include "stest.h"




extern struct test_case_t base_case_spool_list;
extern struct test_case_t ex_case_spool_list;

extern struct test_case_t base_case_spool_list_bitset;
extern struct test_case_t ex_case_spool_list_bitset;
extern struct test_case_t iter_case_spool_list_bitset;

extern struct test_case_t base_case_spool_dlist;
extern struct test_case_t ex_case_spool_dlist;
extern struct test_case_t iter_case_spool_dlist;


extern struct test_case_t base_case_pool_list;
extern struct test_case_t ex_dinamic_case_pool_list;

extern struct test_case_t base_case_pool_list_block;
extern struct test_case_t ex_dinamic_case_pool_list_block;

extern struct test_case_t base_case_pool_dlist;
extern struct test_case_t ex_case_pool_dlist;
extern struct test_case_t ex_dinamic_case_pool_dlist;

extern struct test_case_t base_case_pool_dlist_block;
extern struct test_case_t ex_case_pool_dlist_block;
extern struct test_case_t ex_dinamic_case_pool_dlist_block;



static struct test_case_t *cases[] =
{
    &base_case_spool_list       ,
    &ex_case_spool_list         ,

    &base_case_spool_list_bitset,
    &ex_case_spool_list_bitset  ,
    &iter_case_spool_list_bitset,

    &base_case_spool_dlist      ,
    &ex_case_spool_dlist        ,
    &iter_case_spool_dlist      ,


    &base_case_pool_list       ,
    &ex_dinamic_case_pool_list ,

    &base_case_pool_list_block ,
    &ex_dinamic_case_pool_list_block,

    &base_case_pool_dlist       ,
    &ex_case_pool_dlist         ,
    &ex_dinamic_case_pool_dlist ,

    &base_case_pool_dlist_block ,
    &ex_case_pool_dlist_block   ,
    &ex_dinamic_case_pool_dlist_block,
};



MAIN_CASES(cases)
