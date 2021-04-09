# -----------------------------------------------------------------------------
# MAKEFILE project
# -----------------------------------------------------------------------------
# Project targets:
# + libcuw:               CUnit wrapper library.
# + cuw-test:             Test application for libcuw.
# + cuw-basic-example:    Example using libcuw basic wrapping.
# + cuw-extended-example: Example using libcuw extended wrapping.
# -----------------------------------------------------------------------------

PLATFORM ?= linux
PRFX ?= .
TGT = cuw
ifneq "$(PLATFORM)" "win"
  EXE := 
else
  EXE := .exe
endif

# Project directory management
# Include header & source directories as well as temporary build directories.

INCD := include
SRCD := source
OBJD := obj
LIBD := lib
BIND := bin
TSTD := test
DOCD := doc

DIRS := $(OBJD) $(LIBD) $(BIND)

vpath %.h $(INCD):$(SRCD):$(TSTD)
vpath %.hpp $(INCD):$(SRCD):$(TSTD)
vpath %.cpp $(SRCD):$(TSTD)
vpath %.c $(SRCD):$(TSTD)
vpath %.o $(OBJD)/$(PLATFORM)
vpath %.a $(LIBD)/$(PLATFORM)
vpath %$(EXE) $(BIND)/$(PLATFORM)

# Project source file list

SRC := $(TGT)
OBJS := $(SRC:%=%.o)
OBJSD := $(SRC:%=%-g.o)

# Project test file list

SRCT := $(TGT)_test $(TGT)_test_output $(TGT)_test_args $(TGT)_test_tests
OBJST := $(SRCT:%=%-g.o)

# Compiler and linker options

INCLUDES := $(INCD)

CFLAGS := -Wall -Wextra -Werror -Wpedantic -pedantic-errors -fPIC
ARFLAGS := rcs
LDFLAGS := -fPIC -l cunit -L $(LIBD)/$(PLATFORM)

# Main label

all: dirs lib$(TGT).a
tst: dirs $(TGT)-test$(EXE)
xmp: dirs $(TGT)-basic-example$(EXE) $(TGT)-extended-example$(EXE)
doc: $(DOCD)/html/index.html

# Project file dependencies

$(OBJD)/$(PLATFORM)/$(TGT).o $(OBJD)/$(PLATFORM)/$(TGT)-g.o: $(TGT).c $(TGT).h
$(OBJD)/$(PLATFORM)/$(TGT)_test-g.o \
	$(OBJD)/$(PLATFORM)/$(TGT)_test_output-g.o \
	$(OBJD)/$(PLATFORM)/$(TGT)_test_args-g.o \
	$(OBJD)/$(PLATFORM)/$(TGT)_test_tests-g.o: $(TGT)_test.h $(TGT).h
$(OBJD)/$(PLATFORM)/$(TGT)_test_g.o: $(TGT)_test.c
$(OBJD)/$(PLATFORM)/$(TGT)_test_output-g.o: $(TGT)_test_output.c
$(OBJD)/$(PLATFORM)/$(TGT)_test_args-g.o: $(TGT)_test_args.c
$(OBJD)/$(PLATFORM)/$(TGT)_test_tests-g.o: $(TGT)_test_tests.c
$(OBJD)/$(PLATFORM)/$(TGT)_basic_example.o: $(TGT)_basic_example.c $(TGT).h
$(OBJD)/$(PLATFORM)/$(TGT)_extended_example.o: $(TGT)_extended_example.c $(TGT).h

# Project files build rules

$(OBJD)/$(PLATFORM)/%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES:%=-I %) -o2 -c $< -o $@

$(OBJD)/$(PLATFORM)/%-g.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES:%=-I %) -Itest -D_DEBUG -g -o0 -c $< -o $@

# Project targets build

$(LIBD)/$(PLATFORM)/lib$(TGT).a: $(OBJS)
	@echo ==== Building $@ [lib$(TGT)] ====
	$(AR) $(ARFLAGS) $@ $^
	@echo =*_*= Done [$@] =*_*=
	@echo

$(LIBD)/$(PLATFORM)/lib$(TGT)d.a: $(OBJSD)
	@echo ==== Building $@ [lib$(TGT)] ====
	$(AR) $(ARFLAGS) $@ $^
	@echo =*_*= Done [$@] =*_*=
	@echo

$(BIND)/$(PLATFORM)/$(TGT)-test$(EXE): lib$(TGT)d.a
$(BIND)/$(PLATFORM)/$(TGT)-test$(EXE): $(OBJST)
	@echo ==== Building $@ [$(TGT) test application] ====
	$(CXX) $(LDFLAGS) $(filter %.o,$^) -l $(TGT)d -o $@
	@echo =*_*= Done [$@] =*_*=
	@echo

$(BIND)/$(PLATFORM)/$(TGT)-basic-example$(EXE) $(BIND)/$(PLATFORM)/$(TGT)-extended-example$(EXE): lib$(TGT).a

$(BIND)/$(PLATFORM)/$(TGT)-basic-example$(EXE): $(TGT)_basic_example.o
	@echo ==== Building $@ [$(TGT) basic example application] ====
	$(CXX) $(LDFLAGS) $(filter %.o,$^) -l $(TGT) -o $@
	@echo =*_*= Done [$@] =*_*=
	@echo

$(BIND)/$(PLATFORM)/$(TGT)-extended-example$(EXE): $(TGT)_extended_example.o
	@echo ==== Building $@ [$(TGT) extended example application] ====
	$(CXX) $(LDFLAGS) $(filter %.o,$^) -l $(TGT) -o $@
	@echo =*_*= Done [$@] =*_*=
	@echo

# Documentation

$(DOCD)/html/index.html: $(TGT)_dox.cfg $(TGT).h
	@echo ==== Building $(TGT) documentation ====
	doxygen $<
	@echo =*_*= Done [$@] =*_*=
	@echo

# Other project management label

dirs: $(DIRS:%=%/$(PLATFORM))
$(DIRS:%=%/$(PLATFORM)):
	@for dir in $(DIRS); do \
	  mkdir -p $$dir/$(PLATFORM); \
	done

clean:
	@$(RM) $(foreach dir,$(DIRS),$(dir)/$(PLATFORM)/*)
	@$(RM) -r $(DOCD)/*

cleanall:
	@$(RM) -r $(DIRS) $(DOCD)

.PHONY: all tst xmp doc dirs clean cleanall
