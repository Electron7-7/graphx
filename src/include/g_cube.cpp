#include <g_cube.hpp>

/*
NOTES
-----
you could make a square with two vertices if you mirror them
you could also make a square with one vertex if you know the size
could you make a square with no vertices?
--- 
OpenGl needs vertices to make a triangle and for now, we can't just generate a cube from nothing using variables and offsets.
For now, the minimum number of vertices OpenGL needs to make a cube (using verts and indices) is EIGHT.
*/

void Cube::makeCube()
{
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Idea: set GL_FALSE to GL_TRUE and use ints instead of floats for more efficient storage?
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
}

void Cube::drawCube()
{
	glDrawElements(GL_TRIANGLES, sizeof(indices), GL_UNSIGNED_INT, 0);
}