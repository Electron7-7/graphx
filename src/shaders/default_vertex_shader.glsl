#version 460 core
layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec2 vertex_texture_coords;

out vec2 texture_coordinates;

uniform mat4 model;
uniform mat4 camera_view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * camera_view * model * vec4(vertex_position, 1.0);
	texture_coordinates = vertex_texture_coords;
};