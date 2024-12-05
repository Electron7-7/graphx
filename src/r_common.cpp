#include "sanity.hpp"
#include "r_common.hpp"
#include "r_main.hpp"
#include <iostream>
#include <vector>
#include <iterator>
#include <algorithm>
//
// Texture Function
//
// HEY! IMPORTANT!! This function will crash the program if there are
// missing or otherwise unable-to-load textures! The remedy is to
// have a default missing texture that is ALWAYS able to be loaded!
// Todo: INCLUDE THAT!
// Todo: go from generating one texture per one filepath to n textures per n filepaths (and returning their pointers)
GLuint T_GenerateTexture(const char* filepath)
{
	unsigned int t_texture;
	glGenTextures(1, &t_texture);
	glBindTexture(GL_TEXTURE_2D, t_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, 16);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	int t_width, t_height, t_channels;
	unsigned char *t_data = stbi_load(filepath, &t_width, &t_height, &t_channels, 0);

	// Replace if else with try catch?
	if(!t_data)
		std::cerr << "Failed to load texture!" << std::endl;

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, t_width, t_height, 0, GL_RGB, GL_UNSIGNED_BYTE, t_data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(t_data);

	return t_texture;
}

Mesh::Mesh(std::vector<Vertex> vertices, const unsigned int* indices)
{
	int vertex_array_size = vertices.size() * 5;
	std::vector<GLfloat> vertex_array;
	vertex_array.reserve(vertex_array_size);
	
	for(unsigned int i = 0 ; i < vertices.size() ; i++) // Keep an eye out for overflow/underflow; idk but this seems fishy
	{
		glm::vec3 new_pos = vertices[i].getPosition();
		glm::vec2 new_coord = vertices[i].getTextureCoordinate();
		vertex_array.insert(vertex_array.begin(), );
	}

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertex_array_size, &vertex_array[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	indices_size = sizeof(indices);
}

void Mesh::draw()
{
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}