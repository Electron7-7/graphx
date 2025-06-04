CXX := @ clang++
CC  := @ clang

# LSAN_OPTIONS=verbosity=1:log_threads=1 # Use this environment variable for more verbosity with address sanitizer
CXXFLAGS = -g -Wall -fsanitize=address -frtti -std=c++20 $(JOLTFLAGS) $(GRAPHXFLAGS)
CCFLAGS  := -g -Wall -fsanitize=address -frtti

INCLUDE_COMMON := -I src/ -I src/common -I src/engine/embedded

INCLUDE   = -I src/system/common/linux $(INCLUDE_COMMON)
LIBRARIES = -L src/lib/public/linux -l glfw -l Jolt -l freetype

ifeq ($(OS),Windows_NT)
	WCXX := clang++
	WCC  := clang
else
	WCXX := x86_64-w64-mingw32-g++
	WCC  := x86_64-w64-mingw32-gcc
endif

WINCLUDE   := -I src/system/windows/common $(INCLUDE_COMMON)
WINLIBRARIES := -L src/lib/public/windows -l jolt-mingw-w64/Jolt -l lib-mingw-w64/glfw3 -l gdi32

JOLTFLAGS   := -D JPH_PROFILE_ENABLED -D JPH_OBJECT_STREAM -D JPH_DEBUG_RENDERER
GRAPHXFLAGS = -D COMPILER_FORWARD_DECLARATIONS

# FPS_LIMIT = 60 # FPS limit for mangohud (FPS_LIMIT <= 0 results in an uncapped framerate)
# TESTRUN_LINUX = exit 0 &&
# TESTRUN_WINDOWS = exit 0 &&
# TEST_LINUX = ~/bin/mangohudtest $(FPS_LIMIT) # "mangohudtest" is a custom script I wrote for test-running GraphX with MangoHUD + Gamemode. This is why I disable it on Windows
# TEST_LINUX = 
# TEST_WINDOWS = # nothing here, yet

OUT ?= build/

LINUX   := _$(shell uname -s)_$(subst .,_,$(shell uname -r)).$(shell uname -m)
WINDOWS := _Windows_10_x86_64.exe
APP ?= $(OUT)/GraphX$(LINUX)

SRC_DIRS =                    \
	src/math                  \
	src/engine                \
	src/system                \
	src/engine/interpreter    \
	src/engine/rendering      \
	src/engine/embedded       \
	src/engine/physics        \
	src/engine/theatre        \
	src/engine/ui             \
	src/engine/things         \
	src/engine/things/actors  \
	src/engine/things/devices

# Object files compiled from "DIRTY_SRC_DIRS" will not be cleaned during a dirty clean
DIRTY_SRC_DIRS =         \
	src/common/glad      \
	src/common/DearImGui

RESOURCE_DIR := src/resources
RESOURCE_EMBED_DIR := src/engine/embedded

CXX_SRCS = $(foreach directory,$(SRC_DIRS),$(wildcard $(directory)/*.cpp))
CC_SRCS  = $(foreach directory,$(SRC_DIRS),$(wildcard $(directory)/*.c))
CXX_OBJS = $(addprefix $(OUT)/,$(subst .cpp,.obj,$(notdir $(CXX_SRCS))))
CC_OBJS  = $(addprefix $(OUT)/,$(subst .c,.o,$(notdir $(CC_SRCS))))

DIRTY_CXX_SRCS = $(foreach directory,$(DIRTY_SRC_DIRS),$(wildcard $(directory)/*.cpp))
DIRTY_CC_SRCS  = $(foreach directory,$(DIRTY_SRC_DIRS),$(wildcard $(directory)/*.c))
DIRTY_CXX_OBJS = $(addprefix $(OUT)/,$(subst .cpp,.obj,$(notdir $(DIRTY_CXX_SRCS))))
DIRTY_CC_OBJS  = $(addprefix $(OUT)/,$(subst .c,.o,$(notdir $(DIRTY_CC_SRCS))))

SRCS = $(CXX_SRCS) $(DIRTY_CXX_SRCS) $(CC_SRCS) $(DIRTY_CC_SRCS)
OBJS = $(CXX_OBJS) $(DIRTY_CXX_OBJS) $(CC_OBJS) $(DIRTY_CC_OBJS)

VPATH := $(SRC_DIRS) $(DIRTY_SRC_DIRS)

IMAGES_C = $(RESOURCE_EMBED_DIR)/images.c
IMAGES_H = $(RESOURCE_EMBED_DIR)/images.h
IMAGES = $(wildcard $(RESOURCE_DIR)/images/*)
I = $(RESOURCE_DIR)/images

SHADERS_C = $(RESOURCE_EMBED_DIR)/shaders.cpp
SHADERS_H = $(RESOURCE_EMBED_DIR)/shaders.hpp
SHADERS = $(wildcard $(RESOURCE_DIR)/shaders/*.glsl)
S = $(RESOURCE_DIR)/shaders

THEATRES_C = $(RESOURCE_EMBED_DIR)/theatres.cpp
THEATRES_H = $(RESOURCE_EMBED_DIR)/theatres.hpp
THEATRES = $(wildcard $(RESOURCE_DIR)/theatres/*.gt)
T = $(RESOURCE_DIR)/theatres

MODELS_C = $(RESOURCE_EMBED_DIR)/models.cpp
MODELS_H = $(RESOURCE_EMBED_DIR)/models.hpp
MODELS = $(wildcard $(RESOURCE_DIR)/models/*.obj)
MTL_FILES = $(wildcard $(RESOURCE_DIR)/models/*.mtl)
M = $(RESOURCE_DIR)/models

# Colors for colored output
RESET = \\033[0m
RED   = \\033[31m
GREEN = \\033[32m
BLUE  = \\033[34m

.PHONY: all sublime build debug release linux windows clean dirty_clean clean_tmp_files clean_resources rebuild_resources rebuild_images rebuild_shaders rebuild_theatres rebuild_models

all: $(APP)
	@ echo -e "Finished compiling: $(BLUE)$(APP)$(RESET)"

# This removes the color variables; I use it in Sublime Text build systems, since ST's console output doesn't support colored text by default
sublime: ;@:
	$(eval RESET="")
	$(eval RED="")
	$(eval GREEN="")
	$(eval BLUE="")

eval_windows:
	$(eval INCLUDE = $(WINCLUDE))
	$(eval LIBRARIES = $(WINLIBRARIES))
	$(eval GRAPHXFLAGS += -static -mwindows)

windows: eval_windows
# 	$(eval OUT = build/windows/release)
	$(eval APP = GraphX$(WINDOWS))

windows_debug: eval_windows
# 	$(eval OUT = build/windows/debug)
	$(eval APP = GraphXDebug$(WINDOWS))

eval_debug:
# 	$(eval OUT = build/linux/debug)
	$(eval APP = GraphXDebug$(LINUX))
	$(eval GRAPHXFLAGS += -D GRAPHX_DEBUG)

debug: eval_debug $(APP)
	@ echo -e "Finished compiling: $(BLUE)$(APP)$(RESET)"

eval_release:
	$(eval GRAPHXFLAGS = $(patsubst $%-D GRAPHX_DEBUG,$%,$(CXXFLAGS)))
# 	$(eval OUT = build/linux/release)

release: eval_release $(APP)
	@ echo -e "Finished compiling: $(BLUE)$(APP)$(RESET)"

$(APP): $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(INCLUDE) $^ -o $@ $(LIBRARIES)

$(OUT)/%.obj: %.cpp | build
	@ echo -e "Compiling: $(GREEN)$<$(RESET)"
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

$(OUT)/%.o: %.c | build
	@ echo -e "Compiling: $(GREEN)$<$(RESET)"
	$(CC) $(CCFLAGS) $(INCLUDE) -c $< -o $@

build:
	@ -mkdir -p $(OUT)

clean: rebuild_resources
	@ -rm -rf $(OUT)

dirty_clean:
	@ -rm -f $(CXX_OBJS) $(CC_OBJS)

clean_tmp_files:
	@ -rm -f $(OUT)/*.tmp

clean_resources:
	@ -rm -f $(IMAGES_C) $(IMAGES_H) $(SHADERS_C) $(SHADERS_H) $(THEATRES_C) $(THEATRES_H) $(MODELS_H) $(MODELS_C)

rebuild_resources: clean_resources
	-make -s $(IMAGES_C) $(SHADERS_C) $(THEATRES_C) $(MODELS_C)

# If rebuilding all resources takes too long, use one of these
rebuild_images:
	-rm -f $(IMAGES_C) $(IMAGES_H)
	-make -s $(IMAGES_C)

rebuild_shaders:
	-rm -f $(SHADERS_C) $(SHADERS_H)
	-make -s $(SHADERS_C)

rebuild_theatres:
	-rm -f $(THEATRES_C) $(THEATRES_H)
	-make -s $(THEATRES_C)

rebuild_models:
	-rm -f $(MODELS_C) $(MODELS_H)
	-make -s $(MODELS_C)

# Embedding resources
$(IMAGES_C): $(IMAGES_H)
	$(foreach file,$(IMAGES),$(shell xxd -b -n $(file:$(I)/%=%) -i $(file) >> $(IMAGES_C)))

$(IMAGES_H):
	$(shell printf "#ifndef GRAPHX_EMBEDDED_IMAGES\n#define GRAPHX_EMBEDDED_IMAGES\n#include <string>\n" > $(IMAGES_H))
	$(foreach filename,$(IMAGES), $(shell printf "\n#define $(subst .,_,$(basename $(filename:$(I)/%=%))) std::string(\"$(subst .,_,$(filename:$(I)/%=%))\")\nextern unsigned char $(subst .,_,$(filename:$(I)/%=%))[];\nextern unsigned int $(subst .,_,$(filename:$(I)/%=%))_len;\n" >> $(IMAGES_H)))
	$(shell printf "#endif" >> $(IMAGES_H))

$(SHADERS_C): $(SHADERS_H)
	$(shell printf "#include <string>\n" > $(SHADERS_C))
	$(foreach file,$(shell ls $(S)),$(shell printf "std::string $(subst .,_,$(file:$(S)/%=%)) = R\"~(\n" >> $(SHADERS_C) && cat $(S)/$(file) >> $(SHADERS_C) && printf "\n)~\";\n" >> $(SHADERS_C)))

$(SHADERS_H):
	$(shell printf "#ifndef GRAPHX_EMBEDDED_SHADERS\n#define GRAPHX_EMBEDDED_SHADERS\n#include <string>\n" > $(SHADERS_H))
	$(foreach file,$(shell ls $(S)),$(shell printf "extern std::string $(subst .,_,$(file:$(S)/%=%));\n" >> $(SHADERS_H)))
	$(shell printf "#endif" >> $(SHADERS_H))

$(THEATRES_H):
	$(shell printf "#ifndef GRAPHX_EMBEDDED_THEATRES\n#define GRAPHX_EMBEDDED_THEATRES\n#include <string>\n#include <map>\nextern std::map<int, std::string> embedded_theatres;\n#endif" > $(THEATRES_H))

$(THEATRES_C): $(THEATRES_H)
	$(shell printf "#include <string>\n#include <map>\nstd::map<int, std::string> embedded_theatres =\n{" > $(THEATRES_C))
	$(foreach theatre,$(THEATRES),$(shell printf ",{$(shell printf $(theatre) | grep -P --only-matching '(.+\/)+\K[0-9]+'), std::string{R\"~(" >> $(THEATRES_C) && cat $(theatre) >> $(THEATRES_C) && printf ")~\"}}" >> $(THEATRES_C)))
	$(shell sed 's/^{,{/{{/' -i $(THEATRES_C))
	$(shell printf "\n};" >> $(THEATRES_C))

$(MODELS_H):
	$(shell printf "#ifndef GRAPHX_MODELS\n#define GRAPHX_MODELS\n#include <string>\n" > $(MODELS_H))
	$(foreach graphxmodel,$(wildcard $(M)/*.graphxmodel),$(shell printf "#include \"../$(M:$(SRC)/%=%)/$(graphxmodel:$(M)/%=%)\"\n" >> $(MODELS_H)))
	$(foreach model,$(MODELS),$(shell printf "\n#define $(basename $(model:$(M)/%=%))_MODEL std::string(\"$(subst .,_,$(model:$(M)/%=%))\")\nextern std::string $(subst .,_,$(model:$(M)/%=%));\n" >> $(MODELS_H)))
	$(foreach material,$(MTL_FILES),$(shell printf "extern std::string $(subst .,_,$(material:$(M)/%=%));\n" >> $(MODELS_H)))
	$(shell printf "#endif" >> $(MODELS_H))

$(MODELS_C): $(MODELS_H)
	$(shell printf "#include <string>\n" > $(MODELS_C))
	$(foreach model,$(MODELS),$(shell printf "std::string $(subst .,_,$(model:$(M)/%=%)) = R\"~(\n" >> $(MODELS_C) && cat $(model) >> $(MODELS_C) && printf "\n)~\";\n" >> $(MODELS_C)))
	$(foreach material,$(MTL_FILES),$(shell printf "std::string $(subst .,_,$(material:$(M)/%=%)) = R\"~(\n" >> $(MODELS_C) && cat $(material) >> $(MODELS_C) && printf "\n)~\";\n" >> $(MODELS_C)))