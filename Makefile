CXX = clang++
CC = clang

WCXX = x86_64-w64-mingw32-g++
WCC = x86_64-w64-mingw32-gcc

CXXFLAGS = -g -Wall -std=c++20 -fsanitize=address $(JOLTFLAGS) $(GRAPHXFLAGS)
CCFLAGS = -g -Wall

WCXXFLAGS = -g -Wall -std=c++20 -static -mwindows $(JOLTFLAGS) $(GRAPHXFLAGS)
WCCFLAGS = -g -Wall -static -mwindows

GRAPHXFLAGS = -D GRAPHX_COMPILING -D GRAPHX_DEBUG
JOLTFLAGS = -D JPH_PROFILE_ENABLED -D JPH_OBJECT_STREAM -D JPH_DEBUG_RENDERER

INCLUDES = -I src/include
WINCLUDES = -I src/include -I src/windows_dependencies/include

LIBS = -l glfw -L src/lib -l:libJolt.a
WLIBS = -L src/windows_dependencies/lib src/windows_dependencies/lib/libJolt.a src/windows_dependencies/lib/lib-mingw-w64/libglfw3.a -l gdi32

SRC := src

O = build

OBJS = 						\
	$(O)/glad.o				\
	$(O)/t_interpreter.opp	\
	$(O)/j_common.opp		\
	$(O)/r_common.opp		\
	$(O)/r_renderer.opp		\
	$(O)/g_math.opp			\
	$(O)/g_actors.opp		\
 	$(O)/g_theatres.opp

CWOBJS = $(OBJS:.o=.wo)
WOBJS = $(CWOBJS:.opp=.wopp)

LINUX = GraphX_$(shell uname -s)_$(shell uname -r)_$(shell uname -m)
WINDOWS = GraphX_Windows_x86_64.exe

I = $(SRC)/images
IMAGES_C = $(SRC)/images.c
IMAGES_H = $(SRC)/include/images.h
IMGS = \
	$(I)/COMP04_5.png			\
	$(I)/COMP04_5_SPECULAR.jpg	\
	$(I)/LIGHT.jpg				\
	$(I)/MISSING.jpg			\
	$(I)/MISSING_SPECULAR.jpg	\
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

all: build build_windows

clean: clean_resources
	rm -f build/*
	make -s embed_resources

clean_resources:
	$(shell rm -f $(IMAGES_C) $(IMAGES_H) $(SHADERS_C) $(SHADERS_H) $(THEATRES_C) $(THEATRES_H))

embed_resources: $(IMAGES_C) $(SHADERS_C) $(THEATRES_C)

compile_commands:
	$(eval GRAPHXFLAGS = -D GRAPHX_DEBUG)

testing_interpreter: clean_resources embed_resources debug
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SRC)/t_interpreter.cpp $(SRC)/theatres.cpp $(SRC)/testing.cpp -o testing

debug:
	$(eval LINUX = GraphXDebug)

build: $(O)/$(LINUX)

linux_test:	build
	~/bin/mangohudtest $(FPS_LIMIT) $(O)/$(LINUX)

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

leftparen := (
rightparen := )

$(THEATRES_H):
	$(shell printf "#ifndef GRAPHX_EMBEDDED_THEATRES\n#define GRAPHX_EMBEDDED_THEATRES\n#include <string>\n#include <map>\n" >> $(THEATRES_H))
	$(shell printf "extern std::map<int, std::string> embedded_theatre_names;\nextern std::map<int, std::string> embedded_theatres;\n#endif" >> $(THEATRES_H))

$(THEATRES_C): $(THEATRES_H)
	$(shell printf "#include <string>\n#include <map>\nstd::map<int, std::string> embedded_theatres =\n{" >> $(THEATRES_C))
	$(foreach theatre,$(shell find $(T) -name '*.graphxtheatre'),$(shell printf ",{$(shell printf $(theatre) | grep -P --only-matching '(.+\/)+\K[0-9]+'), std::string{R\"~(\n" >> $(THEATRES_C) && cat $(theatre) >> $(THEATRES_C) && printf ")~\"}}" >> $(THEATRES_C)))
	$(shell sed 's/^{,{/{{/' -i $(THEATRES_C))
	$(shell printf "\n};" >> $(THEATRES_C))

$(O)/$(LINUX): $(O)/images.o $(O)/shaders.opp $(O)/theatres.opp $(OBJS) $(O)/main.opp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(O)/images.o $(O)/shaders.opp $(O)/theatres.opp $(OBJS) $(O)/main.opp \
	-o $(O)/$(LINUX) $(LIBS)

$(O)/images.o: $(IMAGES_C)
	$(CC) $(CCFLAGS) $(INCLUDES) -c $< -o $@

$(O)/shaders.opp: $(SHADERS_C)
	$(CC) $(CCFLAGS) $(INCLUDES) -c $< -o $@

$(O)/theatres.opp: $(THEATRES_C)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(O)/%.opp:	$(SRC)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(O)/%.o:	$(SRC)/%.c
	$(CC) $(CCFLAGS) $(INCLUDES) -c $< -o $@


build_windows: $(O)/$(WINDOWS)

windows_test: build_windows
# 	~/bin/mangohudtest $(FPS_LIMIT) $(O)/$(WINDOWS)
	wine64 $(O)/$(WINDOWS)

$(O)/$(WINDOWS): $(O)/images.wo $(O)/shaders.wopp $(WOBJS) $(O)/main.wopp
	$(WCXX) $(WCXXFLAGS) $(LDFLAGS) $(O)/images.wo $(O)/shaders.wopp $(WOBJS) $(O)/main.wopp \
	-o $(O)/$(WINDOWS) $(WLIBS)

$(O)/%.wopp: $(SRC)/%.cpp
	$(WCXX) $(WCXXFLAGS) $(WINCLUDES) -c $< -o $@

$(O)/%.wo: $(SRC)/%.c
	$(WCC) $(WCCFLAGS) $(WINCLUDES) -c $< -o $@

$(O)/images.wo: $(IMAGES_C)
	$(WCC) $(WCCFLAGS) $(WINCLUDES) -c $< -o $@

$(O)/shaders.wopp: $(SHADERS_C)
	$(WCC) $(WCCFLAGS) $(WINCLUDES) -c $< -o $@