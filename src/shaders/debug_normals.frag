#version 460 core
out vec4 FragColor;

in vec3 fragment_position;
in vec3 vertex_normal;
in vec2 vertex_uv;
in vec3 vertex_color;

void main()
{
	FragColor = vec4((normalize(vertex_normal) + vec3(1.0f)) / vec3(2.0f), 1.0f);
}