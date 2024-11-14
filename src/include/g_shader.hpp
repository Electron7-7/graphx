#include <g_common.hpp>

#ifndef SHADER_LIB
#define SHADER_LIB
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

class Shader
{
public:
	unsigned int ID;

	Shader(const char* vertex_path, const char* fragment_path);

	void use();

	void setBool(const std::string &name, bool value) const;
	void setInt(const std::string &name, int value) const;
	void setFloat(const std::string &name, float value) const;

	void setMatrix(const std::string &name, glm::mat4 value) const;

private:
	void shaderErrorHandler(unsigned int shader, bool is_program = false);
};
#endif