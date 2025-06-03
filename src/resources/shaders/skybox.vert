#version 460 core
layout (location = 0) in vec3 _skybox_vertex_position;

out vec3 skybox_uv;

uniform mat4 skybox_projection_matrix;
uniform mat4 skybox_view_matrix;

void main()
{
	skybox_uv = _skybox_vertex_position;
	vec4 position = skybox_projection_matrix * skybox_view_matrix * vec4(_skybox_vertex_position, 1.0f);
	gl_Position = position.xyww;
}