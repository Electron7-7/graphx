#version 460 core
out vec4 FragColor;

in vec2 texture_coordinate;
in vec3 vertex_color;

uniform sampler2D texture_one;

void main()
{
	// FragColor = texture(texture_one, texture_coordinate);
	FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
};