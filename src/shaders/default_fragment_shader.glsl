#version 460 core
out vec4 FragColor;

in vec2 texture_coordinates;

uniform sampler2D texture_one;

void main()
{
	FragColor = texture(texture_one, texture_coordinates);
};