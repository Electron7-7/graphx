#version 460 core
out vec4 FragColor;

in vec3 vertexColors;

// in vec2 textureCoordinates;

// uniform sampler2D newTexture2D;

void main()
{
	FragColor = vec4(vertexColors, 1.0);
};