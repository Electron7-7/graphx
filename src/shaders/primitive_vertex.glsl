#version 460 core
layout (location = 0) in vec3 _vertex_position;

// uniform vec3 vertex_position[3];
// out int vertex_id;
// uniform mat4 model_matrix;
// uniform mat4 view_matrix;
// uniform mat4 projection_matrix;

void main()
{
	// gl_Position = vec4(vertex_position[glVertexID], 1.0f);
	gl_Position = vec4(_vertex_position, 1.0f);
	// vertex_id = glVertexID;
}