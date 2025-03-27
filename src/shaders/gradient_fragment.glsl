#version 330 core
uniform vec4 top_color;
uniform vec4 bottom_color;
in vec2 v_uv;
out vec4 FragColor;

void main()
{
	FragColor = bottom_color * (1 - uv.y) + top_color * uv.y;
}