# project directories
# source directories
SRCDIR    := ./src
INCDIR    := ./include
# generated directories
BINDIR    := ./bin
LIBDIR    := ./lib
BUILDDIR  := ./build

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
OBJFILES := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(CPPFILES))
OBJFILES := $(OBJFILES:.cpp=.o)
OBJDIRS  := $(dir $(OBJFILES))


# specific targets
# main library (libel.lib)
$(ENGINELIB): $(OBJFILES)
# build directories for the object files
$(OBJFILES): | $(OBJDIRS)
# TODO: tests


# generic targets
# CXX
CXX := clang++
# CXX INCLUDES
INCFLAGS := -isystem $(VULKAN_SDK)/include -I$(INCDIR)
LIBFLAGS := $(OSLIBS) -L$(LIBDIR)
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
$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp | $(BUILDDIR)/
	$(CXX) $(OBJFLAGS) $< -o $@

# libraries
$(ENGINELIB): $(OBJECTS) | $(LIBDIR)/
	$(AR) $(ARFLAGS) $@ $^

# executables
$(BINDIR)/test%.exe: tests/test%.cpp $(ENGINELIB) | $(BINDIR)/
	$(CXX) $< $(EXEFLAGS) -o $@


# UTILITY TARGETS
#
# compile_flags.txt is used by clangd
# generate compile_flags.txt with the same args used to compile .o files
compile_flags.txt: makefile
	#echo $(subst " ","NEWLINE",$(OBJFLAGS)) > $@
	rm $@
	$(foreach X,$(OBJFLAGS),echo $(X) >> $@;)

# generated directories
%/:
	mkdir -p $@

clean:
	-rm -r $(BUILDDIR)
	-rm -r $(LIBDIR)
	-rm -r $(BINDIR)
