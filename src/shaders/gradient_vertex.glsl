#version 460 core
layout (location = 0) in vec3 _vertex;

out vec3 texture_coordinates;

uniform mat4 projection_matrix;
uniform mat4 view_matrix;

void main()
{
	texture_coordinates = _vertex;
	gl_Position = projection_matrix * view_matrix * vec4(_vertex, 1.0f);
}