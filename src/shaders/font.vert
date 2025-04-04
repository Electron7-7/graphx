#version 460 core
layout (location = 0) in vec4 vertex_position_and_uv;
out vec2 vertex_uv;

uniform mat4 projection_matrix;

void main()
{
	gl_Position = projection_matrix * vec4(vertex_position_and_uv.xy, 0.0f, 1.0f);
	vertex_uv = vertex_position_and_uv.zw;
}