# makefile settings
.SUFFIXES:
.SECONDARY:
.SECONDARYEXPANSION:
.DEFAULT_GOAL := tests

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
DEPDIR    := $(BUILDDIR)/dep
GENERATED_DIRS := $(BINDIR) $(LIBDIR) $(BUILDDIR) $(RESULTDIR) $(DEPDIR)
# $(VULKAN_SDK) must be set by environment (vulkan installer sets it)
ifndef VULKAN_SDK
$(error missing environment variable VULKAN_SDK)
endif

# os specific variables
# win32
ifeq ($(OS),Windows_NT)
OSLIBS      := -lOneCore -llibel -lvulkan-1 -L$(VULKAN_SDK)/lib
ENGINELIB   := $(LIBDIR)/libel.lib
SRC_EXCLUDE := linux
SHELL       := pwsh.exe
.SHELLFLAGS := -NoProfile -Command
# linux
# TODO: don't assume linux if not win32
else
OSLIBS      := -lxcb -lel -lvulkan -L$(VULKAN_SDK)/lib
ENGINELIB   := $(LIBDIR)/libel.a
SRC_EXCLUDE := win32
endif

# TODO: make this recurse?
DEPFILES := $(wildcard $(DEPDIR)/*)
-include $(DEPFILES)


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
LIBFLAGS := $(OSLIBS) -L$(LIBDIR)
# CXX GENERIC FLAGS
CXXFLAGS := -Weverything -Wno-c++98-compat -Wno-pre-c++20-compat-pedantic -Wno-padded
CXXFLAGS += -std=c++20
CXXFLAGS += -g -O3
# CXX FILETYPE FLAGS
OBJFLAGS := -c $(CXXFLAGS) $(INCFLAGS)
EXEFLAGS := $(CXXFLAGS) $(OSLIBS) $(INCFLAGS) $(LIBFLAGS)

depFunc = -MMD -MF $(DEPDIR)/$(notdir $1).d

# AR
AR := llvm-ar
ARFLAGS := rc

# objects
$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp | $(DEPDIR)/
	$(CXX) $(OBJFLAGS) $(call depFunc,$@) $< -o $@

# libraries
$(ENGINELIB): $(OBJECTS) | $(LIBDIR)/
	$(AR) $(ARFLAGS) $@ $?

# TODO: move %.exe target sources into a different dir?
# TODO: library tests shouldn't depend on the whole library,
#       changing one source file will rerun every test
$(BINDIR)/%.exe: $(TESTDIR)/%.cpp $(ENGINELIB) | $(BINDIR)/ $(DEPDIR)/
	$(CXX) $< $(EXEFLAGS) $(call depFunc,$@) -o $@


# UTILITY TARGETS
#
# compile_flags.txt is used by clangd
# generate compile_flags.txt with the same args used to compile .o files
compile_flags.txt: makefile
	#echo $(subst " ","NEWLINE",$(OBJFLAGS)) > $@
	rm $@
	$(foreach X,$(OBJFLAGS),echo $(X) >> $@;)

# anything in $(BUILDDIR) needs $(BUILDDIR)
$(BUILDDIR)/%: | $(BUILDDIR)/
$(DEPDIR)/: | $(BUILDDIR)/
# generate directories
%/:
	mkdir -p $@

.PHONY: clean
clean:
	-$(foreach dir,$(GENERATED_DIRS),rm -r $(dir) ; )
