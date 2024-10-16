# NAME=graphx

# CXX = clang++ -v
# LD = clang++ -v

# LIBS = OpenGL glut GLU m
# LDFLAGS = $(addprefix -l, $(LIBS))

# SRC_DIR := ./src
# SRC_FILES := $(shell find $(SRC_DIR) -name '*.cpp')

# BUILD_DIR := ./build
# OBJS = $(BUILD_DIR)/$(SRC_FILES:.cpp=.o)

# $(NAME): $(OBJS)
# 	$(CXX) -o $(NAME) $(OBJS) $(LDFLAGS)

# $(OBJS): $(SRC_FILES)
# 	mkdir -p $(BUILD_DIR)/$(SRC_DIR)
# 	$(CXX) -o $@ -c $<

# test: $(NAME)
# 	./$(NAME)
# 	make clean

# clean:
# 	rm -rf $(BUILD_DIR)
# 	rm ./$(NAME)

test:
	 g++ -lglfw -Isrc/client/include ./src/client/temp_main.cpp ./src/client/glad.c -o test
	./test
	rm ./test

.PHONY: test