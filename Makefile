# IS_WINDOWS := yes # comment out when on linux

CXX = clang++
CC = clang

CFLAGS = -g -Wall
CXXFLAGS = -g -Wall -std=c++20

LIBS = -l glfw

O = build

OBJS = \
	$(O)/glad.o				\
	$(O)/g_math.opp			\
	$(O)/r_common.opp		\
	$(O)/r_renderer.opp		\
	$(O)/g_actors.opp		\
	$(O)/g_theatre.opp

SRC_DIR := src

INCLUDES = -I src/include #-I/usr/include/freetype2


# FPS limit for custom mangohud test run
FPS_LIMIT := 60


all:	$(O)/graphx_linux

build:	$(O)/graphx_linux

clean:
	rm -f *.o *.opp
	rm -f build/*

cleanish_test:
	rm -f build/main.opp
	make test

test:	$(O)/graphx_linux
	~/bin/mangohudtest $(FPS_LIMIT) ./build/graphx_linux

$(O)/graphx_linux:	$(OBJS) $(O)/main.opp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(OBJS) $(O)/main.opp \
	-o $(O)/graphx_linux $(LIBS)

$(O)/%.opp:	src/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(O)/%.o:	src/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# removed cleantest. edit the sublime-project on linux and make the pristine test build just call "make clean && make test && make clean"

windows_clean:
	move "build\\.gitignore" ".\\"
	del /Q "build\\*"
	move ".gitignore" "build\\"