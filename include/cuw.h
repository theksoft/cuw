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

/**
  @file cuw.h
  @brief CUnit wrapper header
  @author Herve Retaureau
  @copyright Copyright (c) 2019

  @example cuw-basic-example.c
  This example shows the use of the very basic CUnit wrapper interface.

  @example cuw-extended-example.c
  This example shows the use of CUnit wrapper internal interface in order to access some customization.
*/

#ifndef CUW_H
#define CUW_H

#include "CUnit/CUnit.h"
#include "CUnit/Basic.h"
#include "CUnit/Console.h"
#include "CUnit/Automated.h"

#include <stdlib.h>
#include <stdio.h>

/** @defgroup _basic Basic wrapping
    This group includes all the necessary definitions for a basic usage of CUnit wrapper.
    @{
*/

/* Definitions
 ------------------------------------------------------------------------------------------------ */

/** Maximum file path length. */
#define CUW_MAX_PATH    1024

/** Define the CUnit run mode.
    @see tCuwContext, cuwProcess, cuwRunSelected.
*/
typedef enum {
  CUW_MODE_BASIC = 0,
  /**< CUnit <a href="http://cunit.sourceforge.net/doc/running_tests.html#basic">basic</a> run mode. */
  CUW_MODE_CONSOLE,
  /**< CUnit <a href="http://cunit.sourceforge.net/doc/running_tests.html#console">console</a> run mode. */
  CUW_MODE_AUTOMATED
  /**< CUnit <a href="http://cunit.sourceforge.net/doc/running_tests.html#automated">automated</a> run mode. */
} eCuwMode;

/** CUnit wrapper execution context.
    @see cuwProcess, cuwRunSelected
*/
typedef struct {
  eCuwMode mode;
  /**< CUnit run mode. */
  CU_BasicRunMode bm;
  /**< CUnit <a href="http://cunit.sourceforge.net/doc/running_tests.html#basic-setmode">ouput type</a> for basic run mode.
       Can be 0 (unused) when console or automated run mode is selected.
  */
  char filename[CUW_MAX_PATH];
  /**< CUnit <a href="http://cunit.sourceforge.net/doc/running_tests.html#auto-setroot">filename root</a> for CUnit automated run mode.
       Can be NULL (unused) when basic or console run mode is selected.
  */
} tCuwContext;

/** CUnit test definition.
    @see tCuwSuite.
*/
typedef struct {
  const char *title;  /**< Test title. */
  void (*test)(void); /**< Test procedure. */
} tCuwTest;

/** CUnit test suite definition.

    This structure provides the data for:
    + the test suite definition,
    + each test defition belonging to the test suite and packed as a NULL terminated table.

    @see tCuwSuiteGetter, cuwCreateTestSuite.
*/
typedef struct {
  struct {
    const char* title;    /**< Test suite title. */
    int (*init)(void);    /**< Test suite initializer. */
    int (*cleanup)(void); /**< Test suite cleanup function. */
  } reg;
  /**< CUnit test suite <a href="http://cunit.sourceforge.net/doc/managing_tests.html#addsuite">registration</a> specification. */
  tCuwTest *tests;
  /**< Table of CUnit test <a href="http://cunit.sourceforge.net/doc/managing_tests.html#addtest">registration</a> specification.
       The table of test is NULL terminated i.e. must terminate with { NULL, NULL } record.
  */
} tCuwSuite;

/** Function type getting test suite definition.
  This function takes no arguments and return a single test suite specification.
  @see cuwProcess, cuwCreateTests.
*/
typedef tCuwSuite* (*tCuwSuiteGetter)(void);
/** tCuwSuiteGetter table ender. */
#define CUW_SUITE_END     ((tCuwSuiteGetter)0)

/* Basic wrapping
 ------------------------------------------------------------------------------------------------ */

/** Get basic context from comand line.
    @param[out] context   CUnit wrapper execution context.
    @param[in]  argc      Number of arguments.
    @param[in]  argv      Command line arguments.
    @return This function returns the argument parsing result.
    @retval -1 The function failed to parse program arguments.
    @retval  0 The function parses the program arguments successfully.
    @retval  1 The function parses the program arguments successfully but help has been requested.
*/ 
int cuwGetContext(tCuwContext *context, int argc, char *argv[]);


/** Install test defititions and run tests as specified in the context.
    @param[in] context
    Execution context defining how to run tests.
    @param[in] getters
    Table of test suite getters for retrieving all tests suites.
    The last element of this table must be ::CUW_SUITE_END to indicate the end of the table.
    @return
    This function returns 1 on success or 0 if test installation or run failed.
    Note that test failure does not issue run failure.
*/
int cuwProcess(const tCuwContext *context, const tCuwSuiteGetter getters[]);

/** @} */

/* Extended wrapping
 ------------------------------------------------------------------------------------------------ */

/** @defgroup _extended Extended wrapping functions
    This group includes all the functions used by the basic wrapping.
    They can be used when some level of details is needed in test programming.
    @{
*/

/** Parse command line arguments.
    @param[out] context   CUnit wrapper execution context.
    @param[out] help      Help is requested if set to 1, 0 otherwise.
    @param[in]  argc      Number of arguments.
    @param[in]  argv      Command line arguments.
    @return This function returns 1 if successful or 0 if failed.

    This function looks for the following options:
    + [-h]  Display help
    + [-m]  Define the execution mode among BASIC, CONSOLE or AUTOMATED.
    + [-f]  Define the filename for automated execution.
    + Basic run mode is set to verbose by default.
*/
int cuwParseArgs(tCuwContext *context, int *help, int argc, char* argv[]);

/** Display the program basic usage corresponding to the default argument parsing.
    @param[in] command    Command argument having invoked the program.
*/
void cuwUsage(const char *command);

/** Get CUnit <a href="http://cunit.sourceforge.net/doc/error_handling.html#errorhandling">error code</a>. */
static inline CU_ErrorCode cuwGetError(void) { return CU_get_error(); }
/** Get CUnit <a href="http://cunit.sourceforge.net/doc/error_handling.html#errorhandling">error message</a>. */
static inline const char* cuwGetErrorMessage(void) { return CU_get_error_msg(); }

/** CUnit <a href="http://cunit.sourceforge.net/doc/test_registry.html#init">registry initialization</a>.
    @return
    This function returns 1 if successful or 0 if failed.
    Actual CUnit registry initialization error can be retrieved with cuwGetError() and cuwGetErrorMessage().
*/
static inline int cuwInitializeRegistry(void) { return (CUE_SUCCESS == CU_initialize_registry()); }
/** CUnit <a href="http://cunit.sourceforge.net/doc/test_registry.html#cleanup">registry cleanup</a>. */
static inline void cuwCleanupRegistry(void) { CU_cleanup_registry(); }

/** Create a test suite specification.
    @param[in] suite 
    Test suite specification describing the test suite and included test procedures.
    @return
    This function returns 1 if successful or 0 if failed.
    Actual CUnit error can be retrieved with cuwGetError() and cuwGetErrorMessage().
*/
int cuwCreateTestSuite(const tCuwSuite *suite);

/** Create a set of test suite.
    @param[in] getters 
    Table of test suite specification getter.
    Each getter returns a test suite specication that can be provided to cuwCreateTestSuite().
    The last element of this table must be ::CUW_SUITE_END to indicate the end of the table.
    @return
    This function returns 1 if successful or 0 if failed.
    Actual CUnit error can be retrieved with cuwGetError() and cuwGetErrorMessage().
*/
int cuwCreateTests(const tCuwSuiteGetter getters[]);

/** Run CUnit test in <a href="http://cunit.sourceforge.net/doc/running_tests.html#basic">basic mode</a>
    @param[in] bm
    @see CUnit <a href="http://cunit.sourceforge.net/doc/running_tests.html#basic">basic run mode</a>.
    @return
    This function returns 1 if successful or 0 if failed.
    Actual CUnit error can be retrieved with cuwGetError() and cuwGetErrorMessage().
*/
int cuwRunBasic(CU_BasicRunMode bm);

/** Run CUnit test in <a href="http://cunit.sourceforge.net/doc/running_tests.html#console">interactive console mode</a>
    @return
    This function returns 1 if successful or 0 if failed.
    Actual CUnit error can be retrieved with cuwGetError() and cuwGetErrorMessage().
*/
int cuwRunConsole();

/** Run CUnit test in <a href="http://cunit.sourceforge.net/doc/running_tests.html#automated">automated mode</a>
    @param[in] filename
    @see CUnit <a href="http://cunit.sourceforge.net/doc/running_tests.html#automated">automated run mode</a>.
    @return
    This function returns 1 if successful or 0 if failed.
    Actual CUnit error can be retrieved with cuwGetError() and cuwGetErrorMessage().
*/
int cuwRunAutomated(const char* filename);

/** Run CUnit test.
    @param[in] context
    Context defining the CUnit test run mode and related argument if any associated with the mode.
    @return
    This function returns 1 if successful or 0 if failed.
    Actual CUnit error can be retrieved with cuwGetError() and cuwGetErrorMessage().
    @see cuwRunBasic, cuwRunConsole, cuwRunAutomated.
*/
int cuwRunSelected(const tCuwContext* context);

/** @} */

/* Test utilities
 ------------------------------------------------------------------------------------------------ */

/** @defgroup _utils Test utility functions
    This group includes all the functions used by the basic wrapping.
    They can be used when some level of details is needed in test programming.
    @{
*/

#define CUW_CHECK_OUTPUT(t, o)      CU_ASSERT(testCheckStdOut((t), (o)))
#define CUW_CHECK_ERROR(t, e)       CU_ASSERT(testCheckStdErr((t), (e)))
#define CUW_CHECK_STREAMS(t, o, e)  CU_ASSERT(testCheckStdStreams((t), (o), (e)))

/** Compares a function output to a stream to an expected text string.
    @param[inout]  s
    Stream
    @param[in] fProc 
    Function that output text to stream.
    @param[in] expected 
    Expected output text.
    @return
    This function returns 1 if the function fTest() output to stream is equals to expected.
*/
int cuwCheckStream(FILE* s, void (*fProc)(), const char *expected);

/** Compares a function output to stdout && stderr to a expected text strings.
    @param[in] fProc 
    Function that output text to stdout && stderr stream.
    @param[in] expectedOut
    Expected output text.
    @param[in] expectedErr
    Expected output error text.
    @return
    This function returns 1 if the function fTest() output to stdout && stderr is equals to resp. expected.
*/
int cuwCheckStdStreams(void (*fProc)(), const char *expectedOut, const char *expectedErr);

/** @} */

#endif  // CUW_H
