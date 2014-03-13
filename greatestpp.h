/*New GreatestPP File*/

/*
 * Copyright (c) 2011 Scott Vokes <vokes.s@gmail.com>
 *               2014 Edward Schembor <edschembor@gmail.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHoOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef GREATESTPP_H
#define GREATESTPP_H

#define GREATESTPP_VERSION_MAJOR 0
#define GREATESTPP_VERSION_MINOR 1
#define GREATESTPP_VERSION_PATCH 0

/* A unit testing system for C++, contained in 1 file.
 * It doesn't use dynamic allocation or depend on anything
 * beyond ANSI C89. It is based of of the C unit testing system
 * called "Greatest" created by Scott Vokes, vokes.s@gmail.com*/


/*********************************************************************
 * Minimal test runner template
 *********************************************************************/
#if 0

#include "greatestpp.h"

TEST foo_should_foo() {
    PASS();
}

static void setup_cb(void *data) {
    printf("setup callback for each test case\n");
}

static void teardown_cb(void *data) {
    printf("teardown callback for each test case\n");
}

SUITE(suite) {
    /* Optional setup/teardown callbacks which will be run before/after
     * every test case in the suite.
     * Cleared when the suite finishes. */
    SET_SETUP(setup_cb, voidp_to_callback_data);
    SET_TEARDOWN(teardown_cb, voidp_to_callback_data);

    RUN_TEST(foo_should_foo);
}

/* Add all the definitions that need to be in the test runner's main file. */
GREATESTPP_MAIN_DEFS();

int main(int argc, char **argv) {
    GREATESTPP_MAIN_BEGIN();      /* command-line arguments, initialization. */
    RUN_SUITE(suite);
    GREATESTPP_MAIN_END();        /* display results */
}

#endif
/*********************************************************************/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>


/***********
 * Options *
 ***********/

/* Default column width for non-verbose output. */
#ifndef GREATESTPP_DEFAULT_WIDTH
#define GREATESTPP_DEFAULT_WIDTH 70
#endif

/* FILE *, for test logging. */
#ifndef GREATESTPP_STDOUT
#define GREATESTPP_STDOUT stdout
#endif

/* Remove GREATEST_ prefix from most commonly used symbols? */
#ifndef GREATESTPP_USE_ABBREVS
#define GREATESTPP_USE_ABBREVS 1
#endif


/*********
 * Types *
 *********/

/* Info for the current running suite. */
typedef struct greatestpp_suite_info {
    unsigned int tests_run;
    unsigned int passed;
    unsigned int failed;
    unsigned int skipped;

    /* timers, pre/post running suite and individual tests */
    clock_t pre_suite;
    clock_t post_suite;
    clock_t pre_test;
    clock_t post_test;
} greatestpp_suite_info;

/* Type for a suite function. */
typedef void (greatestpp_suite_cb)(void);

/* Types for setup/teardown callbacks. If non-NULL, these will be run
 * and passed the pointer to their additional data. */
typedef void (greatestpp_setup_cb)(void *udata);
typedef void (greatestpp_teardown_cb)(void *udata);

typedef enum {
    GREATESTPP_FLAG_VERBOSE = 0x01,
    GREATESTPP_FLAG_FIRST_FAIL = 0x02,
    GREATESTPP_FLAG_LIST_ONLY = 0x04
} GREATESTPP_FLAG;

typedef struct greatestpp_run_info {
    unsigned int flags;
    unsigned int tests_run;     /* total test count */

    /* Overall pass/fail/skip counts. */
    unsigned int passed;
    unsigned int failed;
    unsigned int skipped;

    /* currently running test suite */
    greatestpp_suite_info suite;

    /* info to print about the most recent failure */
    const char *fail_file;
    unsigned int fail_line;
    const char *msg;

    /* current setup/teardown hooks and userdata */
    greatestpp_setup_cb *setup;
    void *setup_udata;
    greatestpp_teardown_cb *teardown;
    void *teardown_udata;

    /* formatting info for ".....s...F"-style output */
    unsigned int col;
    unsigned int width;

    /* only run a specific suite or test */
    char *suite_filter;
    char *test_filter;

    /* overall timers */
    clock_t begin;
    clock_t end;
} greatestpp_run_info;

/* Global var for the current testing context.
 * Initialized by GREATEST_MAIN_DEFS(). */
extern greatestpp_run_info greatest_info;


/**********************
 * Exported functions *
 **********************/

void greatestpp_do_pass(const char *name);
void greatestpp_do_fail(const char *name);
void greatestpp_do_skip(const char *name);
int greatestpp_pre_test(const char *name);
void greatestpp_post_test(const char *name, int res);
void greatestpp_usage(const char *name);
void GREATESTPP_SET_SETUP_CB(greatest_setup_cb *cb, void *udata);
void GREATESTPP_SET_TEARDOWN_CB(greatest_teardown_cb *cb, void *udata);


/**********
 * Macros *
 **********/

/* Define a suite. */
#define GREATESTPP_SUITE(NAME) void NAME(void)

/* Start defining a test function.
 * The arguments are not included, to allow parametric testing. */
#define GREATESTPP_TEST static int

/* Run a suite. */
#define GREATESTPP_RUN_SUITE(S_NAME) greatestpp_run_suite(S_NAME, #S_NAME)

/* Run a test in the current suite. */
#define GREATESTPP_RUN_TEST(TEST)                                         \
    do {                                                                \
        if (greatestpp_pre_test(#TEST) == 1) {                            \
            int res = TEST();                                           \
            greatestpp_post_test(#TEST, res);                             \
        } else if (GREATESTPP_LIST_ONLY()) {                              \
            fprintf(GREATESTPP_STDOUT, "  %s\n", #TEST);                  \
        }                                                               \
    } while (0)

/* Run a test in the current suite with one void* argument,
 * which can be a pointer to a struct with multiple arguments. */
#define GREATESTPP_RUN_TEST1(TEST, ENV)                                   \
    do {                                                                \
        if (greatestpp_pre_test(#TEST) == 1) {                            \
            int res = TEST(ENV);                                        \
            greatestpp_post_test(#TEST, res);                             \
        } else if (GREATESTPP_LIST_ONLY()) {                              \
            fprintf(GREATESTPP_STDOUT, "  %s\n", #TEST);                  \
        }                                                               \
    } while (0)

/* If __VA_ARGS__ (C99) is supported, allow parametric testing
 * without needing to manually manage the argument struct. */
#if __STDC_VERSION__ >= 19901L
#define GREATESTPP_RUN_TESTp(TEST, ...)                                   \
    do {                                                                \
        if (greatestpp_pre_test(#TEST) == 1) {                            \
            int res = TEST(__VA_ARGS__);                                \
            greatestpp_post_test(#TEST, res);                             \
        } else if (GREATESTPP_LIST_ONLY()) {                              \
            fprintf(GREATESTPP_STDOUT, "  %s\n", #TEST);                  \
        }                                                               \
    } while (0)
#endif


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
                                                                        \
/* Is FILTER a subset of NAME? */                                       \
static int greatestpp_name_match(const char *name,                        \
    const char *filter) {                                               \
    size_t offset = 0;                                                  \
    size_t filter_len = strlen(filter);                                 \
    while (name[offset] != '\0') {                                      \
        if (name[offset] == filter[0]) {                                \
            if (0 == strncmp(&name[offset], filter, filter_len)) {      \
                return 1;                                               \
            }                                                           \
        }                                                               \
        offset++;                                                       \
    }                                                                   \
                                                                        \
    return 0;                                                           \
}                                                                       \
                                                                        \
int greatestpp_pre_test(const char *name) {                               \
    if (!GREATESTPP_LIST_ONLY()                                           \
        && (!GREATESTPP_FIRST_FAIL() || greatestpp_info.suite.failed == 0)  \
        && (greatestpp_info.test_filter == NULL ||                        \
            greatestpp_name_match(name, greatestpp_info.test_filter))) {    \
        GREATESTPP_SET_TIME(greatestpp_info.suite.pre_test);                \
        if (greatestpp_info.setup) {                                      \
            greatestpp_info.setup(greatestpp_info.setup_udata);             \
        }                                                               \
        return 1;               /* test should be run */                \
    } else {                                                            \
        return 0;               /* skipped */                           \
    }                                                                   \
}                                                                       \
                                                                        \
void greatestpp_post_test(const char *name, int res) {                    \
    GREATESTPP_SET_TIME(greatestpp_info.suite.post_test);                   \
    if (greatestpp_info.teardown) {                                       \
        void *udata = greatestpp_info.teardown_udata;                     \
        greatestpp_info.teardown(udata);                                  \
    }                                                                   \
                                                                        \
    if (res < 0) {                                                      \
        greatestpp_do_fail(name);                                         \
    } else if (res > 0) {                                               \
        greatestpp_do_skip(name);                                         \
    } else if (res == 0) {                                              \
        greatestpp_do_pass(name);                                         \
    }                                                                   \
    greatestpp_info.suite.tests_run++;                                    \
    greatestpp_info.col++;                                                \
    if (GREATESTPP_IS_VERBOSE()) {                                        \
        GREATESTPP_CLOCK_DIFF(greatestpp_info.suite.pre_test,               \
            greatestpp_info.suite.post_test);                             \
        fprintf(GREATESTPP_STDOUT, "\n");                                 \
    } else if (greatestpp_info.col % greatestpp_info.width == 0) {          \
        fprintf(GREATESTPP_STDOUT, "\n");                                 \
        greatestpp_info.col = 0;                                          \
    }                                                                   \
    if (GREATESTPP_STDOUT == stdout) fflush(stdout);                      \
}                                                                       \
                                                                        \
static void greatest_run_suite(greatestpp_suite_cb *suite_cb,             \
                               const char *suite_name) {                \
    if (greatestpp_info.suite_filter &&                                   \
        !greatestpp_name_match(suite_name, greatestpp_info.suite_filter))   \
        return;                                                         \
    if (GREATESTPP_FIRST_FAIL() && greatest_info.failed > 0) return;      \
    greatestpp_info.suite.tests_run = 0;                                  \
    greatestpp_info.suite.failed = 0;                                     \
    greatestpp_info.suite.passed = 0;                                     \
    greatestpp_info.suite.skipped = 0;                                    \
    greatestpp_info.suite.pre_suite = 0;                                  \
    greatestpp_info.suite.post_suite = 0;                                 \
    greatestpp_info.suite.pre_test = 0;                                   \
    greatestpp_info.suite.post_test = 0;                                  \
    greatestpp_info.col = 0;                                              \
    fprintf(GREATESTPP_STDOUT, "\n* Suite %s:\n", suite_name);            \
    GREATESTPP_SET_TIME(greatestpp_info.suite.pre_suite);                   \
    suite_cb();                                                         \
    GREATESTPP_SET_TIME(greatestpp_info.suite.post_suite);                  \
    if (greatestpp_info.suite.tests_run > 0) {                            \
        fprintf(GREATESTPP_STDOUT,                                        \
            "\n%u tests - %u pass, %u fail, %u skipped",                \
            greatestpp_info.suite.tests_run,                              \
            greatestpp_info.suite.passed,                                 \
            greatestpp_info.suite.failed,                                 \
            greatestpp_info.suite.skipped);                               \
        GREATESTPP_CLOCK_DIFF(greatestpp_info.suite.pre_suite,              \
            greatestpp_info.suite.post_suite);                            \
        fprintf(GREATESTPP_STDOUT, "\n");                                 \
    }                                                                   \
    greatestpp_info.setup = NULL;                                         \
    greatestpp_info.setup_udata = NULL;                                   \
    greatestpp_info.teardown = NULL;                                      \
    greatestpp_info.teardown_udata = NULL;                                \
    greatestpp_info.passed += greatestpp_info.suite.passed;                 \
    greatestpp_info.failed += greatestpp_info.suite.failed;                 \
    greatestpp_info.skipped += greatestpp_info.suite.skipped;               \
    greatestpp_info.tests_run += greatestpp_info.suite.tests_run;           \
}                                                                       \
                                                                        \
void greatestpp_do_pass(const char *name) {                               \
    if (GREATESTPP_IS_VERBOSE()) {                                        \
        fprintf(GREATESTPP_STDOUT, "PASS %s: %s",                         \
            name, greatestpp_info.msg ? greatestpp_info.msg : "");          \
    } else {                                                            \
        fprintf(GREATESTPP_STDOUT, ".");                                  \
    }                                                                   \
    greatestpp_info.suite.passed++;                                       \
}                                                                       \
                                                                        \
void greatestpp_do_fail(const char *name) {                               \
    if (GREATESTPP_IS_VERBOSE()) {                                        \
        fprintf(GREATESTPP_STDOUT,                                        \
            "FAIL %s: %s (%s:%u)",                                      \
            name, greatestpp_info.msg ? greatestpp_info.msg : "",           \
            greatestpp_info.fail_file, greatestpp_info.fail_line);          \
    } else {                                                            \
        fprintf(GREATESTPP_STDOUT, "F");                                  \
        /* add linebreak if in line of '.'s */                          \
        if (greatestpp_info.col % greatestpp_info.width != 0)               \
            fprintf(GREATESTPP_STDOUT, "\n");                             \
        greatestpp_info.col = 0;                                          \
        fprintf(GREATESTPP_STDOUT, "FAIL %s: %s (%s:%u)\n",               \
            name,                                                       \
            greatestpp_info.msg ? greatestpp_info.msg : "",                 \
            greatestpp_info.fail_file, greatestpp_info.fail_line);          \
    }                                                                   \
    greatestpp_info.suite.failed++;                                       \
}                                                                       \
                                                                        \
void greatestpp_do_skip(const char *name) {                               \
    if (GREATESTPP_IS_VERBOSE()) {                                        \
        fprintf(GREATESTPP_STDOUT, "SKIP %s: %s",                         \
            name,                                                       \
            greatestpp_info.msg ?                                         \
            greatestpp_info.msg : "" );                                   \
    } else {                                                            \
        fprintf(GREATESTPP_STDOUT, "s");                                  \
    }                                                                   \
    greatestpp_info.suite.skipped++;                                      \
}                                                                       \
                                                                        \
void greatestpp_usage(const char *name) {                                 \
    fprintf(GREATESTPP_STDOUT,                                            \
        "Usage: %s [-hlfv] [-s SUITE] [-t TEST]\n"                      \
        "  -h        print this Help\n"                                 \
        "  -l        List suites and their tests, then exit\n"          \
        "  -f        Stop runner after first failure\n"                 \
        "  -v        Verbose output\n"                                  \
        "  -s SUITE  only run suite named SUITE\n"                      \
        "  -t TEST   only run test named TEST\n",                       \
        name);                                                          \
}                                                                       \
                                                                        \
void GREATESTPP_SET_SETUP_CB(greatestpp_setup_cb *cb, void *udata) {        \
    greatestpp_info.setup = cb;                                           \
    greatestpp_info.setup_udata = udata;                                  \
}                                                                       \
                                                                        \
void GREATESTPP_SET_TEARDOWN_CB(greatestpp_teardown_cb *cb,                 \
                                    void *udata) {                      \
    greatestpp_info.teardown = cb;                                        \
    greatestpp_info.teardown_udata = udata;                               \
}                                                                       \
                                                                        \
greatestpp_run_info greatestpp_info

/* Handle command-line arguments, etc. */
#define GREATESTPP_MAIN_BEGIN()                                           \
    do {                                                                \
        int i = 0;                                                      \
        memset(&greatestpp_info, 0, sizeof(greatestpp_info));               \
        if (greatestpp_info.width == 0) {                                 \
            greatestpp_info.width = GREATESTPP_DEFAULT_WIDTH;               \
        }                                                               \
        for (i = 1; i < argc; i++) {                                    \
            if (0 == strcmp("-t", argv[i])) {                           \
                if (argc <= i + 1) {                                    \
                    greatestpp_usage(argv[0]);                            \
                    exit(EXIT_FAILURE);                                 \
                }                                                       \
                greatestpp_info.test_filter = argv[i+1];                  \
                i++;                                                    \
            } else if (0 == strcmp("-s", argv[i])) {                    \
                if (argc <= i + 1) {                                    \
                    greatestpp_usage(argv[0]);                            \
                    exit(EXIT_FAILURE);                                 \
                }                                                       \
                greatestpp_info.suite_filter = argv[i+1];                 \
                i++;                                                    \
            } else if (0 == strcmp("-f", argv[i])) {                    \
                greatestpp_info.flags |= GREATESTPP_FLAG_FIRST_FAIL;        \
            } else if (0 == strcmp("-v", argv[i])) {                    \
                greatestpp_info.flags |= GREATESTPP_FLAG_VERBOSE;           \
            } else if (0 == strcmp("-l", argv[i])) {                    \
                greatestpp_info.flags |= GREATESTPP_FLAG_LIST_ONLY;         \
            } else if (0 == strcmp("-h", argv[i])) {                    \
                greatestpp_usage(argv[0]);                                \
                exit(EXIT_SUCCESS);                                     \
            } else {                                                    \
                fprintf(GREATESTPP_STDOUT,                                \
                    "Unknown argument '%s'\n", argv[i]);                \
                greatestpp_usage(argv[0]);                                \
                exit(EXIT_FAILURE);                                     \
            }                                                           \
        }                                                               \
    } while (0);                                                        \
    GREATESTPP_SET_TIME(greatestpp_info.begin)

#define GREATESTPP_MAIN_END()                                             \
    do {                                                                \
        if (!GREATESTPP_LIST_ONLY()) {                                    \
            GREATESTPP_SET_TIME(greatestpp_info.end);                       \
            fprintf(GREATESTPP_STDOUT,                                    \
                "\nTotal: %u tests", greatestpp_info.tests_run);          \
            GREATESTPP_CLOCK_DIFF(greatestpp_info.begin,                    \
                greatestpp_info.end);                                     \
            fprintf(GREATESTPP_STDOUT, "\n");                             \
            fprintf(GREATESTPP_STDOUT,                                    \
                "Pass: %u, fail: %u, skip: %u.\n",                      \
                greatestpp_info.passed,                                   \
                greatestpp_info.failed, greatestpp_info.skipped);           \
        }                                                               \
        return (greatestpp_info.failed > 0                                \
            ? EXIT_FAILURE : EXIT_SUCCESS);                             \
    } while (0)

/* Make abbreviations without the GREATEST_ prefix for the
 * most commonly used symbols. */
#if GREATEST_USE_ABBREVS
#define TEST           GREATESTPP_TEST
#define SUITE          GREATESTPP_SUITE
#define RUN_TEST       GREATESTPP_RUN_TEST
#define RUN_TEST1      GREATESTPP_RUN_TEST1
#define RUN_SUITE      GREATESTPP_RUN_SUITE
#define ASSERT         GREATESTPP_ASSERT
#define ASSERTm        GREATESTPP_ASSERTm
#define ASSERT_FALSE   GREATESTPP_ASSERT_FALSE
#define ASSERT_EQ      GREATESTPP_ASSERT_EQ
#define ASSERT_STR_EQ  GREATESTPP_ASSERT_STR_EQ
#define ASSERT_FALSEm  GREATESTPP_ASSERT_FALSEm
#define ASSERT_EQm     GREATESTPP_ASSERT_EQm
#define ASSERT_STR_EQm GREATESTPP_ASSERT_STR_EQm
#define PASS           GREATESTPP_PASS
#define FAIL           GREATESTPP_FAIL
#define SKIP           GREATESTPP_SKIP
#define PASSm          GREATESTPP_PASSm
#define FAILm          GREATESTPP_FAILm
#define SKIPm          GREATESTPP_SKIPm
#define SET_SETUP      GREATESTPP_SET_SETUP_CB
#define SET_TEARDOWN   GREATESTPP_SET_TEARDOWN_CB

#if __STDC_VERSION__ >= 19901L
#endif /* C99 */
#define RUN_TESTp      GREATESTPP_RUN_TESTp
#endif /* USE_ABBREVS */

#endif
