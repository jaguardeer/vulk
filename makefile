ifneq "$(OS)" "Windows_NT"
$(error makefile only tested on Windows. be careful)
endif


CXX=clang++


# header file include path
INCLUDES=-isystem $(VULKAN_SDK)/Include
# compiler flags
CXXFLAGS=$(INCLUDES) -std=c++20 -c -g -O0
#CXXFLAGS += -Wall -Wextra -Wpedantic
CXXFLAGS += -Weverything -Wno-c++98-compat

# linker flags
LDLIBS=-luser32
LDFLAGS= $(LDLIBS) -g

.SECONDARY:

BUILDDIR=./build
OBJDIR=$(BUILDDIR)/obj

test.exe:

%.exe: $(OBJDIR)/%.o
	$(CXX) $(LDFLAGS) -o $@ $<

$(OBJDIR)/%.o: %.cxx $(OBJDIR)
	$(CXX) $(CXXFLAGS) -o $@ $<

$(OBJDIR):
	pwsh -c mkdir $@


clean:
	-pwsh -c rmdir -Recurse $(BUILDDIR)
	-pwsh -c rm test.exe, test.ilk, test.pdb
