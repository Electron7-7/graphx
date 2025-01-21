CXX = clang++
CC = clang

WCXX = x86_64-w64-mingw32-g++
WCC = x86_64-w64-mingw32-gcc

CXXFLAGS = -g -Wall -std=c++20 -D GRAPHX_COMPILING
CFLAGS = -g -Wall

WCXXFLAGS = -g -Wall -std=c++20 -static -mwindows
WCFLAGS = -g -Wall -static -mwindows

INCLUDES = -I src/include
WINCLUDES = -I src/include -I src/windows_dependencies/include

LIBS = -l glfw
WLIBS = -L src/windows_dependencies/lib src/windows_dependencies/lib/lib-mingw-w64/libglfw3.a -l gdi32

SRC := src

O = build

OBJS = \
	$(O)/glad.o				\
	$(O)/r_common.opp		\
	$(O)/r_renderer.opp		\
	$(O)/g_actors.opp		\
 	$(O)/g_theatre.opp

CWOBJS = $(OBJS:.o=.wo)
WOBJS = $(CWOBJS:.opp=.wopp)

LINUX = graphx_linux
WINDOWS = graphx_windows_x86_64.exe

I = $(SRC)/images

I_C = $(SRC)/images.c
I_H = $(SRC)/include/images.h

IMGS = \
	$(I)/COMP04_5.png			\
	$(I)/COMP04_5_SPECULAR.jpg	\
	$(I)/LIGHT.jpg				\
	$(I)/MISSING.jpg			\
	$(I)/MISSING_SPECULAR.jpg	\
	$(I)/NO_TEXTURE.jpg

FPS_LIMIT = 60		# FPS limit for mangohud (FPS_LIMIT <= 0 results in an uncapped framerate)

all: build

clean:
	rm -f build/* $(I_C) $(I_H)

build: $(O)/$(LINUX)

embed_images: $(I_C)

$(I_C):
	$(foreach file,$(IMGS),$(shell xxd -b -n $(file:$(I)/%=%) -i $(file) >> $(I_C)))
	$(shell printf "#ifndef GRAPHX_EMBEDDED_IMAGES\n#define GRAPHX_EMBEDDED_IMAGES\n" >> $(I_H))
	$(foreach filename,$(IMGS), $(shell printf "\nextern unsigned char $(subst .,_,$(filename:$(I)/%=%))[];\nextern unsigned int $(subst .,_,$(filename:$(I)/%=%))_len;\n" >> $(I_H)))
	$(shell printf "#endif" >> $(I_H))
	

linux_test:	build
	~/bin/mangohudtest $(FPS_LIMIT) $(O)/$(LINUX)

$(O)/$(LINUX): $(O)/images.o $(OBJS) $(O)/main.opp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(O)/images.o $(OBJS) $(O)/main.opp \
	-o $(O)/$(LINUX) $(LIBS)

$(O)/images.o: $(I_C)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(O)/%.opp:	$(SRC)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(O)/%.o:	$(SRC)/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

windows: $(O)/$(WINDOWS)

windows_test: $(O)/$(WINDOWS)
	~/bin/mangohudtest $(FPS_LIMIT) $(O)/$(WINDOWS)

$(O)/$(WINDOWS): $(WOBJS) $(O)/main.wopp
	$(WCXX) $(WCXXFLAGS) $(LDFLAGS) $(WOBJS) $(O)/main.wopp \
	-o $(O)/$(WINDOWS) $(WLIBS)

$(O)/%.wopp: $(SRC)/%.cpp
	$(WCXX) $(WCXXFLAGS) $(WINCLUDES) -c $< -o $@

$(O)/%.wo: $(SRC)/%.c
	$(WCC) $(WCFLAGS) $(WINCLUDES) -c $< -o $@