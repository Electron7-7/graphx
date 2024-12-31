#version 460 core
out vec4 FragColor;

in vec2 vertex_texture_coordinate;

uniform sampler2D texture_one;

void main()
{
	FragColor = texture(texture_one, vertex_texture_coordinate);
};