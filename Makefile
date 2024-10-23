CXX = clang++
CC = clang

NAME := graphx
TEST_NAME := graphx.test

SRC_DIR := src

INCLUDES = -Isrc/include
LINKER_FLAGS := -lglfw

# SRCS := $(shell find $(SRC_DIR) -name '*.cpp' -or -name '*.c')
SRCS := src/temp_main.cpp src/glad.c

# test:
# 	 g++ -lglfw -Isrc/include ./src/temp_main.cpp ./src/glad.c -o test
# 	./test
# 	rm ./test

test:
	$(CXX) $(LINKER_FLAGS) $(INCLUDES) $(SRCS) -o $(TEST_NAME)

testrun: test
	./$(TEST_NAME)
	rm ./$(TEST_NAME)

.PHONY: build test testrun clean