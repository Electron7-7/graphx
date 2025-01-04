// sanity.hpp - Various pre-processor includes for the sake of my sanity
/*
12/11/24 Note: This is probably what makes compiling take so fucking long, since every god damned header and source file includes this... oh wait...
no, I don't think that this should cause compiling to take longer since the #ifndef means that these headers only actually get copied by the preprocessor
once... I think, at least. This could just be a misnomer/not how that works(?) I need to make sure.
*/

#ifndef GRAPHX_FUCKING_FILEPATHS
#define GRAPHX_FUCKING_FILEPATHS
	#include <string>
	#ifdef _WIN32
	// Change the C:/Users/Chea Sextillion/Desktop/Github/graphx/ filepath to wherever you downloaded the github repo to
	#define SRC_DIR(relative_filepath) (std::string("C:/Users/Chea Sextillion/Desktop/Github/graphx/") + relative_filepath)
	#endif

	#ifdef linux
	// At least for me, the relative filepath works fine on Linux, but you may have to copy what I did for Windows, up there ^
	#define SRC_DIR(relative_filepath) (relative_filepath)
	#endif
#endif

#ifndef GRAPHX_DEBUGGING
#define GRAPHX_DEBUGGING
	#include <iostream>
	#define PRINT_MARKER (std::cout << std::endl << "[=======================================]" << std::endl)
	#define PRINT(thing) (std::cout << std::endl << thing << std::endl << std::endl)
#endif

#ifndef GL_INCLUDES
#define GL_INCLUDES
	#include <glad/glad.h>
	#include <GLFW/glfw3.h>
	#include <glm/fwd.hpp>
	#include <glm/glm.hpp>
	#include <glm/gtc/matrix_transform.hpp>
	#include <glm/gtc/type_ptr.hpp>
	#include <glm/gtc/quaternion.hpp>
	#include <glm/ext.hpp>
	#include <stb_image.h>
#endif

#ifndef GLM_ENABLE_EXPERIMENTAL
#define GLM_ENABLE_EXPERIMENTAL
	#include <glm/gtx/string_cast.hpp>
#endif