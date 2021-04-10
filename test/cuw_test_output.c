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

#include <stdlib.h>
#include <stdio.h>

#define SIMPLE_TEXT     "The quick brown fox jumps over the lazy dog"
#define MULTILINE_TEXT  \
  "Lorem ipsum dolor sit amet, consectetur adipiscing elit.\n" \
  "\tSed non risus.\nSuspendisse lectus tortor, dignissim sit amet, adipiscing nec, ultricies sed, dolor.\n" \
  "Cras elementum ultrices diam.\tMaecenas ligula massa, varius a, semper congue, euismod non, mi.\n" \
  "Proin porttitor, orci nec nonummy molestie, enim est eleifend mi, non fermentum diam nisl sit amet erat.\r" \
  "Duis semper.\n" \
  "Duis arcu massa, scelerisque vitae, consequat in, pretium a, enim. Pellentesque congue.\n" \
  "Ut in risus volutpat libero pharetra tempor. Cras vestibulum bibendum augue. Praesent egestas leo in pede.\n" \
  "Praesent blandit odio eu enim.     Pellentesque sed dui ut augue blandit sodales.\n" \
  "Vestibulum ante ipsum primis in faucibus orci luctus et ultrices posuere cubilia Curae;\tAliquam nibh.\n " \
  "Mauris ac mauris sed pede pellentesque fermentum. Maecenas adipiscing ante non diam sodales hendrerit.\n\t...\n"

static void noPrint(void);
static int testNoPrint(void);

static void simplePrint(void);
static void multiLinePrint(void);
static void simplePrintError(void);
static void multiLinePrintError(void);

static int testSimplePrint(void);
static int testMultiLinePrint(void);
static int testSimplePrintError(void);
static int testMultiLinePrintError(void);

static void simplePrintErr(void);
static void multiLinePrintErr(void);
static void simplePrintErrError(void);
static void multiLinePrintErrError(void);

static int testSimplePrintErr(void);
static int testMultiLinePrintErr(void);
static int testSimplePrintErrError(void);
static int testMultiLinePrintErrError(void);

static void simplePrintStrm(void);
static void multiLinePrintStrm(void);
static void printStrmError1(void);
static void printStrmError2(void);

static int testSimplePrintStrm(void);
static int testMultiLinePrintStrm(void);
static int testPrintStrmError1(void);
static int testPrintStrmError2(void);

tCuwUTest* getOutputSuite(void) {
  static tCuwUTest s[] = {
    { "Check empty printing", testNoPrint },
    { "Print simple characters (stdout)", testSimplePrint },
    { "Print multi-line text (stdout)", testMultiLinePrint },
    { "Detect print error on simple characters (stdout)", testSimplePrintError },
    { "Detect print error on multi-line text (stdout)", testMultiLinePrintError },
    { "Print simple characters (stderr)", testSimplePrintErr },
    { "Print multi-line text (stderr)", testMultiLinePrintErr },
    { "Detect print error on simple characters (stderr)", testSimplePrintErrError },
    { "Detect print error on multi-line text (stderr)", testMultiLinePrintErrError },
    { "Print simple characters (stdout & stderr)", testSimplePrintStrm },
    { "Print multi-line text (stdout & stderr)", testMultiLinePrintStrm },
    { "Detect print error #1 (stdout & stderr)", testPrintStrmError1 },
    { "Detect print error #2 (stdout & stderr)", testPrintStrmError2 },
    { NULL, NULL }
  };
  return s;
}

/* ---------------------------------------------------------------------------------------------- */

static void noPrint(void) {}
static int testNoPrint(void) {
  return cuwCheckStream(stdout, noPrint, NULL)
      && cuwCheckStream(stderr, noPrint, NULL)
      && cuwCheckStdStreams(noPrint, NULL, NULL);
}

/* ---------------------------------------------------------------------------------------------- */

static void simplePrint(void) { fprintf(stdout, SIMPLE_TEXT); }
static int testSimplePrint(void) {
  return cuwCheckStream(stdout, simplePrint, SIMPLE_TEXT);
}

static void multiLinePrint(void) { fprintf(stdout, MULTILINE_TEXT); }
static int testMultiLinePrint(void) {
  return cuwCheckStream(stdout, multiLinePrint, MULTILINE_TEXT);
}

static void simplePrintError(void) { fprintf(stdout, MULTILINE_TEXT); }
static int testSimplePrintError(void) {
  return !cuwCheckStream(stdout, simplePrintError, SIMPLE_TEXT);
}

static void multiLinePrintError(void) { fprintf(stdout, SIMPLE_TEXT); }
static int testMultiLinePrintError(void) {
  return !cuwCheckStream(stdout, multiLinePrintError, MULTILINE_TEXT);
}

/* ---------------------------------------------------------------------------------------------- */

static void simplePrintErr(void) { fprintf(stderr, SIMPLE_TEXT); }
static int testSimplePrintErr(void) {
  return cuwCheckStream(stderr, simplePrintErr, SIMPLE_TEXT);
}

static void multiLinePrintErr(void) { fprintf(stderr, MULTILINE_TEXT); }
static int testMultiLinePrintErr(void) {
  return cuwCheckStream(stderr, multiLinePrintErr, MULTILINE_TEXT);
}

static void simplePrintErrError(void) { fprintf(stderr, MULTILINE_TEXT); }
static int testSimplePrintErrError(void) {
  return !cuwCheckStream(stderr, simplePrintErrError, SIMPLE_TEXT);
}

static void multiLinePrintErrError(void) { fprintf(stderr, SIMPLE_TEXT); }
static int testMultiLinePrintErrError(void) {
  return !cuwCheckStream(stderr, multiLinePrintErrError, MULTILINE_TEXT);
}

/* ---------------------------------------------------------------------------------------------- */

static void simplePrintStrm(void) { fprintf(stdout, SIMPLE_TEXT); fprintf(stderr, SIMPLE_TEXT); }
static int testSimplePrintStrm(void) {
  return cuwCheckStdStreams(simplePrintStrm, SIMPLE_TEXT, SIMPLE_TEXT);
}

static void multiLinePrintStrm(void) { fprintf(stderr, MULTILINE_TEXT); fprintf(stdout, MULTILINE_TEXT); }
static int testMultiLinePrintStrm(void) {
  return cuwCheckStdStreams(multiLinePrintStrm, MULTILINE_TEXT, MULTILINE_TEXT);
}

static void printStrmError1(void) { fprintf(stderr, SIMPLE_TEXT); fprintf(stdout, MULTILINE_TEXT); }
static int testPrintStrmError1(void) {
  return !cuwCheckStdStreams(printStrmError1, SIMPLE_TEXT, MULTILINE_TEXT);
}

static void printStrmError2(void) { fprintf(stdout, SIMPLE_TEXT); fprintf(stderr, MULTILINE_TEXT); }
static int testPrintStrmError2(void) {
  return !cuwCheckStdStreams(printStrmError2, MULTILINE_TEXT, SIMPLE_TEXT);
}
