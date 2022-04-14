# TODO: AUTO GENERATE HEADER DEPS


ifeq ($(OS),Windows_NT)
SHELL := pwsh.exe
.SHELLFLAGS := -NoProfile -Command
OSLIBS := -luser32
else # TODO: probably shouldn't assume it's Linux if not Windows
OSLIBS := -lxcb
endif


BUILDDIR := ./build
SRCDIR := ./src
CXX := clang++

INCLUDES := -isystem $(VULKAN_SDK)/include
CXXFLAGS := -Weverything -Wno-c++98-compat -Wno-pre-c++20-compat-pedantic -Wno-padded
CXXFLAGS += -std=c++20
CXXFLAGS += -g

OBJFLAGS := $(INCLUDES) $(CXXFLAGS) -c
EXEFLAGS := $(CXXFLAGS) $(OSLIBS)

.SECONDARY:
.PHONY: clean

testWindow.exe: $(BUILDDIR)/Window.o $(BUILDDIR)/WindowImpl_linux.o testWindow.cpp
	$(CXX) $(EXEFLAGS) $^ -o $@

ifeq ($(OS),Windows_NT)
testWindow.exe: $(BUILDDIR)/Window.o $(BUILDDIR)/WindowImpl_win32.o testWindow.cpp
	$(CXX) $(EXEFLAGS) $^ -o $@
endif

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp | $(BUILDDIR)
	$(CXX) $(OBJFLAGS) $< -o $@

compile_flags.txt:
	echo -std=c++20 > $@
	echo -isystem >> $@
	echo $(VULKAN_SDK)/include >> $@

$(BUILDDIR):
	mkdir $@

clean:
	-rm -r $(BUILDDIR)
