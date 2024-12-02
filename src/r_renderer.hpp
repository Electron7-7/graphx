// r_renderer.hpp - Renderer-specific Declarations
#include "sanity.hpp"
#include "r_common.hpp"
#include <vector>

#ifndef GRAPHX_RENDERER
#define GRAPHX_RENDERER
// --------------------------------------------
// Global Programming (i.e: Templates and shit)
// --------------------------------------------



// ---------------------------
// Object-Oriented Programming
// ---------------------------

//
// Vertex Struct
//
struct Vertex
{
	glm::vec3 v_position;
	glm::vec2 v_texture_coordinate;

	Vertex(const glm::vec3& position, const glm::vec2& texture_coordinate);

	inline glm::vec3* getPosition() { return &v_position; }
	inline glm::vec2* getTextureCoordinate() { return &v_texture_coordinate; }
};

//
// MeshCmd struct
//
struct MeshCmd
{
	const char* texture;
	
};

//
// RenderCmd struct
//
struct RenderCmd
{

};

//
// GraphXRenderer class
//
class GraphXRenderer
{
public:
	void Initialize(RenderInitializeCmd initialize_command);
};


// ----------------------
// Functional Programming
// ----------------------
bool instantiateRenderable();

#endif