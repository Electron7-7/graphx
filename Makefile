CXX = clang++
CC = clang

CXXFLAGS = -g -Wall -std=c++20 $(JOLTFLAGS) $(GRAPHXFLAGS)
CCFLAGS = -g -Wall

INCLUDES = -I src/include
LIBS = -l glfw -L src/lib -l:libJolt.a

WCXX = g++
WCC = gcc

WCXXFLAGS = -g -Wall -std=c++20 -static -ffat-lto-objects -fuse-ld=lld $(JOLTFLAGS) $(GRAPHXFLAGS)
WCCFLAGS = -g -Wall -static -fuse-ld=lld
WLIBS = -L src/windows_dependencies/lib/jolt-mingw-w64 -l Jolt -L src/windows_dependencies/lib/lib-mingw-w64 -l glfw3 -l gdi32
WINCLUDES = -I src/include -I src/windows_dependencies/include

JOLTFLAGS = -D JPH_PROFILE_ENABLED -D JPH_OBJECT_STREAM -D JPH_DEBUG_RENDERER
GRAPHXFLAGS = -D GRAPHX_COMPILING

LINUX = GraphX_$(shell uname -s)_$(shell uname -r)_$(shell uname -m)
WINDOWS = GraphX_Windows_x86_64.exe
NAME =

SRC := src

O = build

OBJS = 						\
	$(O)/glad.o				\
	$(O)/images.o			\
	$(O)/shaders.opp		\
	$(O)/theatres.opp		\
	$(O)/g_actors.opp		\
	$(O)/j_common.opp		\
	$(O)/r_common.opp		\
	$(O)/r_renderer.opp		\
	$(O)/g_math.opp			\
	$(O)/t_interpreter.opp	\
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
	$(I)/MISSING_SPECULAR.jpg	\
	$(I)/SOURCE_ORANGE.png		\
	$(I)/SOURCE_LIGHT_GREY.png	\
	$(I)/NO_TEXTURE.jpg

S = $(SRC)/shaders
SHADERS_C = $(SRC)/shaders.cpp
SHADERS_H = $(SRC)/include/shaders.hpp
SHDRS = \
	$(S)/phong_vertex.glsl		\
	$(S)/phong_fragment.glsl

T = $(SRC)/theatres
THEATRES_C = $(SRC)/theatres.cpp
THEATRES_H = $(SRC)/theatres.hpp

FPS_LIMIT = 60		# FPS limit for mangohud (FPS_LIMIT <= 0 results in an uncapped framerate)

PHONY = all clean clean_resources clean_theatres embed_resources compile_commands debug release linux windows test build

all: release linux windows

clean: clean_resources embed_resources
	-rm -rf build/*

clean_linux:
	-rm -rf build/*.o
	-rm -rf build/*.opp
	-rm -rf build/GraphXDebug
	-rm -rf build/$(LINUX)

clean_windows:
	-rm -rf build/*.wo
	-rm -rf build/*.wopp
	-rm -rf build/GraphXDebug.exe
	-rm -rf build/$(WINDOWS)

clean_resources:
	-rm -f $(IMAGES_C) $(IMAGES_H) $(SHADERS_C) $(SHADERS_H) $(THEATRES_C) $(THEATRES_H)

embed_resources: $(IMAGES_C) $(SHADERS_C) $(THEATRES_C)

clean_theatres:
	-rm -f $(THEATRES_C) $(THEATRES_H)

compile_commands:
	$(eval GRAPHXFLAGS = -D GRAPHX_DEBUG)

debug: clean_theatres embed_resources
	$(info Version: Debug)
	$(eval LINUX := GraphXDebug)
	$(eval WINDOWS := GraphXDebug.exe)
	$(eval GRAPHXFLAGS = -D GRAPHX_COMPILING -D GRAPHX_DEBUG)

release: clean_resources embed_resources
	$(info Version: Release)
	$(eval GRAPHXFLAGS = -D GRAPHX_COMPILING)

linux: NAME = $(LINUX)
linux: $(OBJS) $(O)/main.opp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(OBJS) $(O)/main.opp -o $(O)/$(NAME) $(LIBS)
	~/bin/mangohudtest $(FPS_LIMIT) $(O)/$(NAME)

windows: NAME = $(WINDOWS)
windows: $(WOBJS) $(O)/main.wopp
	$(WCXX) $(WCXXFLAGS) $(LDFLAGS) $(WOBJS) $(O)/main.wopp -o $(O)/$(NAME) $(WLIBS)

$(IMAGES_C): $(IMAGES_H)
	$(foreach file,$(IMGS),$(shell xxd -b -n $(file:$(I)/%=%) -i $(file) >> $(IMAGES_C)))

$(IMAGES_H):
	$(shell printf "#ifndef GRAPHX_EMBEDDED_IMAGES\n#define GRAPHX_EMBEDDED_IMAGES\n" >> $(IMAGES_H))
	$(foreach filename,$(IMGS), $(shell printf "\nextern unsigned char $(subst .,_,$(filename:$(I)/%=%))[];\nextern unsigned int $(subst .,_,$(filename:$(I)/%=%))_len;\n" >> $(IMAGES_H)))
	$(shell printf "#endif" >> $(IMAGES_H))

$(SHADERS_C): $(SHADERS_H)
	$(shell printf "#include <string>\n" >> $(SHADERS_C))
	$(foreach file,$(SHDRS),$(shell printf "std::string $(subst .,_,$(file:$(S)/%=%)) = R\"~(\n" >> $(SHADERS_C) && cat $(file) >> $(SHADERS_C) && printf "\n)~\";\n" >> $(SHADERS_C)))

$(SHADERS_H):
	$(shell printf "#ifndef GRAPHX_EMBEDDED_SHADERS\n#define GRAPHX_EMBEDDED_SHADERS\n#include <string>\n" >> $(SHADERS_H))
	$(foreach file,$(SHDRS),$(shell printf "extern std::string $(subst .,_,$(file:$(S)/%=%));\n" >> $(SHADERS_H)))
	$(shell printf "#endif" >> $(SHADERS_H))

$(THEATRES_H):
	$(shell printf "#ifndef GRAPHX_EMBEDDED_THEATRES\n#define GRAPHX_EMBEDDED_THEATRES\n#include <string>\n#include <map>\nextern std::map<int, std::string> embedded_theatres;\n#endif" >> $(THEATRES_H))

$(THEATRES_C): $(THEATRES_H)
	$(shell printf "#include \"theatres.hpp\"\nstd::map<int, std::string> embedded_theatres =\n{" >> $(THEATRES_C))
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