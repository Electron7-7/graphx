#include "sanity.hpp"
#include "r_common.hpp"
#include <iostream>
#include <vector>
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

Mesh::Mesh(std::vector<float> new_vertices, std::vector<unsigned int> new_indices)
{
	render_storage_commands.push_back(RenderStorageCmd(new_vertices, new_indices));
	render_indices_amount_storage.push_back(new_indices.size());
}