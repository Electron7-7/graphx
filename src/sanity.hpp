// sanity.hpp - Various pre-processor includes for the sake of my sanity
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