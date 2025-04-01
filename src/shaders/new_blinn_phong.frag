#version 460 core
#define MAX_NUMBER_OF_LIGHTS 100

out vec4 FragColor;

in vec3 fragment_position;
in vec3 vertex_normal;
in vec2 vertex_uv;
in vec3 vertex_color;

struct Environment
{

};

struct Material
{
	
};

struct Light
{
	vec3 color;
	float specular_strength;

};

void main()
{
	FragColor = vec4(1.0f);
}