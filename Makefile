# -----------------------------------------------------------------------------
# MAKEFILE project
# -----------------------------------------------------------------------------
# Project targets:
# + libcuw: CUnit wrapper library.
# + cuw-test: Test application for libcuw.
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
OBJD := obj
LIBD := lib
BIND := bin
TSTD := test

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

# Compiler and linker options

INCLUDES := $(INCD)

CFLAGS := -Wall -Wextra -Werror -Wpedantic -pedantic-errors -fPIC
ARFLAGS := rcs
LDFLAGS := -fPIC -lconfig

# Main label

all: dirs lib$(TGT).a
tst: dirs $(TGT)-tst$(EXE)

# Project file dependencies

$(OBJD)/$(PLATFORM)/$(TGT).o: $(TGT).c $(TGT).h
$(OBJD)/$(PLATFORM)/$(TGT)-tst.o:

# Project files build rules

$(OBJD)/$(PLATFORM)/%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES:%=-I %) -c $< -o $@

# Project targets build

$(LIBD)/$(PLATFORM)/lib$(TGT).a: $(OBJS)
	@echo ==== Building $@ [lib$(TGT)] ====
	$(AR) $(ARFLAGS) $@ $^
	@echo =*_*= Done [$@] =*_*=
	@echo

$(BIND)/$(PLATFORM)/$(TGT)-tst$(EXE): lib$(TGT).a
$(BIND)/$(PLATFORM)/$(TGT)-tst$(EXE): $(TGT)-tst.o
	@echo ==== Building $@ [$(TGT) test application] ====
	$(CXX) $(LDFLAGS) $^ -l cunit -l $(TGT) -L $(LIBD)/$(PLATFORM) -o $@
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

cleanall:
	@$(RM) -r $(DIRS)

.PHONY: all dirs clean cleanall
