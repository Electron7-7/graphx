#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
// layout (location = 1) in vec3 aColor; // 2D plane
// layout (location = 2) in vec2 aTexCoord; // 2D plane

// out vec3 vertexColor;
out vec2 textureCoordinates;

uniform mat4 transform;
uniform mat4 model;
uniform mat4 backwards_view;
uniform mat4 projection;

void main()
{
	// gl_Position = transform * vec4(aPos, 1.0);
	gl_Position = projection * backwards_view * model * vec4(aPos, 1.0);
	// vertexColor = aColor;
	textureCoordinates = aTexCoord;
};