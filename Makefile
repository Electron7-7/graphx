CXX = clang++
CC = clang

NAME := graphx
TEST_NAME := graphx.test

SRC_DIR := src

INCLUDES = -Isrc/include -I/usr/include/freetype2
LINKER_FLAGS := -lglfw

SRCS := src/main.cpp src/glad.c

# FPS limit for custom mangohud test run
FPS_LIMIT := 60


test:
	$(CXX) $(LINKER_FLAGS) $(INCLUDES) $(SRCS) -o $(TEST_NAME)

testrun: test
# 	./$(TEST_NAME) # Normal run without mangohud
	~/bin/mangohudtest $(FPS_LIMIT) ./$(TEST_NAME) # Run using custom mangohud command to limit FPS for test cases
	rm ./$(TEST_NAME)

.PHONY: build test testrun clean