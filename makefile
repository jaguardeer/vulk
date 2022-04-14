# TODO: AUTO GENERATE HEADER DEPS


ifeq ($(OS),Windows_NT)
SHELL := pwsh.exe
.SHELLFLAGS := -NoProfile -Command
OSLIBS := -luser32
endif

BUILDDIR := ./build
SRCDIR := ./src
CXX := clang++

INCLUDES := -isystem $(VULKAN_SDK)/include
CXXFLAGS := -std=c++20 -Weverything -Wno-c++98-compat -Wno-pre-c++20-compat-pedantic

OBJFLAGS := $(INCLUDES) $(CXXFLAGS) -c
EXEFLAGS := $(CXXFLAGS) $(OSLIBS)

.SECONDARY:
.PHONY: clean

testWindow.exe: $(BUILDDIR)/Window.o $(BUILDDIR)/WindowImpl_win32.o testWindow.cpp
	$(CXX) $(EXEFLAGS) $^ -o $@

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
