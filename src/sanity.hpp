// sanity.hpp - Various pre-processor includes for the sake of my sanity
/*
12/11/24 Note: This is probably what makes compiling take so fucking long, since every god damned header and source file includes this... oh wait...
no, I don't think that this should cause compiling to take longer since the #ifndef means that these headers only actually get copied by the preprocessor
once... I think, at least. This could just be a misnomer/not how that works(?) I need to make sure.
*/
#ifndef GRAPHX_SANITY
#define GRAPHX_SANITY
#ifdef WIN32
#include <windows.h>
#endif
#ifdef GRAPHX_WINDOWS
#include <windows.h>
#include <libloaderapi.h>
inline std::string getBinaryPath()
{
	char out_path[MAX_PATH] = {0};
	GetModuleFileNameA(NULL, out_path, MAX_PATH);
	std::string buffer = "";
	bool filename_removed = false;
	for(int i = (sizeof(out_path)/sizeof(out_path[0])) ; i >= 0 ; i--)
	{
		if(out_path[i] == 0x00)
			continue;
		if(((out_path[i] == '\\' && !(out_path[i+1] == ' ')) || out_path[i] == '/') && !filename_removed)
		{
			buffer = "";
			filename_removed = true;
		}

		buffer += out_path[i];
	}

	std::string new_buffer = "";

	for(int i = buffer.length() - 1 ; i > 0 ; i--)
		new_buffer += buffer[i];

	return new_buffer + "\\";
}
#define BINARY_PATH getBinaryPath()
#define EXTERNAL_THEATRES_DIRECTORY std::string("theatres")
#else
#include <filesystem>
#define BINARY_PATH std::filesystem::read_symlink(std::filesystem::path("/proc/self/exe")).remove_filename().string()
#define EXTERNAL_THEATRES_DIRECTORY std::string("theatres")
#endif

static std::string binary_path = BINARY_PATH;
static std::string theatres_directory = std::string(BINARY_PATH) + EXTERNAL_THEATRES_DIRECTORY;

#include <iostream>
#include <images.h>
#include <shaders.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/ext.hpp>

#define PRINT_MARKER std::cout << std::endl << "[=======================================]" << std::endl;
#define PRINTERR(thing) std::cerr << std::endl << "[ERROR]    " << thing << std::endl;
#define PRINTIMPORTANT(thing) for(int i = 0 ; i < 10 ; i++) { std::cout << std::endl << "[!]    " << thing; }; std::cout << std::endl;
#define PRINTNOTE(thing) std::cout << std::endl << "[NOTE]    " << thing << std::endl;
#ifdef GRAPHX_DEBUG
#define TICK(tick) std::cout << "[TICK #" << tick << "]" << std::endl;
#define PRINT(thing) std::cout << thing;
#define PRINTLN(thing) std::cout << std::endl << thing << std::endl;
#define PRINTDEBUG(thing) std::cout << "[DEBUG]    " << thing << std::endl;
#define JOLTDEBUG(thing) std::cout << "[JOLT]    " << thing << std::endl;
#define IS_DEBUG true
#else
#define TICK(tick);
#define PRINT(thing);
#define PRINTLN(thing);
#define PRINTDEBUG(thing); // Might change this to print nothing, but I like having a little marker to remind/notify me that there are debug messages in the code somewhere
#define JOLTDEBUG(thing);
#define IS_DEBUG false
#endif
#endif