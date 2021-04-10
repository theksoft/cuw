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

#include "cuw_test.h"

#undef CUW_CAN_CHECK_CUNIT_OUT

static void test11(void);
static void test12(void);
static tCuwSuite *getTS1();
static void test21(void);
static tCuwSuite *getTS2();
static tCuwSuiteGetter tests[] = { getTS1, getTS2, CUW_SUITE_END };

// Interactive console not tested automatically as it requires ... interaction
static int processBasicMode(void);
static int processAutomatedMode(void);
static int processPostProcess(void);

tCuwUTest* getTestsSuite(void) {
  static tCuwUTest s[] = {
    { "Check basic mode entry (N/A)", processBasicMode },
    { "Check automated mode entry", processAutomatedMode },
    { "Check post-processing (automated)", processPostProcess },
    { NULL, NULL }
  };
  return s;
}

/* BASIC MODE
 *------------------------------------------------------------------------------------------------*/

#define TEST_RESULT \
  "\n\n     CUnit - A unit testing framework for C - Version 2.1-3\n" \
  "     http://cunit.sourceforge.net/\n\n\n" \
  "Suite: Test suite #1\n" \
  "  Test: TS#1 - Test #1 ...FAILED\n" \
  "    1. test/cuw_test_tests.c:68  - 3 == myAddition(2, 2)\n" \
  "  Test: TS#1 - Test #2 ...passed\n" \
  "Suite: Test suite 2\n" \
  "  Test: First test of TS2 ...passed\n\n" \
  "Run Summary:    Type  Total    Ran Passed Failed Inactive\n" \
  "              suites      2      2    n/a      0        0\n" \
  "               tests      3      3      2      1        0\n" \
  "             asserts     11     11     10      1      n/a\n\n" \
  "Elapsed time =    0.000 seconds\n"

static void processBasicModeTest(void) {
  tCuwContext c = { .mode = CUW_MODE_BASIC, .bm = CU_BRM_VERBOSE };
  if (!cuwProcess(&c, tests, NULL)) {
    fprintf(stderr, "ERROR executing predefined tests\n");
    fprintf(stdout, ".\n");   // For comparison to fail
  }
}

static int processBasicMode(void) {
  // Cannot test based on stdout as CUnit implementation may use a "different" stream implementation.
#ifdef CUW_CAN_CHECK_CUNIT_OUT
  return cuwCheckStream(stdout, processBasicModeTest, TEST_RESULT);
#else
  (void)processBasicModeTest;
  return 1;
#endif
}

/* AUTOMATED MODE
 *------------------------------------------------------------------------------------------------*/

#define CUW_TEST_ROOT   "test"
#define CUW_TEST_FN     CUW_TEST_ROOT"-Results.xml"

static int checkTestFile(const char *fn);

static int processAutomatedMode(void) {
  int rtn = 1;
  tCuwContext c = { .mode = CUW_MODE_AUTOMATED, .filename = CUW_TEST_ROOT };
  if (!cuwProcess(&c, tests, NULL)) {
    fprintf(stderr, "ERROR executing predefined tests\n");
    rtn = 0;
  }
  if (!checkTestFile(CUW_TEST_FN)) {
    fprintf(stderr, "ERROR incorrect expected test file\n");
    rtn = 0;
  }
  return rtn;
}

/* AUTOMATED MODE with POST-PROCESSING
 *------------------------------------------------------------------------------------------------*/

static void postProcess(const tCuwContext *context) {
  (void)context;
  if (1 != CU_get_number_of_tests_failed())
    fprintf(stderr, "Warning: Unexpected number of test failed (%d)\n", CU_get_number_of_tests_failed());
  if (1 != CU_get_number_of_failures())
    fprintf(stderr, "Warning: Unexpected number of failures (%d)\n", CU_get_number_of_failures());
}

static int processPostProcess(void) {
  int rtn = 1;
  tCuwContext c = { .mode = CUW_MODE_AUTOMATED, .filename = CUW_TEST_ROOT };
  if (!cuwProcess(&c, tests, postProcess)) {
    fprintf(stderr, "ERROR executing predefined tests\n");
    rtn = 0;
  }
  if (!checkTestFile(CUW_TEST_FN)) {
    fprintf(stderr, "ERROR incorrect expected test file\n");
    rtn = 0;
  }
  return rtn;
}

/* Dummy implementation of something we want to test - we'll use it in both test suites
 *------------------------------------------------------------------------------------------------*/

int myAddition(int a, int b) {
  return a + b;
}

int mySubstraction(int a, int b) {
  return a - b;
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

static tCuwSuite *getTS1() {

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
  CU_ASSERT(0 == mySubstraction(1, 1));
  CU_ASSERT(0 == mySubstraction(2, 2));   // Just to have some test failed reported
  CU_ASSERT(-1 == mySubstraction(2, 3));
  CU_ASSERT(3 == mySubstraction(10, 7));
  CU_ASSERT(5 == mySubstraction(5, 0));
}

static tCuwSuite *getTS2() {

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

/* Check expected test file report
 *------------------------------------------------------------------------------------------------*/

#define DAT1 \
    "<?xml version=\"1.0\" ?> \n" \
    "<?xml-stylesheet type=\"text/xsl\" href=\"CUnit-Run.xsl\" ?> \n" \
    "<!DOCTYPE CUNIT_TEST_RUN_REPORT SYSTEM \"CUnit-Run.dtd\"> \n" \
    "<CUNIT_TEST_RUN_REPORT> \n" \
    "  <CUNIT_HEADER/> \n" \
    "  <CUNIT_RESULT_LISTING> \n" \
    "    <CUNIT_RUN_SUITE> \n" \
    "      <CUNIT_RUN_SUITE_SUCCESS> \n" \
    "        <SUITE_NAME> Test suite #1 </SUITE_NAME> \n" \
    "        <CUNIT_RUN_TEST_RECORD> \n" \
    "          <CUNIT_RUN_TEST_FAILURE> \n" \
    "            <TEST_NAME> TS#1 - Test #1 </TEST_NAME> \n" \
    "            <FILE_NAME> test/cuw_test_tests.c </FILE_NAME> \n" \
    "            <LINE_NUMBER> 150 </LINE_NUMBER> \n" \
    "            <CONDITION> 3 == myAddition(2, 2) </CONDITION> \n" \
    "          </CUNIT_RUN_TEST_FAILURE> \n" \
    "        </CUNIT_RUN_TEST_RECORD> \n" \
    "        <CUNIT_RUN_TEST_RECORD> \n" \
    "          <CUNIT_RUN_TEST_SUCCESS> \n" \
    "            <TEST_NAME> TS#1 - Test #2 </TEST_NAME> \n" \
    "          </CUNIT_RUN_TEST_SUCCESS> \n" \
    "        </CUNIT_RUN_TEST_RECORD> \n" \
    "      </CUNIT_RUN_SUITE_SUCCESS> \n" \
    "    </CUNIT_RUN_SUITE> \n" \
    "    <CUNIT_RUN_SUITE> \n" \
    "      <CUNIT_RUN_SUITE_SUCCESS> \n" \
    "        <SUITE_NAME> Test suite 2 </SUITE_NAME> \n" \
    "        <CUNIT_RUN_TEST_RECORD> \n" \
    "          <CUNIT_RUN_TEST_SUCCESS> \n" \
    "            <TEST_NAME> First test of TS2 </TEST_NAME> \n" \
    "          </CUNIT_RUN_TEST_SUCCESS> \n" \
    "        </CUNIT_RUN_TEST_RECORD> \n" \
    "      </CUNIT_RUN_SUITE_SUCCESS> \n" \
    "    </CUNIT_RUN_SUITE> \n" \
    "  </CUNIT_RESULT_LISTING>\n" \
    "  <CUNIT_RUN_SUMMARY> \n" \
    "    <CUNIT_RUN_SUMMARY_RECORD> \n" \
    "      <TYPE> Suites </TYPE> \n" \
    "      <TOTAL> 2 </TOTAL> \n" \
    "      <RUN> 2 </RUN> \n" \
    "      <SUCCEEDED> - NA - </SUCCEEDED> \n" \
    "      <FAILED> 0 </FAILED> \n" \
    "      <INACTIVE> 0 </INACTIVE> \n" \
    "    </CUNIT_RUN_SUMMARY_RECORD> \n" \
    "    <CUNIT_RUN_SUMMARY_RECORD> \n" \
    "      <TYPE> Test Cases </TYPE> \n" \
    "      <TOTAL> 3 </TOTAL> \n" \
    "      <RUN> 3 </RUN> \n" \
    "      <SUCCEEDED> 2 </SUCCEEDED> \n" \
    "      <FAILED> 1 </FAILED> \n" \
    "      <INACTIVE> 0 </INACTIVE> \n" \
    "    </CUNIT_RUN_SUMMARY_RECORD> \n" \
    "    <CUNIT_RUN_SUMMARY_RECORD> \n" \
    "      <TYPE> Assertions </TYPE> \n" \
    "      <TOTAL> 11 </TOTAL> \n" \
    "      <RUN> 11 </RUN> \n" \
    "      <SUCCEEDED> 10 </SUCCEEDED> \n" \
    "      <FAILED> 1 </FAILED> \n" \
    "      <INACTIVE> n/a </INACTIVE> \n" \
    "    </CUNIT_RUN_SUMMARY_RECORD> \n" \
    "  </CUNIT_RUN_SUMMARY> \n" \
    "  <CUNIT_FOOTER> File Generated By CUnit v2.1-3 - "

#define DAT2 \
    "Wed Dec  4 22:29:36 2019"

#define DAT3 \
    "\n" \
    " </CUNIT_FOOTER> \n" \
    "</CUNIT_TEST_RUN_REPORT>"

static int checkTestFile(const char *fn) {
  int rtn = 1, rm = 0;
  FILE *f = NULL;

  if (NULL == (f = fopen(fn, "r+b")))
    return 0;

  rm = 1;
  fseek(f, 0, SEEK_END);
  size_t expLength = strlen(DAT1)+strlen(DAT2)+strlen(DAT3), length = (size_t)ftell(f);
  char* data = NULL;
  if (NULL == (data = malloc(length+1))) {
    fprintf(stderr, "ERROR internal memory allocation error!");
    rtn = 0;
  } else {
    memset(data, 0, length+1);
    fseek(f, 0, SEEK_SET);
    if (
      length != fread(data, 1, length, f) ||
      expLength != length ||
      0 != memcmp(data, DAT1, strlen(DAT1)) ||
      0 != memcmp(data+strlen(DAT1)+strlen(DAT2), DAT3, strlen(DAT3))
    ) {
      rtn = 0;
    }
  }
  fclose(f);

  if (rm)
    remove(fn);
  return rtn;
}

