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

#include "cuw-test.h"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <getopt.h>

#define CMD     "TEST"
#define USAGE   \
  "\nUsage: "CMD" [options]\nOptions:\n" \
  "  -m <mode>      Mode for running test: BASIC, CONSOLE or AUTOMATED\n" \
  "  -f <filepath>  <filepath> for automated test (default is \"./result.log\")\n" \
  "  -h             Display this help and exit\n\n"

static void resetGetopt() {
  optind = 1;
}

static int testCallNullArgs(void);
static int testCallNoArgs(void);
static int testCallHelpArgs(void);
static int testCallBadArgs(void);
static int testCallArgs(void);

tCuwUTest* getArgsSuite(void) {
  static tCuwUTest s[] = {
    { "Call with NULL argument", testCallNullArgs },
    { "Call with no argument", testCallNoArgs },
    { "Call with help argument", testCallHelpArgs },
    { "Call with bad arguments", testCallBadArgs },
    { "Call with expected arguments", testCallArgs },
    { NULL, NULL }
  };
  return s;
}

/* ---------------------------------------------------------------------------------------------- */

static int testCallNullArgs(void) {
  int argc = 1; char *argv[] = { CMD };
  resetGetopt();
  if (-1 != cuwGetContext(NULL, argc, argv))  return 0;
  return 1;
}

/* ---------------------------------------------------------------------------------------------- */

static int testCallNoArgs(void) {
  int argc = 1; char *argv[] = { CMD };
  tCuwContext c;
  resetGetopt();
  if (0 != cuwGetContext(&c, argc, argv))  return 0;
  if (0 != c.filename[0]) return 0;
  if (CUW_MODE_BASIC != c.mode) return 0;
  if (CU_BRM_VERBOSE != c.bm) return 0;
  return 1;
}

/* ---------------------------------------------------------------------------------------------- */

static void helpArgsCall(void) {
  int argc = 2; char *argv[] = { CMD, "-h" };
  tCuwContext c;
  resetGetopt();
  if (1 != cuwGetContext(&c, argc, argv)) {
    fprintf(stderr, "ERROR with help command line\n");
    fprintf(stdout, ".\n");   // For comparison to fail
  }
}
static void helpAndMoreArgsCall(void) {
  int argc = 3; char *argv[] = { CMD, "-mBASIC", "-h" };
  tCuwContext c;
  resetGetopt();
  if (1 != cuwGetContext(&c, argc, argv)) {
    fprintf(stderr, "ERROR with help command line\n");
    fprintf(stdout, ".\n");   // For comparison to fail
  }
}
static void helpAndMoreArgsCall2(void) {
  int argc = 4; char *argv[] = { CMD, "-m", "BASIC", "-h" };
  tCuwContext c;
  resetGetopt();
  if (1 != cuwGetContext(&c, argc, argv)) {
    fprintf(stderr, "ERROR with help command line\n");
    fprintf(stdout, ".\n");   // For comparison to fail
  }
}
static int testCallHelpArgs(void) {
  return cuwCheckStream(stdout, helpArgsCall, USAGE)
      && cuwCheckStream(stdout, helpAndMoreArgsCall, USAGE)
      && cuwCheckStream(stdout, helpAndMoreArgsCall2, USAGE);
}

/* ---------------------------------------------------------------------------------------------- */

#define BAD_MODE  "AUTO is invalid for m option.\n"

static void badModeCall(void) {
  int argc = 2; char *argv[] = { CMD, "-mAUTO" };
  tCuwContext c;
  resetGetopt();
  if (-1 != cuwGetContext(&c, argc, argv)) {
    fprintf(stderr, "ERROR with missing mode command line\n");
    fprintf(stdout, ".\n");   // For comparison to fail
  }
}

#define MISS_MODE \
  "TEST: option requires an argument -- 'm'\n" \
  "Option -m requires an argument.\n"

static void missingModeCall(void) {
  int argc = 2; char *argv[] = { CMD, "-m" };
  tCuwContext c;
  resetGetopt();
  if (-1 != cuwGetContext(&c, argc, argv)) {
    fprintf(stderr, "ERROR with missing mode command line\n");
    fprintf(stdout, ".\n");   // For comparison to fail
  }
}

#define MISS_FILE \
  "TEST: option requires an argument -- 'f'\n" \
  "Option -f requires an argument.\n"

static void missingFileCall(void) {
  int argc = 2; char *argv[] = { CMD, "-f" };
  tCuwContext c;
  resetGetopt();
  if (-1 != cuwGetContext(&c, argc, argv)) {
    fprintf(stderr, "ERROR with missing file command line\n");
    fprintf(stdout, ".\n");   // For comparison to fail
  }
}

#define INVALID_OPTION \
  "TEST: invalid option -- 'o'\n" \
  "Unknown option '-o'.\n"

static void unknownOptionCall(void) {
  int argc = 3; char *argv[] = { CMD, "-o", "myOption" };
  tCuwContext c;
  resetGetopt();
  if (-1 != cuwGetContext(&c, argc, argv)) {
    fprintf(stderr, "ERROR with unknown option command line\n");
    fprintf(stdout, ".\n");   // For comparison to fail
  }
}

static int testCallBadArgs(void) {
  return cuwCheckStdStreams(badModeCall, USAGE, BAD_MODE)
      && cuwCheckStdStreams(missingModeCall, USAGE, MISS_MODE)
      && cuwCheckStdStreams(missingFileCall, USAGE, MISS_FILE)
      && cuwCheckStdStreams(unknownOptionCall, USAGE, INVALID_OPTION);
}

/* ---------------------------------------------------------------------------------------------- */

static int testCallArgs(void) {
  tCuwContext c;

  char *argv1[] = { CMD, "-mBASIC" };
  resetGetopt();
  if (0 != cuwGetContext(&c, 2, argv1))  return 0;
  if (0 != c.filename[0]) return 0;
  if (CUW_MODE_BASIC != c.mode) return 0;
  if (CU_BRM_VERBOSE != c.bm) return 0;

  char *argv2[] = { CMD, "-m", "CONSOLE" };
  resetGetopt();
  if (0 != cuwGetContext(&c, 3, argv2))  return 0;
  if (0 != c.filename[0]) return 0;
  if (CUW_MODE_CONSOLE != c.mode) return 0;
  if (CU_BRM_VERBOSE != c.bm) return 0;

  char *argv3[] = { CMD, "-m", "AUTOMATED" };
  resetGetopt();
  if (0 != cuwGetContext(&c, 3, argv3))  return 0;
  if (0 != c.filename[0]) return 0;
  if (CUW_MODE_AUTOMATED != c.mode) return 0;
  if (CU_BRM_VERBOSE != c.bm) return 0;

  char *argv4[] = { CMD, "-m", "BASIC", "-f", "TeSt" };
  resetGetopt();
  if (0 != cuwGetContext(&c, 5, argv4))  return 0;
  if (0 != strcmp(c.filename, "TeSt")) return 0;
  if (CUW_MODE_BASIC != c.mode) return 0;
  if (CU_BRM_VERBOSE != c.bm) return 0;

  char *argv5[] = { CMD, "-mAUTOMATED", "-fmyTest" };
  resetGetopt();
  if (0 != cuwGetContext(&c, 3, argv5))  return 0;
  if (0 != strcmp(c.filename, "myTest")) return 0;
  if (CUW_MODE_AUTOMATED != c.mode) return 0;
  if (CU_BRM_VERBOSE != c.bm) return 0;

  return 1;
}
