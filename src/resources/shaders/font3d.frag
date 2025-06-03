// 3D Font Fragment Shader
#version 460 core
in vec2 vertex_uv;

out vec4 FragColor;

uniform bool is_debug;
uniform sampler2D glyph_texture;
uniform vec3 text_color;

void main()
{
	if(is_debug)
	{
		FragColor = vec4(vec3(0.0f), 1.0f);
		return;
	}
	float glyph_shape = texture(glyph_texture, vertex_uv).r;
	if(glyph_shape < 0.5)
		discard;
	FragColor = vec4(text_color, 1.0f);
}