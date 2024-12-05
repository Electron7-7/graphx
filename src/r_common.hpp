// r_common.hpp - general purpose rendering code (i.e: Vertex & Mesh structs, code for generating Textures, etc.)
#include "sanity.hpp"
#include <vector>

#ifndef GRAPHX_RENDERING_COMMON
#define GRAPHX_RENDERING_COMMON

struct Vertex
{
	float v_position;
	float v_texture_coordinate;

	Vertex(float position, float texture_coordinate) : v_position(position), v_texture_coordinate(texture_coordinate)
	{}

	inline float getPosition() { return v_position; }
	inline float getTextureCoordinate() { return v_texture_coordinate; }
};

struct Mesh
{
	const char* texture_image;
	GLuint VAO, VBO, EBO;
	int indices_size;

	Mesh(std::vector<Vertex> vertices, const unsigned int* indices);
	
	bool isEmpty();
	void draw();
};

GLuint T_GenerateTexture(const char* filepath);
#endif