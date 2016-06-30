/* @file        test_rm7.h
 * @brief       Test suite #7.
 * @details     Test of rm_rx_process_delta_element.
 * @author      Piotr Gregor <piotrek.gregor at gmail.com>
 * @version     0.1.2
 * @date        20 May 2016 09:08 PM
 * @copyright   LGPLv2.1 */


#ifndef RSYNCME_TEST_RM7_H
#define RSYNCME_TEST_RM7_H


#include "rm_defs.h"
#include "rm.h"
#include "rm_rx.h"
#include "rm_error.h"


#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>


#define RM_TEST_7_DELETE_FILES      1	/* 0 no, 1 yes */
#define RM_TEST_L_BLOCKS_SIZE       34
#define RM_TEST_L_MAX               1024UL
#define RM_TEST_FNAMES_N            15
const char* rm_test_fnames[RM_TEST_FNAMES_N];
size_t      rm_test_fsizes[RM_TEST_FNAMES_N];
size_t      rm_test_L_blocks[RM_TEST_L_BLOCKS_SIZE];

struct test_rm_file {
    FILE    *f;
    char    name[RM_FILE_LEN_MAX + 100];
};

struct test_rm_state
{
    size_t              *l;
    void                *buf;
    void                *buf2;
    struct rm_session   *s;
    struct test_rm_file f_z;
};

struct test_rm_state	rm_state;	/* global tests state */

/* @brief   The setup function which is called before
 *          all unit tests are executed.
 * @details Handles all side-effects: allocates memory needed
 *          by tests, makes IO system calls, cancels test suite
 *          run if preconditions can't be met. */
int
test_rm_setup(void **state);

/* @brief   The teardown function  called after all
 *          tests have finished. */
int
test_rm_teardown(void **state);

int
rm_random_file(char *name, uint32_t len);


/* @brief   Test if result file @f_z is reconstructed properly
 *          when x file is same as y (file has no changes). */
void
test_rm_rx_process_delta_element_1(void **state);

/* @brief   Test if result file @f_z is reconstructed properly
 *          when x is copy of y, but first byte in x is changed. */
void
test_rm_rx_process_delta_element_2(void **state);


#endif	/* RSYNCME_TEST_RM7_H */
