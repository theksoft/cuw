# CUW - CUnit wrapper

CUnit wrapper goal is to reduce the API call for using CUnit in most basic usage of this library.
*cuw.h* interface defines:

+ a very basic interface composed of few structures and 2 functions:
  - *__cuwGetArgs()__* is a function parsing arguments for a basic test application including help, 
    test run mode and a file root name for automated run mode. Test run modes are:
    + *__CUW_MODE_BASIC__* is the CUnit basic mode configured by default in max verbose mode.
      Results are printed on stdout stream.
    + *__CUW_MODE_CONSOLE__* is the CUnit interactive console mode which let you select test to run.
    + *__CUW_MODE_AUTOMATED__* is the CUnit automated mode which writes test results in a file named
      as the provide file root name extended with *-result.log* resulting with *\<filerootname\>-result.log*.
      This mode is especially convenient for continuous integration systems.
  - *__cuwProcess()__* processes a provided test specification.
+ a more detailed interface which is in fact the CUW internal internal exposed for those needing
  to customize a bit more the CUnit execution.
  If more detailed interfacing is needed, you should go directly with CUnit interface.

Two example files are provided:
+ *cuw-basic-example.c* shows the use of the very basic CUW interface.
+ *cuw-extended-example.c* shows the use of the more detailed interface with some customization.

# Generating libcuw.a

In order to generate a *libcuw.a*, you'll need to have:
+ a CUnit installation and the resulting *libcunit.a* accessible to the C compiler
+ a system supporting getopt feature and *getopt.h* header accessible to the C compiler

# Genrating doxygen documentation

*Doxygen* HTML documentation can be generated using *cuw-dox.cfg* configuration file.

# Test file organization example

Define a test file by test suite containing - for example a <testSuite> in <testSuite.c>:

    #include "<tests>.h"

+ test procedures which prototype should be implemented as:

        void <test>(void) {
          // test something
        }

+ The test table definition for the test suite terminated by a NULL record:

        static tCuwTest <tests>[] = {
          { "testTitle#1", <test1> },
          { "testTitle#2", <test2> },
          { NULL, NULL }  // End of test table
        };

+ The test suite definition - initialize and cleanup may be NULL when nothing to initialize or cleanup:

        static tCuwSuite <testSuite> = {
          .reg = { "testSuiteTitle", <testSuiteInit>, <testSuiteCleanup> },
          .tests = <tests>
        };

+ The test suite suite getter - test and test suite definition may be included within:

        tCuwSuite *<getTestSuite>() {
          return &<testSuite>;
        }

Define a common test header file <tests.h> declaring all test suite getters:

    #include "cuw.h"

    tCuwSuite *<getTestSuite>();
    // Additional test suite getter declaration

In the main test application <main.c>, define the test suite table and use it the very basic way:

    #include "<tests>.h"

    static tCuwSuiteGetter <testSuites>[] = {
      <getTestSuite>,
      // Additional test suite getter reference
      CUW_SUITE_END     // End of test suite
    };

    int main(int argc, char *argv[]) {
      int e = 0;
      tCuwContext c;
      if (0 != (e = cuwGetContext(&c, argc, argv)))
        return (1 == e) ? EXIT_SUCCESS : EXIT_FAILURE;
      return (!cuwProcess(&c, <testSuites>)) ? EXIT_FAILURE : EXIT_SUCCESS;
    }
