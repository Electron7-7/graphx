IS_WINDOWS := yes # leave empty when on linux

CXX = $(if $(IS_WINDOWS), clang-cl, clang++)
CC = $(if $(IS_WINDOWS), clang-cl, clang)

CFLAGS = -g #-Wall
CXXFLAGS = -g /EHa #-Wall

LIBS_LINUX := -l glfw
LIBS_WIN := /link "C:\Users\Chea Sextillion\include\glfw\lib-vc2022\glfw3.lib" /MD "C:\Users\Chea Sextillion\include\glfw\lib-vc2022\glfw3.dll" 
LIBS = $(if $(IS_WINDOWS), $(LIBS_WIN), $(LIBS_LINUX))

O = build

OBJS = \
	$(O)/glad.o				\
	$(O)/g_math.opp			\
	$(O)/r_common.opp		\
	$(O)/r_renderer.opp		\
	$(O)/g_actors.opp		\
	$(O)/g_spaces.opp

SRC_DIR := src

INCLUDES_WIN := -I "src\\include" -I "C:\\Users\\Chea Sextillion\\include" -I "C:\\Users\\Chea Sextillion\\include\\glfw\\include"
INCLUDES_LINUX := -I src/include #-I/usr/include/freetype2
INCLUDES = $(if $(IS_WINDOWS), $(INCLUDES_WIN), $(INCLUDES_LINUX))


# FPS limit for custom mangohud test run
FPS_LIMIT := 60


all:	$(O)/graphx_linux

$(O)/graphx_linux:	$(OBJS) $(O)/main.opp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(OBJS) $(O)/main.opp \
	-o $(O)/graphx_linux $(LIBS)

$(O)/%.opp:	src/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(O)/%.o:	src/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f *.o *.opp
	rm -f build/*

windows_clean:
	move "build\\.gitignore" ".\\"
	del /Q "build\\*"
	move ".gitignore" "build\\"

test:	$(O)/graphx_linux
	~/bin/mangohudtest $(FPS_LIMIT) ./build/graphx_linux

# removed cleantest. edit the sublime-project on linux and make the pristine test build just call "make clean && make test && make clean"