/*
 * Copyright (c) 2011 Scott Vokes <vokes.s@gmail.com>
 * 				 2014 Edward Schembor <edschembor@gmail.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 * 
 * Based on the "greatest" C library by Scott Vokes
 */

#ifndef GREATESTPP_H
#define GREATESTPP_H

#define GREATESTPP_VERSION_MAJOR 0
#define GREATESTPP_VERSION_MINOR 1
#define GREATESTPP_VERSION_PATCH 0

/* A unit testing system for C++, contained in 1 file.
 * It doesn't use dynamic allocation */
 
 /**********************************************************
  * Minimal Test Runner Template
  **********************************************************/
  
#include "greatestpp.h"
#include <iostream>
using std::cout;
  
TEST method_works()
{
	PASS();
}
  
static void setup_cb( void *data )
{
	cout << "Setup callback for each test case\n";
}
  
static void teardown_cb( void *data )
{
	cout << "Teardown callback for each test case\n";
}

SUITE( suite )
{
	/*  Optional setup/teardown callbacks which run before/after each test case
	 *  in the suite. Clears when the suite finishes */
	SET_SETUP( setup_cb, voidp_to_callback_data );
	SET_TEARDOWN( teardown_cb, voidp_to_callback_data );
	
	RUN_TEST( method_works );
}

/* Add definitions for the test runner's main file */
GREATESTPP_MAIN_DEFS();

int main( int argc, char **argv )
{
	GREATEST_MAIN_BEGIN();
	RUN_SUITE( suite );
	GREATEST_MAIN_END(); //Display results
}

/******************************************************************/

#include <iostream>
#include <string.h>
#include <time.h>

using std::string;

/**************
 *  Options   *
 **************/
 
 /* Default column width to avoid extra long output. */
 #ifndef DEFAULT_WIDTH
 #define DEFAULT_WIDTH 72
 #endif
 
 /* FILE *, for test logging. */
 #ifndef GREATESTPP_STDOUT
 #define GREATESTPP_STDOUT stdout
 #endif

 /*************
  *   Types   *
  *************/
  
// Info for the current running suite. 
typedef struct greatest_suite_info
{
	unsigned int tests_run;
	unsigned int passed;
	unsigned int failed;
	unsigned int skipped;
	
	//Timers
	clock_t pre_suite;
	clock_t post_suite;
	clock_t pre_test;
	clock_t post_test;
} suite_info;

// Type for suite function
typedef void ( suite_cb )( void );

/* Types for setup/teardown callbacks. If non-NULL, will be run and pass
 * the pointer to their additional data */
 typedef ( setup_cb )( void *udata );
 typedef ( teardown_cb )( void *udata );
 
typedef enum
{
	FLAG_VERBOSE = 0x01,
	FLAG_FIRST_FAIL = 0x02,
	FLAG_LIST_ONLY = 0x04
} FLAG;

typedef struct run_info
{
	unsigned int flags;
	unsigned int tests_run;
	
	//Overall counts
	unsigned int passed;
	unsigned int failed;
	unsigned int skipped;
	
	//Currently running test suite
	suite_info suite;
	
	//Data on the most recent failure
	const char *fail_file;
	unsigned int fail_line;
	const char *msg;
	
	//Current setup/teardown hooks and userdata
	setup_cb *setup;
	void *setup_udata;
	teardown_cb *teardown;
	void *teardown_udata;
	
	//Formatting info
	unsigned int col;
	unsigned int width;
	
	//Only run a specific suite or test
	char *suite_filter;
	char *test_filter;
	
	//Overall timers
	clock_t begin;
	clock_t end;
} run_info;

/*Global var for the current testing context.
 * Initialized by MAIN_DEFS(). */
extern run_info info;

/*****************************
 *    Exported Functions     *
 *****************************/
 
void do_pass( const char *name );
void do_fail( const char *name );
void do_skip( const char *name );
int pre_test( const char *name );
void post_test( const char *name, int res );
void usage( const char *name );
void SET_SETUP_CB( setup_cb *cb, void *udata );
void SET_TEARDOWN_CB( teardown_cb *cb, void *udata );

/************
 *  Macros  *
 ************/
 
 /* Define a suite. */
#define SUITE( NAME ) void NAME( void )

/* Start defining a test function.
 * The arguments are not included, to allow parametric testing. */
#define TEST static int

/* Run a suite. */
#define RUN_SUITE( S_NAME ) run_suite( S_NAME, #S_NAME )

/* Run a test in the current suite. */
#define RUN_TEST( TEST )                                       			\
    do {                                                                \
        if ( pre_test( #TEST ) == 1 ) 									\
		{                            									\
            int res = TEST();                                           \
            post_test( #TEST, res );                          		 	\
        } else if ( LIST_ONLY() ) 										\
		{                              									\
            fprintf( STDOUT, "  %s\n", #TEST );                			\
        }                                                               \
    } while (0)															\
	
/* Run a test in the current suite with one void* argument,
 * which can be a pointer to a struct with multiple arguments. */
 #define RUN_TEST1(TEST, ENV)                                 		    \
    do {                                                                \
        if ( pre_test(#TEST) == 1 ) {                            		\
            int res = TEST(ENV);                                        \
            post_test( #TEST, res );                             		\
        } else if ( LIST_ONLY() ) {                              		\
            fprintf( STDOUT, "  %s\n", #TEST);                  		\
        }                                                               \
    } while (0)
	
//Parametric testing macro --- add later

//Point at which I realized deleting "GREATESTPP" before each method/variable was stupid

/* Check if the test runner is in verbose mode. */
#define GREATESTPP_IS_VERBOSE() (greatestpp_info.flags & GREATESTPP_FLAG_VERBOSE)
#define GREATESTPP_LIST_ONLY() (greatestpp_info.flags & GREATESTPP_FLAG_LIST_ONLY)
#define GREATESTPP_FIRST_FAIL() (greatestpp_info.flags & GREATESTPP_FLAG_FIRST_FAIL)
#define GREATESTPP_FAILURE_ABORT() (greatestpp_info.suite.failed > 0 && GREATESTPP_FIRST_FAIL())

/* Message-less forms. */
#define GREATESTPP_PASS() GREATESTPP_PASSm(NULL)
#define GREATESTPP_FAIL() GREATESTPP_FAILm(NULL)
#define GREATESTPP_SKIP() GREATESTPP_SKIPm(NULL)
#define GREATESTPP_ASSERT(COND) GREATESTPP_ASSERTm(#COND, COND)
#define GREATESTPP_ASSERT_FALSE(COND) GREATESTPP_ASSERT_FALSEm(#COND, COND)
#define GREATESTPP_ASSERT_EQ(EXP, GOT) GREATESTPP_ASSERT_EQm(#EXP " != " #GOT, EXP, GOT)
#define GREATESTPP_ASSERT_STR_EQ(EXP, GOT) GREATESTPP_ASSERT_STR_EQm(#EXP " != " #GOT, EXP, GOT)

/* The following forms take an additional message argument first,
 * to be displayed by the test runner. */

/* Fail if a condition is not true, with message. */
#define GREATESTPP_ASSERTm(MSG, COND)                                     \
    do {                                                                \
        greatestpp_info.msg = MSG;                                        \
        greatestpp_info.fail_file = __FILE__;                             \
        greatestpp_info.fail_line = __LINE__;                             \
        if (!(COND)) return -1;                                         \
        greatestpp_info.msg = NULL;                                       \
    } while (0)

#define GREATESTPP_ASSERT_FALSEm(MSG, COND)                               \
    do {                                                                \
        greatestpp_info.msg = MSG;                                        \
        greatestpp_info.fail_file = __FILE__;                             \
        greatestpp_info.fail_line = __LINE__;                             \
        if ((COND)) return -1;                                          \
        greatestpp_info.msg = NULL;                                       \
    } while (0)

#define GREATESTPP_ASSERT_EQm(MSG, EXP, GOT)                              \
    do {                                                                \
        greatestpp_info.msg = MSG;                                        \
        greatestpp_info.fail_file = __FILE__;                             \
        greatestpp_info.fail_line = __LINE__;                             \
        if ((EXP) != (GOT)) return -1;                                  \
        greatestpp_info.msg = NULL;                                       \
    } while (0)

#define GREATESTPP_ASSERT_STR_EQm(MSG, EXP, GOT)                          \
    do {                                                                \
        const char *exp_s = (EXP);                                      \
        const char *got_s = (GOT);                                      \
        greatestpp_info.msg = MSG;                                        \
        greatestpp_info.fail_file = __FILE__;                             \
        greatestpp_info.fail_line = __LINE__;                             \
        if (0 != strcmp(exp_s, got_s)) {                                \
            fprintf(GREATESTPP_STDOUT,                                    \
                "Expected:\n####\n%s\n####\n", exp_s);                  \
            fprintf(GREATESTPP_STDOUT,                                    \
                "Got:\n####\n%s\n####\n", got_s);                       \
            return -1;                                                  \
        }                                                               \
        greatestpp_info.msg = NULL;                                       \
    } while (0)
        
#define GREATESTPP_PASSm(MSG)                                             \
    do {                                                                \
        greatestpp_info.msg = MSG;                                        \
        return 0;                                                       \
    } while (0)
        
#define GREATESTPP_FAILm(MSG)                                             \
    do {                                                                \
        greatestpp_info.fail_file = __FILE__;                             \
        greatestpp_info.fail_line = __LINE__;                             \
        greatestpp_info.msg = MSG;                                        \
        return -1;                                                      \
    } while (0)

#define GREATESTPP_SKIPm(MSG)                                             \
    do {                                                                \
        greatestpp_info.msg = MSG;                                        \
        return 1;                                                       \
    } while (0)

#define GREATESTPP_SET_TIME(NAME)                                         \
    NAME = clock();                                                     \
    if (NAME == (clock_t) -1) {                                         \
        fprintf(GREATESTPP_STDOUT,                                        \
            "clock error: %s\n", #NAME);                                \
        exit(EXIT_FAILURE);                                             \
    }

#define GREATESTPP_CLOCK_DIFF(C1, C2)                                     \
    fprintf(GREATESTPP_STDOUT, " (%lu ticks, %.3f sec)",                  \
        (long unsigned int) (C2) - (C1),                                \
        (double)((C2) - (C1)) / (1.0 * (double)CLOCKS_PER_SEC))         \

/* Include several function definitions in the main test file. */
#define GREATESTPP_MAIN_DEFS()                                            \