# set shell for windows
ifeq ($(OS),Windows_NT)
SHELL       := pwsh.exe
.SHELLFLAGS := -NoProfile -Command
endif

# flags
.DEFAULT_GOAL := debug
WARNINGS = #-pedantic -Wall -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-include-dirs -Wnoexcept -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=5 -Wswitch-default -Wundef -Wno-unused -Wuseless-cast -Wzero-as-null-pointer-constant -Wduplicated-cond
#-Wmissing-declarations

OPTIMIZE_FLAGS = -O3 -Ofast
DEBUG_FLAGS = -g


# compiling
CXX = clang++
CXXFLAGS = -std=c++17

COMPILEXX = $(CXX) $(DEP_FLAGS) $(CXXFLAGS) -isystem $(VULKAN_SDK)/include
POSTCOMPILE = @mv -f $(DEP_DIR)/$*.Td $(DEP_DIR)/$*.d && touch $@

OUTPUT_FLAGS = -o $@
DEP_FLAGS = -MT $@ -MMD -MP -MF $(DEP_DIR)/$*.d

# directories

SRC_DIR := .
BUILD_DIR := build
DEP_DIR := build

.PHONY : clean
clean:
	-rm -r $(BUILD_DIR)
	mkdir $(BUILD_DIR)
	-rm a.exe


%.exe: %.cxx
	$(COMPILEXX) $(OUTPUT_FLAGS) $< $(LIB_FLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cxx
	$(COMPILEXX) $(OUTPUT_FLAGS) $(INCLUDE_FLAGS) -c $<

OBJECT_FILES := test_graphics_engine.o graphics_engine.o window_win32.o input_win32.o vulkan_device.o
OBJECT_FILES := $(addprefix $(BUILD_DIR)/, $(OBJECT_FILES))

SHADERS := vert.spv frag.spv

APP_FILES := a.exe vert.spv frag.spv

# main targets
debug: CXXFLAGS += $(WARNINGS) $(DEBUG_FLAGS)
debug: $(APP_FILES)

a.exe: LIB_FLAGS = -L$(VULKAN_SDK)/lib -lvulkan-1 -luser32
a.exe: $(OBJECT_FILES)
	$(COMPILEXX) $(OBJECT_FILES) -o a.exe $(LIB_FLAGS)

test: CXXFLAGS += $(WARNINGS) $(DEBUG_FLAGS)
test: test.cxx
	$(COMPILEXX) test.cxx

# shaders
%.spv: shader.%
	$(VULKAN_SDK)/bin/glslangValidator -V $<



# auto-generated dependencies
include $(wildcard $(DEP_DIR)/*.d)
