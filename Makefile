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
TSTD := test

BUILD ?= build/$(PLATFORM)
OBJD ?= $(BUILD)/obj
LIBD ?= $(BUILD)/lib
BIND ?= $(BUILD)/bin
DOCD := doc

DIRS := $(OBJD) $(LIBD) $(BIND)

vpath %.h $(INCD):$(SRCD):$(TSTD)
vpath %.hpp $(INCD):$(SRCD):$(TSTD)
vpath %.cpp $(SRCD):$(TSTD)
vpath %.c $(SRCD):$(TSTD)
vpath %.o $(OBJD)
vpath %.a $(LIBD)
vpath %$(EXE) $(BIND)

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
LDFLAGS := -fPIC
LIBFLAGS :=  -l cunit -L $(LIBD)

# Main label

all: dirs lib$(TGT).a
tst: dirs $(TGT)-test$(EXE)
xmp: dirs $(TGT)-basic-example$(EXE) $(TGT)-extended-example$(EXE)
doc: $(DOCD)/html/index.html

# Install label

PRFX ?= .

install: all doc
	cp $(LIBD)/lib$(TGT).a $(PRFX)/lib/
	cp $(INCD)/$(TGT).h $(PRFX)/include/
	@-mkdir -p $(PRFX)/doc/$(TGT)
	@-$(RM) -r $(PRFX)/doc/$(TGT)/*
	cp -rf $(DOCD)/* $(PRFX)/doc/$(TGT)/

# Project file dependencies

$(OBJD)/$(TGT).o $(OBJD)/$(TGT)-g.o: $(TGT).c $(TGT).h
$(OBJD)/$(TGT)_test-g.o \
	$(OBJD)/$(TGT)_test_output-g.o \
	$(OBJD)/$(TGT)_test_args-g.o \
	$(OBJD)/$(TGT)_test_tests-g.o: $(TGT)_test.h $(TGT).h
$(OBJD)/$(TGT)_test_g.o: $(TGT)_test.c
$(OBJD)/$(TGT)_test_output-g.o: $(TGT)_test_output.c
$(OBJD)/$(TGT)_test_args-g.o: $(TGT)_test_args.c
$(OBJD)/$(TGT)_test_tests-g.o: $(TGT)_test_tests.c
$(OBJD)/$(TGT)_basic_example.o: $(TGT)_basic_example.c $(TGT).h
$(OBJD)/$(TGT)_extended_example.o: $(TGT)_extended_example.c $(TGT).h

# Project files build rules

$(OBJD)/%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES:%=-I %) -o2 -c $< -o $@

$(OBJD)/%-g.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES:%=-I %) -Itest -D_DEBUG -g -o0 -c $< -o $@

# Project targets build

$(LIBD)/lib$(TGT).a: $(OBJS)
	@echo ==== Building $@ [lib$(TGT)] ====
	$(AR) $(ARFLAGS) $@ $^
	@echo =*_*= Done [$@] =*_*=
	@echo

$(LIBD)/lib$(TGT)d.a: $(OBJSD)
	@echo ==== Building $@ [lib$(TGT)] ====
	$(AR) $(ARFLAGS) $@ $^
	@echo =*_*= Done [$@] =*_*=
	@echo

$(BIND)/$(TGT)-test$(EXE): lib$(TGT)d.a
$(BIND)/$(TGT)-test$(EXE): $(OBJST)
	@echo ==== Building $@ [$(TGT) test application] ====
	$(CXX) $(LDFLAGS) $(filter %.o,$^) -l $(TGT)d $(LIBFLAGS) -o $@
	@echo =*_*= Done [$@] =*_*=
	@echo

$(BIND)/$(TGT)-basic-example$(EXE) $(BIND)/$(TGT)-extended-example$(EXE): lib$(TGT).a

$(BIND)/$(TGT)-basic-example$(EXE): $(TGT)_basic_example.o
	@echo ==== Building $@ [$(TGT) basic example application] ====
	$(CXX) $(LDFLAGS) $(filter %.o,$^) -l $(TGT) $(LIBFLAGS) -o $@
	@echo =*_*= Done [$@] =*_*=
	@echo

$(BIND)/$(TGT)-extended-example$(EXE): $(TGT)_extended_example.o
	@echo ==== Building $@ [$(TGT) extended example application] ====
	$(CXX) $(LDFLAGS) $(filter %.o,$^) -l $(TGT) $(LIBFLAGS) -o $@
	@echo =*_*= Done [$@] =*_*=
	@echo

# Documentation

$(DOCD)/html/index.html: $(TGT)_dox.cfg $(TGT).h
	@echo ==== Building $(TGT) documentation ====
	doxygen $<
	@echo =*_*= Done [$@] =*_*=
	@echo

# Other project management label

dirs: $(DIRS)
$(DIRS):
	@for dir in $(DIRS); do \
	  mkdir -p $$dir; \
	done

clean:
	@$(RM) $(foreach dir,$(DIRS),$(dir)/*)
	@$(RM) -r $(DOCD)/*

cleanall:
	@$(RM) -r $(DIRS) $(DOCD)

.PHONY: all tst xmp doc dirs clean cleanall
