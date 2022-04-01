ifneq "$(OS)" "Windows_NT"
$(error makefile only tested on Windows. be careful)
endif


CXX=clang++
INCLUDES=-I$(VULKAN_SDK)/include
CXXFLAGS=$(INCLUDES) -c -ggdb

.SECONDARY:

BUILDDIR=./build
OBJDIR=$(BUILDDIR)/obj

%.exe: $(OBJDIR)/%.o
	$(CXX) -o $@ $<

$(OBJDIR)/%.o: %.cxx $(OBJDIR)
	$(CXX) $(CXXFLAGS) -o $@ $<

$(OBJDIR):
	pwsh -c mkdir $@


clean:
	-pwsh -c rmdir -Recurse $(BUILDDIR)
	-pwsh -c rm test.exe
