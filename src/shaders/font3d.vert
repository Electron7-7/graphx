// 3D Font Vertex Shader
#version 460 core
layout (location = 0) in vec2 _vertex_position;
layout (location = 1) in vec2 _vertex_uv;

out vec2 vertex_uv;

uniform vec2 glyph_scale;

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;

void main()
{
	// https://stackoverflow.com/a/62629272
	gl_Position = projection_matrix * view_matrix * model_matrix * vec4(vec2(_vertex_position / glyph_scale / 2) - vec2(1.0f), 0.0f, 1.0f);
	vertex_uv = _vertex_uv.xy;
}