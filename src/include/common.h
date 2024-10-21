#ifndef ALL_INCLUDES
#define ALL_INCLUDES
	#include <glad/glad.h>
	#include <GLFW/glfw3.h>
	#include <string>
	#include <math.h>
#endif

#ifndef FUNCTIONS_COMMON_INCLUDED
#define FUNCTIONS_COMMON_INCLUDED

void shaderErrorHandler(unsigned int shader, bool is_program = false);

#endif

#ifndef SHADER_LIB
#define SHADER_LIB

class Shader
{
public:
	unsigned int ID;

	Shader(const char* vertex_path, const char* fragment_path);

	void use();

	void setBool(const std::string &name, bool value) const;
	void setInt(const std::string &name, int value) const;
	void setFloat(const std::string &name, float value) const;
};
#endif