// 2D Font Vertex Shader
#version 460 core
layout (location = 0) in vec2 _vertex_position;
layout (location = 1) in vec2 _vertex_uv;

out vec2 vertex_uv;

uniform mat4 projection_matrix;

void main()
{
	gl_Position = projection_matrix * vec4(_vertex_position, 0.0f, 1.0f);
	vertex_uv = _vertex_uv;
}