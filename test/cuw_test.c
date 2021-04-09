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

static tCuwUTest* (*suites[])(void) = {
  getOutputSuite, getArgsSuite, getTestsSuite,
  0
};

int main(int argc, char *argv[]) {
  (void)argc; (void)argv;
  for (int i = 0; suites[i]; i++) {
    fprintf(stdout,"\n>>> Executing test suite #%d\n", i+1);
    tCuwUTest* tests = (*(suites[i]))();;
    for(int j = 0; tests[j].title && tests[j].test; j++) {
      if((*(tests[j].test))())
        fprintf(stdout, "  [SUCCESS] %s\n", tests[j].title);
      else
        fprintf(stdout, "\x1b[1;31m  [FAILED ] %s\x1b[0m\n", tests[j].title);
    }
    fprintf(stdout,">>> Done with test suite #%d\n", i+1);
  }
  fprintf(stdout, "\n");
  return EXIT_SUCCESS;
}
