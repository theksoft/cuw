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

#include <string.h>
#include <assert.h>
#include <getopt.h>

/* Basic wrapping
 *----------------------------------------------------------------------------------------------- */

int cuwGetContext(tCuwContext *context, int argc, char *argv[]) {
  if (!context) return -1;
  assert(argc && argv);
  int help = 0;
  if (!cuwParseArgs(context, &help, argc, argv) || help) {
    cuwUsage(argv[0]);
    return (help) ? 1 : -1;
  }
  return 0;
}

int cuwProcess(
  const tCuwContext* context,
  const tCuwSuiteGetter getters[],
  const tCuwPostProcess postProcess
) {
  if (!context || !getters) return 0;
  if (!cuwInitializeRegistry()) {
    fprintf(stderr, "ERROR(%d) %s\n", cuwGetError(), cuwGetErrorMessage());
    return 0;
  }
  if ( !cuwCreateTests(getters) || !cuwRunSelected(context)) {
    cuwCleanupRegistry();
    fprintf(stderr, "ERROR(%d) %s\n", cuwGetError(), cuwGetErrorMessage());
    return 0;
  }
  if (postProcess)
    (*postProcess)(context);
  cuwCleanupRegistry();
  return 1;
}

/* Extended wrapping - Command line parsing
 *----------------------------------------------------------------------------------------------- */

#define CUW_FILENAME    "\0"

int cuwParseArgs(tCuwContext *context, int *help, int argc, char* argv[]) {
  assert(help && context);
  *help = 0;
  context->mode = CUW_MODE_BASIC;
  context->bm = CU_BRM_VERBOSE;
  memset(&context->filename[0], 0, CUW_MAX_PATH);

  int c, rtn = 1;
  while (-1 != rtn && -1 != (c = getopt (argc, argv, "hm:f:"))) {
    switch (c) {
    case 'h':
      *help = 1;
      break;
    case 'm':
      if      (0 == strcmp("BASIC", optarg))      context->mode = CUW_MODE_BASIC;
      else if (0 == strcmp("CONSOLE", optarg))    context->mode = CUW_MODE_CONSOLE;
      else if (0 == strcmp("AUTOMATED", optarg))  context->mode = CUW_MODE_AUTOMATED;
      else {
        rtn = 0;
        fprintf(stderr, "%s is invalid for m option.\n", optarg);
      }
      break;
    case 'f':
      if (CUW_MAX_PATH > strlen(optarg))
        strncpy(&context->filename[0], optarg, CUW_MAX_PATH-1);
      break;
    case '?':
      if (optopt == 'm' || optopt == 'f')
        fprintf (stderr, "Option -%c requires an argument.\n", optopt);
      else
        fprintf (stderr, "Unknown option '-%c'.\n", optopt);
      rtn = 0;
      break;
    default:
      fprintf(stderr, "Parsing error!\n");
      rtn = 0;
      break;
    }
  }
  return rtn;
}

void cuwUsage(const char *command) {
  fprintf(stdout, "\nUsage: %s [options]\nOptions:\n", command);
  fprintf(stdout, "  -m <mode>      Mode for running test: BASIC, CONSOLE or AUTOMATED\n");
  fprintf(stdout, "  -f <filepath>  <filepath> for automated test (default is \"./result.log\")\n");
  fprintf(stdout, "  -h             Display this help and exit\n\n");
}

/* Extended wrapping - Test and test suite management
 *----------------------------------------------------------------------------------------------- */

int cuwCreateTests(const tCuwSuiteGetter getters[]) {
  assert(getters);
  int rtn = 1;
  for (int i = 0; getters[i]; i++) {
    if (!cuwCreateTestSuite((getters[i])())) {
      rtn = 0;
      break;
    }
  }
  return rtn;
}

int cuwCreateTestSuite(const tCuwSuite *suite) {
  assert(suite && suite->reg.title && suite->tests);
  CU_pSuite ps = NULL;
  if (NULL == (ps = CU_add_suite(suite->reg.title, suite->reg.init, suite->reg.cleanup)))
    return 0;
  for (tCuwTest *t = suite->tests; t->title && t->test; t++) {
    if (NULL == CU_add_test(ps, t->title, t->test))
      return 0;
  }
  return 1;
}

/* Extended wrapping - Test run management
 *----------------------------------------------------------------------------------------------- */

int cuwRunSelected(const tCuwContext* context) {
  assert(context && (CUW_MODE_AUTOMATED != context->mode || context->filename[0]));
  int rtn = 1;
  switch(context->mode) {
    case CUW_MODE_BASIC:      cuwRunBasic(context->bm); break;
    case CUW_MODE_CONSOLE:    cuwRunConsole(); break;
    case CUW_MODE_AUTOMATED:  cuwRunAutomated(context->filename); break;
    default: rtn = 0; break;
  }
  return rtn;
}

int cuwRunBasic(CU_BasicRunMode bm) {
  CU_basic_set_mode(bm);
  CU_basic_run_tests();
  return (CUE_SUCCESS == CU_get_error());
}

int cuwRunConsole() {
  CU_console_run_tests();
  return (CUE_SUCCESS == CU_get_error());
}

int cuwRunAutomated(const char* filename) {
  assert(filename);
  CU_set_output_filename(filename);
  CU_automated_run_tests();
  return (CUE_SUCCESS == CU_get_error());
}

/* Utils
 *----------------------------------------------------------------------------------------------- */

static char* cuwBufferStream(FILE *s, size_t lm) {
  assert(s && lm);
  // Set a buffer with a minimal size of 8192 or a multiple of 8192
  lm >>= 13; lm++; lm <<= 13;
  char *buf = malloc(lm);
  memset(buf, 0, lm);
  fflush(s);
  setvbuf(s, buf, _IOFBF, lm);
  fseek(s, 0, SEEK_SET);
  return buf;
}

static void cuwResetStream(FILE *s, char* buf, size_t lm) {
  assert(s && buf && lm);
  lm >>= 13; lm++; lm <<= 13;
  memset(buf, 0, lm);
  fseek(s, 0, SEEK_SET);
  setvbuf(s, NULL, _IONBF, 0);
  free(buf);
}

int cuwCheckStream(FILE *s, void (*fProc)(), const char *expected) {
  if (!s || !fProc)  return 0;   // Bad arguments
  size_t ln = (expected && 0 != strlen(expected)) ? strlen(expected) : 1;
  char *buf = cuwBufferStream(s, 2*ln);
  fProc();
  int rtn = (expected) ? (0 == buf[ln]) && (0 == memcmp(expected, buf, ln) && ln == strlen(buf)) : (0 == strlen(buf));
  cuwResetStream(s, buf, 2*ln);
  return rtn;
}

int cuwCheckStdStreams(void (*fProc)(), const char *expectedOut, const char *expectedErr) {
  if (!fProc)  return 0;   // Bad arguments
  size_t lno = (expectedOut && 0 != strlen(expectedOut)) ? strlen(expectedOut) : 1,
         lne = (expectedErr && 0 != strlen(expectedErr)) ? strlen(expectedErr) : 1;
  char *bufo = cuwBufferStream(stdout, 2*lno),
       *bufe = cuwBufferStream(stderr, 2*lne);
  fProc();
  int rtn = ((expectedOut) ? (0 == bufo[lno]) && (0 == memcmp(expectedOut, bufo, lno) && lno == strlen(bufo)) : (0 == strlen(bufo)))
         && ((expectedErr) ? (0 == bufe[lne]) && (0 == memcmp(expectedErr, bufe, lne) && lne == strlen(bufe)) : (0 == strlen(bufe)));
  cuwResetStream(stderr, bufe, 2*lne);
  cuwResetStream(stdout, bufo, 2*lno);
  return rtn;
}
