// sanity.hpp - Various pre-processor includes for the sake of my sanity
/*
12/11/24 Note: This is probably what makes compiling take so fucking long, since every god damned header and source file includes this... oh wait...
no, I don't think that this should cause compiling to take longer since the #ifndef means that these headers only actually get copied by the preprocessor
once... I think, at least. This could just be a misnomer/not how that works(?) I need to make sure.
*/

#ifndef GRAPHX_DEBUGGING
#define GRAPHX_DEBUGGING
	#include <iostream>
	#define PRINT_MARKER (std::cout << "\n[=======================================]\n")
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
	#include <glm/ext.hpp>
	#include <stb_image.h>
#endif

#ifndef GLM_ENABLE_EXPERIMENTAL
#define GLM_ENABLE_EXPERIMENTAL
	#include <glm/gtx/string_cast.hpp>
#endif