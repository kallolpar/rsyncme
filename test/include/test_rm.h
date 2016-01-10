/// @file	test_rm.h
/// @author	peterg
/// @version	0.1.1
/// @date	10 Jan 2016 04:07 PM
/// @copyright	LGPLv2.1


#ifndef RSYNCME_TEST_RM_H
#define RSYNCME_TEST_RM_H


#include "rm.h"
#include "rm_error.h"


#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>


#define RM_TEST_DELETE_FILES	1	// 0 no, 1 yes
char	rm_f_100_name[RM_FILE_LEN_MAX];

struct test_rm_state
{
};

struct test_rm_state	rm_state;	// global tests state

/// The setup function which is called before
/// all unit tests are executed.
/// Handles all side-effects: allocates memory needed
/// by tests, makes IO system calls, cancels test suite
/// run if preconditions can't be met. 
int
test_rm_setup(void **state);

/// The teardown function  called after all
/// tests have finished.
int
test_rm_teardown(void **state);

int
rm_random_file(char *name, uint32_t len);

void
test_rm_adler32_1(void **state);


#endif	// RSYNCME_TEST_RM_H
