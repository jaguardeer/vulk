# project directories
# source directories
SRCDIR    := ./src
INCDIR    := ./include
TESTDIR   := ./tests
# generated directories
BINDIR    := ./bin
LIBDIR    := ./lib
BUILDDIR  := ./build
RESULTDIR := ./results
GENERATED_DIRS := $(BINDIR) $(LIBDIR) $(BUILDDIR) $(RESULTDIR)

# os specific variables
# win32
ifeq ($(OS),Windows_NT)
OSLIBS      := -luser32 -llibel
ENGINELIB   := $(LIBDIR)/libel.lib
SRC_EXCLUDE := linux
SHELL       := pwsh.exe
.SHELLFLAGS := -NoProfile -Command
# linux
# TODO: don't assume linux if not win32
else
OSLIBS      := -lxcb -lel
ENGINELIB   := $(LIBDIR)/libel.a
SRC_EXCLUDE := win32
endif


# parse ./src
# adapted from stackoverflow.com/questions/4036191/sources-from-subdirectories-in-makefile
# returns list of all files (recursively) in ./$1 that match $2 and aren't named $3
recurse_exclude = $(wildcard $1$2) $(foreach d,$(filter-out $1$3,$(wildcard $1*)),$(call recurse_exclude,$d/,$2,$3))
# CPPFILES = all **/*.cpp files in $(SRCDIR) that aren't named $(SRC_EXCLUDE)
CPPFILES := $(call recurse_exclude,$(SRCDIR)/,*.cpp,$(SRC_EXCLUDE))
# OBJFILES = same as CPPFILES, but is .o and in ./build/*
OBJFILES := $(patsubst $(SRCDIR)/%.cpp,$(BUILDDIR)/%.o,$(CPPFILES))

# specific targets
# main library (libel.lib)
$(ENGINELIB): $(OBJFILES)
# object files need directories
$(OBJFILES): | $(dir $(OBJFILES))

# TODO: do tests correctly
# TESTBINS = ./test/*.cpp -> ./bin/*.exe
TESTBINS := $(patsubst $(TESTDIR)/%.cpp,$(BINDIR)/%.exe,$(wildcard $(TESTDIR)/*.cpp))
.PHONY: tests run-tests
tests run-tests: $(TESTBINS)
run-tests: $(patsubst $(BINDIR)/%.exe,$(RESULTDIR)/%.txt,$(TESTBINS))

$(RESULTDIR)/%.txt: bin/%.exe | $(RESULTDIR)/
	./$< | tee $@


# generic targets
# CXX
CXX := clang++
# CXX INCLUDES
INCFLAGS := -isystem $(VULKAN_SDK)/include -I$(INCDIR)
LIBFLAGS := $(OSLIBS) -L$(LIBDIR) -lvulkan -L$(VULKAN_SDK)/lib
# CXX GENERIC FLAGS
CXXFLAGS := -Weverything -Wno-c++98-compat -Wno-pre-c++20-compat-pedantic -Wno-padded
CXXFLAGS += -std=c++20
CXXFLAGS += -g
# CXX FILETYPE FLAGS
OBJFLAGS := -c $(CXXFLAGS) $(INCFLAGS)
EXEFLAGS := $(CXXFLAGS) $(OSLIBS) $(INCFLAGS) $(LIBFLAGS)

# AR
AR := llvm-ar
ARFLAGS := rc

# special targets
.SUFFIXES:
.SECONDARY:
.PHONY: clean

# objects
# TODO: AUTO GENERATE HEADER DEPS
$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(OBJFLAGS) $< -o $@

# libraries
$(ENGINELIB): $(OBJECTS) | $(LIBDIR)/
	$(AR) $(ARFLAGS) $@ $?

# executables
$(BINDIR)/test.exe: $(TESTDIR)/test.cpp $(ENGINELIB) | $(BINDIR)/
	$(CXX) $< $(EXEFLAGS) -o $@

$(BINDIR)/test%.exe: $(TESTDIR)/test%.cpp $(ENGINELIB) | $(BINDIR)/
	$(CXX) $< $(EXEFLAGS) -o $@

# TODO: move %.exe target sources into a different dir
# TODO: move %.exe target sources into a different dir
$(BINDIR)/%.exe: $(TESTDIR)/%.cpp $(ENGINELIB) | $(BINDIR)/
	$(CXX) $< $(EXEFLAGS) -o $@


# UTILITY TARGETS
#
# compile_flags.txt is used by clangd
# generate compile_flags.txt with the same args used to compile .o files
compile_flags.txt: makefile
	#echo $(subst " ","NEWLINE",$(OBJFLAGS)) > $@
	rm $@
	$(foreach X,$(OBJFLAGS),echo $(X) >> $@;)

# anything in $(BUILDDIR) needs $(BUILDDIR)
$(BUILDDIR)/%: | $(BUILDDIR)
# generate directories
%/:
	mkdir -p $@

clean:
	-$(foreach dir,$(GENERATED_DIRS),rm -r $(dir) ; )
