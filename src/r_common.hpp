// r_main.hpp - rendering code specifically for the renderer logic
#include "sanity.hpp"
#include <vector>
#include <array>

#define GLSHADER_TYPE_VERTEX	0
#define GLSHADER_TYPE_FRAGMENT	1
#define GLSHADER_TYPE_PROGRAM	2

#define VAOS_AMOUNT		4
//-----------------------
#define VAO_ENVIRONMENT	0
#define VAO_CHARACTERS	1
#define VAO_PROPS		2
#define VAO_TESTING		3

#ifndef GRAPHX_RENDERING
#define GRAPHX_RENDERING

class GLShader
{
public:
	unsigned int ID;

	GLShader(const char* vertex_path, const char* fragment_path);

	void use();

	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void setMatrix(const std::string& name, glm::mat4 value) const;

private:
	void shaderErrorHandler(int thing, int type);
};

struct RenderState
{
	unsigned int vao_id, vertex_data_offset, index_data_offset, indices_amount;
	glm::vec3 render_position;
	glm::vec3 render_rotation_euler;	// x (pitch), y (yaw), z (roll)
};

struct RenderCmd
{
	RenderState previous_state[2];
	RenderState current_state[2];
	int state_index = 0;
};

struct RenderStorageCmd
{
	std::vector<GLfloat> vertices;
	std::vector<GLuint> indices;
	unsigned int vao_id, indices_amount, VBO, EBO;

	RenderStorageCmd(unsigned int new_vao_id, std::vector<GLfloat> new_vertices, std::vector<GLuint> new_indices) : vertices(new_vertices), indices(new_indices), vao_id(new_vao_id), indices_amount(new_indices.size())
	{}
};

/*
	VERTEX ARRAY OBJECTS
	Limit the amount used, and have each render command provide an ID to the VAO it uses AND ONLY SWITCH THEM IF NEEDED.
	(perhaps even sort render commands by VAO IDs??)
	(OH DEFINITELY SORT RENDER STORAGE COMMANDS BY VAO IDS!!)
*/
extern std::array<GLuint, VAOS_AMOUNT> vertex_array_objects;
extern std::vector<RenderStorageCmd> render_storage_commands;
extern std::vector<RenderCmd> render_commands;

GLFWwindow* W_CreateWindow(u_int16_t width, u_int16_t height, const char* title = "Fucking GraphX", bool make_context_current = true);
void W_SwapAndClear(GLFWwindow* w_window, float clear_color_r = 0.3f, float clear_color_g = 0.4f, float clear_color_b = 0.7f, float clear_color_a = 1.0f);

void R_StoreBuffers();
void R_Render(GLShader current_shader);
#endif

#ifndef GRAPHX_RENDERING_COMMON
#define GRAPHX_RENDERING_COMMON

struct Mesh
{
	const char* texture_image;

	// Note: there should be multiple constructors; one for raw vertex data, one for .obj files, etc.
	Mesh(unsigned int vao_id, std::vector<float> new_vertices, std::vector<unsigned int> new_indices)
	{ render_storage_commands.push_back( RenderStorageCmd(vao_id, new_vertices, new_indices) ); }
};

GLuint T_GenerateTexture(const char* filepath);
#endif