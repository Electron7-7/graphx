#version 460 core
layout (location = 0) in vec3 _vertex_position;
layout (location = 1) in vec3 _vertex_normal;
layout (location = 2) in vec2 _vertex_texture_coordinate;

out vec3 _fragment_position;
out vec3 vertex_normal;
out vec2 vertex_texture_coordinate;

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;

void main()
{
	vertex_normal = _vertex_normal;
	vertex_texture_coordinate = _vertex_texture_coordinate;
	_fragment_position = vec3(view_matrix * model_matrix * vec4(_vertex_position, 1.0f)); // Transforming vertex position from local to global coordinates

	gl_Position = projection_matrix * view_matrix * model_matrix * vec4(_vertex_position, 1.0);
};