
# os specific variables
ifeq ($(OS),Windows_NT)
OSLIBS := -luser32 -llibel
ENGINELIB = $(LIBDIR)/libel.lib
SHELL := pwsh.exe
.SHELLFLAGS := -NoProfile -Command
else # TODO: probably shouldn't assume it's Linux if not Windows
OSLIBS := -lxcb -lel
ENGINELIB = $(LIBDIR)/libel.a
endif

# source directories
SRCDIR := ./src
INCDIR := ./include
# generated directories
LIBDIR := ./lib
BUILDDIR := ./build

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

.SECONDARY:
.PHONY: clean

# specific targets
# TODO: AUTO GENERATE OBJECT DEPS
$(ENGINELIB): $(BUILDDIR)/Window.o $(BUILDDIR)/WindowImpl_linux.o

testWindow.exe: tests/testWindow.cpp $(ENGINELIB)
		$(CXX) $< $(EXEFLAGS) -o $@

# objects
# TODO: AUTO GENERATE HEADER DEPS
$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp | $(BUILDDIR)
	$(CXX) $(OBJFLAGS) $< -o $@

# libraries
$(ENGINELIB): | $(LIBDIR)
	$(AR) $(ARFLAGS) $@ $^

# executables?


# compile_flags.txt is used by clangd
# generate compile_flags.txt with the same args used to compile .o files
compile_flags.txt: makefile
	#echo $(subst " ","NEWLINE",$(OBJFLAGS)) > $@
	rm $@
	$(foreach X,$(OBJFLAGS),echo $(X) >> $@;)


# generated directories
$(BUILDDIR) $(LIBDIR):
	mkdir $@

clean:
	-rm -r $(BUILDDIR)
	-rm -r $(LIBDIR)
