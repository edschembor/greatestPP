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
#define GREATEST_RUN_TEST( TEST )                                       \
    do {                                                                \
        if ( greatest_pre_test( #TEST ) == 1 ) 							\
		{                            									\
            int res = TEST();                                           \
            greatest_post_test( #TEST, res );                           \
        } else if ( GREATEST_LIST_ONLY() ) 								\
		{                              									\
            fprintf( GREATEST_STDOUT, "  %s\n", #TEST );                \
        }                                                               \
    } while (0)															\