#version 460 core
out vec4 FragColor;

in vec3 vertexColor;
in vec2 textureCoordinates;

uniform sampler2D newTexture2D;

void main()
{
	FragColor = vec4(vertexColor, 1.0);
	FragColor = texture(newTexture2D, textureCoordinates) * vec4(vertexColor, 1.0);
};