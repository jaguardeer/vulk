# TODO: AUTO GENERATE HEADER DEPS


ifeq ($(OS),Windows_NT)
SHELL := pwsh.exe
.SHELLFLAGS := -NoProfile -Command
OSLIBS := -luser32
else # TODO: probably shouldn't assume it's Linux if not Windows
OSLIBS := -lxcb
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
HINCFLAGS := -isystem $(VULKAN_SDK)/include -I$(INCDIR)
LINCFLAGS := $(OSLIBS) -L$(LIBDIR) -llibel
# CXX GENERIC FLAGS
CXXFLAGS := -Weverything -Wno-c++98-compat -Wno-pre-c++20-compat-pedantic -Wno-padded
CXXFLAGS += -std=c++20
CXXFLAGS += -g
# CXX FILETYPE FLAGS
OBJFLAGS := -c $(CXXFLAGS) $(HINCFLAGS)
EXEFLAGS := $(CXXFLAGS) $(OSLIBS) $(HINCFLAGS) $(LINCFLAGS)

# AR
AR := llvm-ar
ARFLAGS := rc

.SECONDARY:
.PHONY: clean

$(LIBDIR)/libel.a: $(BUILDDIR)/Window.o $(BUILDDIR)/WindowImpl_win32.o
$(LIBDIR)/libel.lib: $(BUILDDIR)/Window.o $(BUILDDIR)/WindowImpl_win32.o

# TODO: do this in source?
ifeq ($(OS),Windows_NT)
testWindow.exe: tests/testWindow.cpp $(LIBDIR)/libel.lib
	$(CXX) $(EXEFLAGS) $^ -o $@
else
testWindow.exe: tests/testWindow.cpp $(LIBDIR)/libel.lib
		$(CXX) $(EXEFLAGS) $^ -o $@
endif

%.exe: lib/libel.a

$(LIBDIR)/%.a $(LIBDIR)/%.lib: | $(LIBDIR)
	$(AR) $(ARFLAGS) $@ $^


$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp | $(BUILDDIR)
	$(CXX) $(OBJFLAGS) $< -o $@


# compile_flags.txt is used by clangd
# generate compile_flags.txt with the same args used to compile .o files
compile_flags.txt: makefile
	echo $(subst " ","\n",$(OBJFLAGS)) > $@

# generated directories
$(BUILDDIR) $(LIBDIR):
	mkdir $@

clean:
	-rm -r $(BUILDDIR)
	-rm -r $(LIBDIR)
