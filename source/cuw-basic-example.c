/*
  MIT License

  Copyright (c) 2019 Hervé Retaureau

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

#include "cuw.h"

#include <stdlib.h>

/* Test definition - Can be dispatched in external file, one by test suite for example.
  A test suite file may include the following definition (check after main entry point):
    + all test definitions
    + the tCuwSuite definition
    + the test getter implementation
  A common test header may include all test getter declaration (hereunder).
 *================================================================================================*/

tCuwSuite* getTS1();
tCuwSuite* getTS2();

/* Test management
 *================================================================================================*/

static tCuwSuiteGetter tests[] = { getTS1, getTS2, CUW_SUITE_END };

int main(int argc, char *argv[]) {
  int e = 0;
  tCuwContext c;
  if (0 != (e = cuwGetContext(&c, argc, argv)))
    return (1 == e) ? EXIT_SUCCESS : EXIT_FAILURE;
  if (!cuwProcess(&c, tests))
    return EXIT_FAILURE;
  return EXIT_SUCCESS;
}

/* Screen result should be as follow
 *------------------------------------------------------------------------------------------------*

Suite: Test suite #1
  Test: TS#1 - Test #1 ...FAILED
    1. source/cuw-basic-example.c:94  - 3 == myAddition(2, 2)
  Test: TS#1 - Test #2 ...passed
Suite: Test suite 2
  Test: First test of TS2 ...FAILED
    1. source/cuw-basic-example.c:125  - cuwCheckOutput(printSomething, "The quick brown fox jumps over the lazy dog!\n")
    2. source/cuw-basic-example.c:129  - cuwCheckOutput(printSomething, "The quick brown dog jumps over the lazy fox!\n")

Run Summary:    Type  Total    Ran Passed Failed Inactive
              suites      2      2    n/a      0        0
               tests      3      3      1      2        0
             asserts      9      9      6      3      n/a

 *------------------------------------------------------------------------------------------------*/

/* Example of test definition for 2 test suites.
   Can be dispatched in separated test suite files.
 *================================================================================================*/

/* Dummy implementation of something we want to test - we'll use it in both test suites
 *------------------------------------------------------------------------------------------------*/

int myAddition(int a, int b) {
  return a + b;
}

void printSomething() {
  fprintf(stdout, "The quick brown fox jumps over the lazy dog!");
}

/* Test suite 1
 *------------------------------------------------------------------------------------------------*/

static void test11(void) {
  CU_ASSERT(2 == myAddition(1, 1));
  CU_ASSERT(3 == myAddition(2, 2));   // Just to have some test failed reported
  CU_ASSERT(5 == myAddition(2, 3));
}

static void test12(void) {
  CU_ASSERT(-2 == myAddition(-1, -1));
  CU_ASSERT(-4 == myAddition(-2, -2));
  CU_ASSERT(-5 == myAddition(-2, -3));
}

tCuwSuite *getTS1() {

  static tCuwTest tests1[] = {
    { "TS#1 - Test #1", test11 },
    { "TS#1 - Test #2", test12 },
    { NULL, NULL }  // End of test suite
  };

  static tCuwSuite TS1 = {
    .reg = { "Test suite #1", NULL, NULL },
    .tests = tests1
  };

  return &TS1;
}

/* Test suite 2
 *------------------------------------------------------------------------------------------------*/

static void test21(void) {
  // Failed!
  CU_ASSERT(cuwCheckStream(stdout, printSomething, "The quick brown fox jumps over the lazy dog!\n"));
  // Success!
  CU_ASSERT(cuwCheckStream(stdout, printSomething, "The quick brown fox jumps over the lazy dog!"));
  // Failed!
  CU_ASSERT(cuwCheckStream(stdout, printSomething, "The quick brown dog jumps over the lazy fox!\n"));
}

tCuwSuite *getTS2() {

  static tCuwTest tests2[] = {
    { "First test of TS2", test21 },
    { NULL, NULL }  // End of test suite
  };

  static tCuwSuite TS2 = {
    .reg = { "Test suite 2", NULL, NULL },
    .tests = tests2
  };

  return &TS2;
}
