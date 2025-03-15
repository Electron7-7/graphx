// sanity.hpp - Various pre-processor includes for the sake of my sanity
/*
12/11/24 Note: This is probably what makes compiling take so fucking long, since every god damned header and source file includes this... oh wait...
no, I don't think that this should cause compiling to take longer since the #ifndef means that these headers only actually get copied by the preprocessor
once... I think, at least. This could just be a misnomer/not how that works(?) I need to make sure.
*/
#ifndef GRAPHX_SANITY
#define GRAPHX_SANITY
#ifdef GRAPHX_WINDOWS
#include <windows.h>
#include <string>
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

#include "sanity_printouts.hpp"
#endif