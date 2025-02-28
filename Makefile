CXX = clang++
CC = clang

CXXFLAGS = -g -Wall -std=c++20 $(JOLTFLAGS) $(GRAPHXFLAGS)
CCFLAGS = -g -Wall

INCLUDES = -I src/include
LIBS = -l glfw -L src/lib -l:libJolt.a -l:libassimp.so

ifeq ($(OS),Windows_NT) 
	WCXX = g++
	WCC = gcc
else
	WCXX = x86_64-w64-mingw32-g++
	WCC = x86_64-w64-mingw32-gcc
endif

WCXXFLAGS = -g -Wall -std=c++20 -static -mwindows -ffat-lto-objects $(JOLTFLAGS) $(GRAPHXFLAGS)
WCCFLAGS = -g -Wall -static -mwindows
WLIBS = -L src/windows_dependencies/lib/jolt-mingw-w64 -l Jolt -L src/windows_dependencies/lib/lib-mingw-w64 -l glfw3 -l gdi32
WINCLUDES = -I src/include -I src/windows_dependencies/include

JOLTFLAGS = -D JPH_PROFILE_ENABLED -D JPH_OBJECT_STREAM -D JPH_DEBUG_RENDERER
GRAPHXFLAGS = -D GRAPHX_COMPILING

LINUX = GraphX_$(shell uname -s)_$(shell uname -r)_$(shell uname -m)
WINDOWS = GraphX_Windows_x86_64.exe
NAME = ""

FPS_LIMIT = 60 # FPS limit for mangohud (FPS_LIMIT <= 0 results in an uncapped framerate)
TESTRUN_LINUX = exit 0 &&
TESTRUN_WINDOWS = exit 0 &&
TEST_LINUX = ~/bin/mangohudtest $(FPS_LIMIT) # "mangohudtest" is a custom script I wrote for test-running GraphX with MangoHUD + Gamemode. This is why I disable it on Windows
TEST_WINDOWS = # nothing here, yet

SRC := src

O = build

OBJS = 							\
	$(O)/glad.o					\
	$(O)/imgui.opp				\
	$(O)/imgui_draw.opp			\
	$(O)/imgui_impl_glfw.opp	\
	$(O)/imgui_impl_opengl3.opp	\
	$(O)/imgui_stdlib.opp		\
	$(O)/imgui_tables.opp		\
	$(O)/imgui_widgets.opp		\
	$(O)/imgui_demo.opp			\
	$(O)/images.o				\
	$(O)/shaders.opp			\
	$(O)/theatres.opp			\
	$(O)/g_math.opp				\
	$(O)/r_common.opp			\
	$(O)/j_common.opp			\
	$(O)/g_actors.opp			\
	$(O)/g_imgui.opp			\
	$(O)/t_interpreter.opp		\
	$(O)/r_renderer.opp			\
 	$(O)/g_theatres.opp

WOBJS = $(subst .o,.wo,$(OBJS))

I = $(SRC)/images
IMAGES_C = $(SRC)/images.c
IMAGES_H = $(SRC)/include/images.h
IMGS = \
	$(I)/COMP04_5.png			\
	$(I)/COMP04_5_SPECULAR.jpg	\
	$(I)/LIGHT.jpg				\
	$(I)/MISSING.jpg			\
	$(I)/SOURCE_ORANGE.png		\
	$(I)/SOURCE_LIGHT_GREY.png	\
	$(I)/NO_TEXTURE.jpg			\
	$(I)/FLAT_SPEC.jpg

S = $(SRC)/shaders
SHADERS_C = $(SRC)/shaders.cpp
SHADERS_H = $(SRC)/include/shaders.hpp
SHDRS = \
	$(S)/phong_vertex.glsl			\
	$(S)/phong_fragment.glsl

T = $(SRC)/theatres
THEATRES_C = $(SRC)/theatres.cpp
THEATRES_H = $(SRC)/include/theatres.hpp

PHONY = obj_testing all clean dirty_clean clean_resources clean_theatres embed_resources compile_commands debug release linux windows test build

all: release linux windows

clean: clean_resources embed_resources
	-rm -f build/*

clean_windows_files:
	-rm -f build/*.wo
	-rm -f build/*.wopp
	-rm -f build/*.tmp
	-rm -f build/GraphXDebug
	-rm -f build/GraphXDebug.exe

dirty_clean:
	-mkdir build/backup/
	-mv build/imgui* build/glad.o build/backup/
	-rm -f build/*.o
	-rm -f build/*.opp
	-rm -f build/*.wo
	-rm -f build/*.wopp
	-rm -f build/*.tmp
	-rm -f build/GraphXDebug
	-rm -f build/GraphXDebug.exe
	-rm -f build/$(LINUX)
	-rm -f build/$(WINDOWS)
	-mv build/backup/* build/
	-rmdir build/backup/

clean_resources:
	-rm -f $(IMAGES_C) $(IMAGES_H) $(SHADERS_C) $(SHADERS_H) $(THEATRES_C) $(THEATRES_H)

embed_resources:
	-make -s $(IMAGES_C) $(SHADERS_C) $(THEATRES_C)

clean_theatres:
	-rm -f $(THEATRES_C) $(THEATRES_H)

compile_commands:
	$(eval GRAPHXFLAGS = -D GRAPHX_DEBUG)

debug: clean_theatres embed_resources
	$(info Version: Debug)
	$(eval LINUX := GraphXDebug)
	$(eval WINDOWS := GraphXDebug.exe)
	$(eval GRAPHXFLAGS = -D GRAPHX_COMPILING -D GRAPHX_DEBUG)
	-rm -f build/*.tmp

release: clean_resources embed_resources
	$(info Version: Release)
	$(eval GRAPHXFLAGS = -D GRAPHX_COMPILING)
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

test:
	$(info GraphX Will Test-Run After Compiling)
	$(eval TESTRUN_LINUX := $(TEST_LINUX))
	$(eval TESTRUN_WINDOWS := $(TEST_WINDOWS))

$(IMAGES_C): $(IMAGES_H)
	$(foreach file,$(IMGS),$(shell xxd -b -n $(file:$(I)/%=%) -i $(file) >> $(IMAGES_C)))

$(IMAGES_H):
	$(shell printf "#ifndef GRAPHX_EMBEDDED_IMAGES\n#define GRAPHX_EMBEDDED_IMAGES\n" >> $(IMAGES_H))
	$(foreach filename,$(IMGS), $(shell printf "\nextern unsigned char $(subst .,_,$(filename:$(I)/%=%))[];\nextern unsigned int $(subst .,_,$(filename:$(I)/%=%))_len;\n" >> $(IMAGES_H)))
	$(shell printf "#endif" >> $(IMAGES_H))

$(SHADERS_C): $(SHADERS_H)
	$(shell printf "#include <string>\n" >> $(SHADERS_C))
	$(foreach file,$(shell ls $(S)),$(shell printf "std::string $(subst .,_,$(file:$(S)/%=%)) = R\"~(\n" >> $(SHADERS_C) && cat $(S)/$(file) >> $(SHADERS_C) && printf "\n)~\";\n" >> $(SHADERS_C)))

$(SHADERS_H):
	$(shell printf "#ifndef GRAPHX_EMBEDDED_SHADERS\n#define GRAPHX_EMBEDDED_SHADERS\n#include <string>\n" >> $(SHADERS_H))
	$(foreach file,$(shell ls $(S)),$(shell printf "extern std::string $(subst .,_,$(file:$(S)/%=%));\n" >> $(SHADERS_H)))
	$(shell printf "#endif" >> $(SHADERS_H))

$(THEATRES_H):
	$(shell printf "#ifndef GRAPHX_EMBEDDED_THEATRES\n#define GRAPHX_EMBEDDED_THEATRES\n#include <string>\n#include <map>\nextern std::map<int, std::string> embedded_theatres;\n#endif" >> $(THEATRES_H))

$(THEATRES_C): $(THEATRES_H)
	$(shell printf "#include <string>\n#include <map>\nstd::map<int, std::string> embedded_theatres =\n{" >> $(THEATRES_C))
	$(foreach theatre,$(shell find $(T) -name '*.gt'),$(shell printf ",{$(shell printf $(theatre) | grep -P --only-matching '(.+\/)+\K[0-9]+'), std::string{R\"~(" >> $(THEATRES_C) && cat $(theatre) >> $(THEATRES_C) && printf ")~\"}}" >> $(THEATRES_C)))
	$(shell sed 's/^{,{/{{/' -i $(THEATRES_C))
	$(shell printf "\n};" >> $(THEATRES_C))

$(O)/%.opp: $(SRC)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(O)/%.o: $(SRC)/%.c
	$(CC) $(CCFLAGS) $(INCLUDES) -c $< -o $@

$(O)/%.wopp: $(SRC)/%.cpp
	$(WCXX) $(WCXXFLAGS) $(WINCLUDES) -c $< -o $@

$(O)/%.wo: $(SRC)/%.c
	$(WCC) $(WCCFLAGS) $(WINCLUDES) -c $< -o $@