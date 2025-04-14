#version 460 core
layout (location = 0) in vec4 _vertex_position_and_uv;

out vec2 vertex_uv;

uniform vec2 glyph_scale;
uniform vec2 glyph_position;

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;

void main()
{
	// https://stackoverflow.com/a/62629272
	gl_Position = vec4(_vertex_position_and_uv.xy, 0.0f, 1.0f) * vec4(glyph_scale, 1.0f, 1.0f) * 2 + vec4(glyph_position, 0.0f, 0.0f);
	// gl_Position = projection_matrix * view_matrix * model_matrix * vec4(_vertex_position_and_uv.xy, 0.0f, 1.0f);
	vertex_uv = _vertex_position_and_uv.zw;
}