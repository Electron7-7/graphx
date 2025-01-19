# CMake is currently only used for building the Windows application. Why?
# Well, I didn't want to use CMake because I had no experience with either
# CMake or Make, and CMake was the more daunting one to me at the time.
# This was fine and dandy until I tried to compile on Windows... without
# Visual Studio. Look, I have a personal gripe with VS and if I'm not
# being forced to use it in a workplace environment, I want to avoid it
# at all costs (because I'm pretentious). The drawback to that (aside from
# the headache of doing everything manually) is that Makefiles on Windows
# (like most things on Windows) are fucking insane and you can either go
# fucking insane yourself, or use CMake.
# I chose CMake.
# Eventually, I do want to move linux building to CMake so everything's in one
# place, but let me just say that I much prefered the cleanliness of a single
# Makefile to the clutter of CMake's generated files and folders.
# Just sayin'.

cmake_minimum_required(VERSION 3.30)
project(GraphX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Xlinker")

set(SRC src)
set(DEPENDENCIES ${SRC}/windows_dependencies)
set(GLFW_LIB ../${DEPENDENCIES}/GLFW/glfw3.lib)

set(SOURCES
	${SRC}/glad.c
	${SRC}/r_common.cpp
	${SRC}/r_renderer.cpp
	${SRC}/g_actors.cpp
	${SRC}/g_theatre.cpp
	${SRC}/main.cpp
)

include_directories(${SRC}/include ${DEPENDENCIES})

add_library(glfw3 STATIC IMPORTED)
set_target_properties(glfw3 PROPERTIES IMPORTED_LOCATION ${GLFW_LIB})

add_executable(graphx_windows ${SOURCES})

target_link_libraries(graphx_windows glfw3)
set_property(TARGET graphx_windows PROPERTY CXX_STANDARD 20)