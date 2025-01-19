CXX = clang++
CC = clang

CXXFLAGS = -g -Wall -std=c++20
CFLAGS = -g -Wall

LIBS = -l glfw

O = build

OBJS = \
	$(O)/glad.o				\
	$(O)/r_common.opp		\
	$(O)/r_renderer.opp		\
	$(O)/g_actors.opp		\
 	$(O)/g_theatre.opp

SRC_DIR := src

INCLUDES = -I src/include


# FPS limit for custom mangohud test run (value <= 0 -> uncapped framerate)
FPS_LIMIT := 60


all:	$(O)/graphx_linux

build:	$(O)/graphx_linux

clean:
	rm -f *.o *.opp
	rm -f build/*

test:	$(O)/graphx_linux
	~/bin/mangohudtest $(FPS_LIMIT) ./build/graphx_linux

$(O)/graphx_linux:	$(OBJS) $(O)/main.opp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(OBJS) $(O)/main.opp \
	-o $(O)/graphx_linux $(LIBS)

$(O)/%.opp:	./src/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(O)/%.o:	./src/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@