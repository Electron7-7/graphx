#version 460 core
layout (location = 0) in vec3 _vertex_position;
layout (location = 1) in vec2 _vertex_texture_coordinate;

out vec2 vertex_texture_coordinate;

uniform mat4 model;
uniform mat4 camera_view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * camera_view * model * vec4(_vertex_position, 1.0);
	vertex_texture_coordinate = _vertex_texture_coordinate;
};