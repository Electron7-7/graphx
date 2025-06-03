#version 460 core
out vec4 FragColor;

in vec3 fragment_position;
in vec3 vertex_normal;
in vec2 vertex_uv;
in vec3 vertex_color;

struct Material
{
	vec3 diffuse_color;
	sampler2D texture_diffuse;
};

uniform Material current_material;

void main()
{
	FragColor = vec4(texture(current_material.texture_diffuse, vertex_uv).rgb * current_material.diffuse_color, 1.0f);
}