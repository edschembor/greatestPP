#include <stdlib.h>

#include "greatestpp.h"

/* Declare a local test suite. */
SUITE(other_suite);

TEST blah(void) {
    PASS();
}

GREATESTPP_SUITE(other_suite) {
    RUN_TEST(blah);
}