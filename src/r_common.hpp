// r_common.hpp - general purpose rendering code (i.e: Vertex & Mesh structs, code for generating Textures, etc.)
#include "sanity.hpp"
#include <vector>

#ifndef GRAPHX_RENDERING_COMMON
#define GRAPHX_RENDERING_COMMON

struct Vertex
{
	glm::vec3 v_position;
	glm::vec2 v_texture_coordinate;

	Vertex(glm::vec3 position, glm::vec2 texture_coordinate) : v_position(position), v_texture_coordinate(texture_coordinate)
	{}

	inline glm::vec3 getPosition() { return v_position; }
	inline glm::vec2 getTextureCoordinate() { return v_texture_coordinate; }
};

struct Mesh
{
	const char* texture_image;
	GLuint VAO, VBO, EBO;

	Mesh(float* vertices, GLuint* indices);
	
	bool isEmpty();
	void draw();
};

GLuint T_GenerateTexture(const char* filepath);
#endif