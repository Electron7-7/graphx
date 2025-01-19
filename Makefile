CXX = clang++
CC = clang

WCXX = x86_64-w64-mingw32-g++
WCC = x86_64-w64-mingw32-gcc

CXXFLAGS = -g -Wall -std=c++20
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

FPS_LIMIT = 60		# FPS limit for mangohud (FPS_LIMIT <= 0 results in an uncapped framerate)

all: build

clean:
	rm -f build/*

build:	$(O)/$(LINUX)

linux_test:	$(O)/$(LINUX)
	~/bin/mangohudtest $(FPS_LIMIT) $(O)/$(LINUX)

$(O)/$(LINUX):	$(OBJS) $(O)/main.opp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(OBJS) $(O)/main.opp \
	-o $(O)/$(LINUX) $(LIBS)

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