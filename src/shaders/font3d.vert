// 3D Font Vertex Shader
#version 460 core
layout (location = 0) in vec2 _vertex_position;
layout (location = 1) in vec2 _vertex_uv;

out vec2 vertex_uv;

uniform float text_scale;

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;
uniform mat4 ortho_matrix;

void main()
{
	// Todo: maybe change `/ text_scale` to `* text_scale`
	gl_Position = projection_matrix * view_matrix * model_matrix * ortho_matrix * vec4(_vertex_position * text_scale, 0.0f, 1.0f);
	vertex_uv = _vertex_uv.xy;
}