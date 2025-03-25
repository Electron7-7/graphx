#version 460 core
uniform (layout = 0) in vec3 _vertex_position;

uniform mat4 model_matrix;

void main()
{
	gl_Position = model_matrix * vec4(_vertex_position, 1.0f);
}