BUILDDIR=./build


CXX=clang++

INCLUDES=-isystem $(VULKAN_SDK)/include
CXXFLAGS=-fmodules -std=c++20 -stdlib=libc++

OBJFLAGS=$(INCLUDES) $(CXXFLAGS) -c
PCMFLAGS=$(INCLUDES) $(CXXFLAGS) --precompile -x c++-module

.SECONDARY:

$(BUILDDIR)/testVulkanResultString.exe:

$(BUILDDIR):
	mkdir $@

$(BUILDDIR)/%.pcm: %.ixx | $(BUILDDIR)
	$(CXX) $(PCMFLAGS) $< -o $@

$(BUILDDIR)/%.o: %.cxx $(BUILDDIR)/%.pcm | $(BUILDDIR)
	$(CXX) $(OBJFLAGS) -fmodule-file=$(word 2,$^) $< -o $@

$(BUILDDIR)/%.o: %.cxx $(BUILDDIR)/vulkanResultString.pcm | $(BUILDDIR)
	$(CXX) $(OBJFLAGS) -fprebuilt-module-path=$(BUILDDIR) $< -o $@

#vulkanResultString.o: vulkanResultString.cxx vulkanResultString.pcm
#	$(CXX) $(CXXFLAGS) -fmodule-file=$(basename $@).pcm -c $< -o $@
#
#testVulkanResultString.o: testVulkanResultString.cxx vulkanResultString.pcm
#	$(CXX) $(CXXFLAGS) -fprebuilt-module-path=. -c $< -o $@

$(BUILDDIR)/testVulkanResultString.exe: $(BUILDDIR)/testVulkanResultString.o $(BUILDDIR)/vulkanResultString.o
	$(CXX) $(CXXFLAGS) $^ -o $@
