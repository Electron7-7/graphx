CXX = clang++
CC = clang

# CXXFLAGS = -g -Wall -fsanitize=address -frtti -std=c++20 $(JOLTFLAGS) $(GRAPHXFLAGS)
CXXFLAGS = -g -Wall -frtti -std=c++20 $(JOLTFLAGS) $(GRAPHXFLAGS)
CCFLAGS = -g -Wall

INCLUDES = -I src/include
LIBS = -l glfw -L src/lib/ -l Jolt -l freetype

ifeq ($(OS),Windows_NT) 
	WCXX = clang++
	WCC = clang
else
	WCXX = x86_64-w64-mingw32-g++
	WCC = x86_64-w64-mingw32-gcc
endif

WCXXFLAGS = -g -Wall -std=c++20 -static -mwindows -frtti -ffat-lto-objects $(JOLTFLAGS) $(GRAPHXFLAGS)
WCCFLAGS = -g -Wall -static -mwindows
WINCLUDES = -I src/include -I src/windows_dependencies/include
WLIBS = -L src/windows_dependencies/lib/jolt-mingw-w64 -l Jolt -L src/windows_dependencies/lib/lib-mingw-w64 -l glfw3 -l gdi32

JOLTFLAGS = -D JPH_PROFILE_ENABLED -D JPH_OBJECT_STREAM -D JPH_DEBUG_RENDERER
GRAPHXFLAGS = -D COMPILER_FORWARD_DECLARATIONS

LINUX = GraphX_$(shell uname -s)_$(shell uname -r)_$(shell uname -m)
WINDOWS = GraphX_Windows_x86_64.exe
NAME = ""

FPS_LIMIT = 60 # FPS limit for mangohud (FPS_LIMIT <= 0 results in an uncapped framerate)
TESTRUN_LINUX = exit 0 &&
TESTRUN_WINDOWS = exit 0 &&
# TEST_LINUX = ~/bin/mangohudtest $(FPS_LIMIT) # "mangohudtest" is a custom script I wrote for test-running GraphX with MangoHUD + Gamemode. This is why I disable it on Windows
TEST_LINUX = 
TEST_WINDOWS = # nothing here, yet

SRC := src

O = build

EXT_OBJS =                      \
	$(O)/glad.o                 \
	$(O)/imgui.opp              \
	$(O)/imgui_draw.opp         \
	$(O)/imgui_impl_glfw.opp    \
	$(O)/imgui_impl_opengl3.opp \
	$(O)/imgui_stdlib.opp       \
	$(O)/imgui_tables.opp       \
	$(O)/imgui_widgets.opp      \
	$(O)/imgui_demo.opp

EMBED_OBJS =          \
	$(O)/images.o     \
	$(O)/shaders.opp  \
	$(O)/theatres.opp \
	$(O)/models.opp

GRAPHX_OBJS =              \
	$(O)/g_math.opp        \
	$(O)/g_jolt.opp        \
	$(O)/g_device.opp      \
	$(O)/g_actor.opp       \
	$(O)/g_devices.opp     \
	$(O)/g_actors.opp      \
	$(O)/g_imgui.opp       \
	$(O)/t_interpreter.opp \
	$(O)/g_theatre.opp     \
	$(O)/r_common.opp      \
	$(O)/r_rendering.opp

OBJS =             	\
	$(EXT_OBJS)    	\
	$(EMBED_OBJS)  	\
	$(GRAPHX_OBJS)

WOBJS = $(subst .o,.wo,$(OBJS))

I = $(SRC)/images
IMAGES_C = $(SRC)/images.c
IMAGES_H = $(SRC)/include/images.h
IMGS = $(wildcard $(I)/*)

S = $(SRC)/shaders
SHADERS_C = $(SRC)/shaders.cpp
SHADERS_H = $(SRC)/include/shaders.hpp
SHDRS = $(wildcard $(S)/*.glsl)

T = $(SRC)/theatres
THEATRES_C = $(SRC)/theatres.cpp
THEATRES_H = $(SRC)/include/theatres.hpp
THTRS = $(wildcard $(T)/*.gt)

M = $(SRC)/models
MODELS_C = $(SRC)/models.cpp
MODELS_H = $(SRC)/include/models.hpp
MDLS = $(wildcard $(M)/*.obj)
MTLS = $(wildcard $(M)/*.mtl)


PHONY = obj_testing all clean dirty_clean clean_resources embed_resources rebuild_images rebuild_shaders rebuild_theatres rebuild_models compile_commands debug release linux windows test build

all: release linux windows

embed_resources:
	-make -s $(IMAGES_C) $(SHADERS_C) $(THEATRES_C) $(MODELS_C)

clean_resources:
	-rm -f $(IMAGES_C) $(IMAGES_H) $(SHADERS_C) $(SHADERS_H) $(THEATRES_C) $(THEATRES_H) $(MODELS_H) $(MODELS_C)

dirty_clean:
	-rm -f $(O)/*.tmp
	-rm -f $(O)/main.*
	-rm -f $(GRAPHX_OBJS)
	-rm -f $(WOBJS)

clean: clean_resources embed_resources
	-rm -f $(OBJS)
	-rm -f $(WOBJS)
	-rm -f $(O)/main.*
	-rm -f $(O)/*.tmp
	-rm -f $(O)/$(LINUX)
	-rm -f $(O)/$(WINDOWS)
	-rm -f $(O)/GraphXDebug
	-rm -f $(O)/GraphXDebug.exe

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

compile_commands:
	$(eval GRAPHXFLAGS += -D GRAPHX_DEBUG)

test: eval_test
	$(info GraphX Will Test-Run After Compiling)

eval_test:
	$(eval TESTRUN_LINUX = $(TEST_LINUX))
	$(eval TESTRUN_WINDOWS = $(TEST_WINDOWS))

debug: rebuild_shaders rebuild_theatres
	$(info Version: Debug)
	$(eval LINUX := GraphXDebug)
	$(eval WINDOWS := GraphXDebug.exe)
	$(eval GRAPHXFLAGS += -D GRAPHX_DEBUG)
	-rm -f build/*.tmp

release: clean_resources embed_resources
	$(info Version: Release)
	-rm -f build/*.tmp

linux: NAME = $(LINUX)
linux: $(OBJS) $(O)/main.opp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(OBJS) $(O)/main.opp -o $(O)/$(NAME) $(LIBS)
	$(TESTRUN_LINUX) $(O)/$(NAME)

windows: NAME = $(WINDOWS)
windows: GRAPHXFLAGS += -D GRAPHX_WINDOWS
windows: $(WOBJS) $(O)/main.wopp
	$(WCXX) $(WCXXFLAGS) $(LDFLAGS) $(WOBJS) $(O)/main.wopp -o $(O)/$(NAME) $(WLIBS)
	$(TESTRUN_WINDOWS) $(O)/$(NAME)

$(IMAGES_C): $(IMAGES_H)
	$(foreach file,$(IMGS),$(shell xxd -b -n $(file:$(I)/%=%) -i $(file) >> $(IMAGES_C)))

$(IMAGES_H):
	$(shell printf "#ifndef GRAPHX_EMBEDDED_IMAGES\n#define GRAPHX_EMBEDDED_IMAGES\n#include <string>\n" > $(IMAGES_H))
	$(foreach filename,$(IMGS), $(shell printf "\n#define $(subst .,_,$(basename $(filename:$(I)/%=%))) std::string(\"$(subst .,_,$(filename:$(I)/%=%))\")\nextern unsigned char $(subst .,_,$(filename:$(I)/%=%))[];\nextern unsigned int $(subst .,_,$(filename:$(I)/%=%))_len;\n" >> $(IMAGES_H)))
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
	$(foreach theatre,$(THTRS),$(shell printf ",{$(shell printf $(theatre) | grep -P --only-matching '(.+\/)+\K[0-9]+'), std::string{R\"~(" >> $(THEATRES_C) && cat $(theatre) >> $(THEATRES_C) && printf ")~\"}}" >> $(THEATRES_C)))
	$(shell sed 's/^{,{/{{/' -i $(THEATRES_C))
	$(shell printf "\n};" >> $(THEATRES_C))

$(MODELS_H):
	$(shell printf "#ifndef GRAPHX_MODELS\n#define GRAPHX_MODELS\n#include <string>\n" > $(MODELS_H))
	$(foreach graphxmodel,$(wildcard $(M)/*.graphxmodel),$(shell printf "#include \"../$(M:$(SRC)/%=%)/$(graphxmodel:$(M)/%=%)\"\n" >> $(MODELS_H)))
	$(foreach model,$(MDLS),$(shell printf "\n#define $(basename $(model:$(M)/%=%))_MODEL std::string(\"$(subst .,_,$(model:$(M)/%=%))\")\nextern std::string $(subst .,_,$(model:$(M)/%=%));\n" >> $(MODELS_H)))
	$(foreach material,$(MTLS),$(shell printf "extern std::string $(subst .,_,$(material:$(M)/%=%));\n" >> $(MODELS_H)))
	$(shell printf "#endif" >> $(MODELS_H))

$(MODELS_C): $(MODELS_H)
	$(shell printf "#include <string>\n" > $(MODELS_C))
	$(foreach model,$(MDLS),$(shell printf "std::string $(subst .,_,$(model:$(M)/%=%)) = R\"~(\n" >> $(MODELS_C) && cat $(model) >> $(MODELS_C) && printf "\n)~\";\n" >> $(MODELS_C)))
	$(foreach material,$(MTLS),$(shell printf "std::string $(subst .,_,$(material:$(M)/%=%)) = R\"~(\n" >> $(MODELS_C) && cat $(material) >> $(MODELS_C) && printf "\n)~\";\n" >> $(MODELS_C)))

$(O)/%.opp: $(SRC)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(O)/%.o: $(SRC)/%.c
	$(CC) $(CCFLAGS) $(INCLUDES) -c $< -o $@

$(O)/%.wopp: $(SRC)/%.cpp
	$(WCXX) $(WCXXFLAGS) $(WINCLUDES) -c $< -o $@

$(O)/%.wo: $(SRC)/%.c
	$(WCC) $(WCCFLAGS) $(WINCLUDES) -c $< -o $@

$(O)/%.gch: $(SRC)/%.hpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@