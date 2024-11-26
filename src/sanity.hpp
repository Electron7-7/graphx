// sanity.hpp - Various pre-processor includes for the sake of my sanity
#ifndef GL_INCLUDES
#define GL_INCLUDES
	#include <glad/glad.h>
	#include <glm/fwd.hpp>
	#include <GLFW/glfw3.h>
	#include <glm/glm.hpp>
	#include <glm/gtc/matrix_transform.hpp>
	#include <glm/gtc/type_ptr.hpp>
#endif

#ifndef ERROR_HANDLING_FLAGS
#define ERROR_HANDLING_FLAGS
	#define ERROR_GLFW_WINDOW 0
	#define ERROR_GLAD_INIT 1
#endif