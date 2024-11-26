#include <string>
#include "sanity.hpp"

#ifndef GRAPHX_SHADER_LIB
#define GRAPHX_SHADER_LIB
#define GLSHADER_TYPE_VERTEX 0
#define GLSHADER_TYPE_FRAGMENT 1
#define GLSHADER_TYPE_PROGRAM 2

class GLShader
{
public:
	unsigned int ID;

	GLShader(const char* vertex_path, const char* fragment_path);

	void use();

	void setBool(const std::string &name, bool value) const;
	void setInt(const std::string &name, int value) const;
	void setFloat(const std::string &name, float value) const;
	void setMatrix(const std::string &name, glm::mat4 value) const;

private:
	void shaderErrorHandler(int thing, int type);
};

#endif