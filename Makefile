CXX = clang++
CC = clang

CFLAGS = -g -Wall
CXXFLAGS = -g -Wall

LIBS = -lglfw

O = build

OBJS = \
		$(O)/glad.o


NAME := graphx
TEST_NAME := graphx.test

SRC_DIR := src

INCLUDES = -Isrc/include -I/usr/include/freetype2

SRCS := src/main.cpp src/glad.c

# FPS limit for custom mangohud test run
FPS_LIMIT := 60


all:	$(O)/graphx_linux

clean:
	rm -f *.o *.opp
	rm -f build/*
	rmdir build

$(O)/graphx_linux:	$(OBJS) $(O)/main.opp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(OBJS) $(O)/main.opp \
	-o $(O)/graphx_linux $(LIBS)

$(O)/%.opp:	src/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(O)/%.o:	src/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

test:	$(O)/graphx_linux
	~/bin/mangohudtest $(FPS_LIMIT) ./build/graphx_linux
	rm ./build/graphx_linux
	make clean