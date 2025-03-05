#version 460 core
layout (location = 0) in vec3 _vertex_position;
layout (location = 1) in vec3 _vertex_normal;
layout (location = 2) in vec2 _vertex_texture_coordinate;
layout (location = 3) in vec3 _vertex_colors;

out vec3 fragment_position;
out vec2 texture_coordinate;
out vec3 vertex_colors;
out vec3 normal;

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;
uniform mat3 normal_matrix;

void main()
{
	gl_Position = projection_matrix * view_matrix * model_matrix * vec4(_vertex_position, 1.0);
	texture_coordinate = _vertex_texture_coordinate;
	fragment_position = vec3(model_matrix * vec4(_vertex_position, 1.0f)); // Transforming vertex position from local to global coordinates
	normal = normal_matrix * _vertex_normal;
	vertex_colors = _vertex_colors;
};