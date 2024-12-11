// r_main.hpp - rendering code specifically for the renderer logic
#ifndef GRAPHX_RENDERING
#define GRAPHX_RENDERING
#include "sanity.hpp"
#include <vector>
#include <array>

#define GLSHADER_TYPE_VERTEX	0
#define GLSHADER_TYPE_FRAGMENT	1
#define GLSHADER_TYPE_PROGRAM	2

#define VAOS_AMOUNT		5
//-----------------------
#define VAO_ENVIRONMENT	0
#define VAO_CHARACTERS	1
#define VAO_PROPS		2
#define VAO_TESTING		3
#define VAO_ERR			4

class GLShader
{
public:
	unsigned int ID;

	GLShader(const char *vertex_path, const char *fragment_path);

	void use();

	void setBool(const std::string &name, bool value) const;
	void setInt(const std::string &name, int value) const;
	void setFloat(const std::string &name, float value) const;
	void setMatrix(const std::string &name, glm::mat4 value) const;

private:
	void shaderErrorHandler(int thing, int type);
};

struct RenderState
{
	unsigned int vao_id;
	// unsigned int vertex_data_offset, index_data_offset, indices_amount;
	glm::vec3 render_position;
	glm::vec3 render_rotation_euler;	// x (pitch), y (yaw), z (roll)
};

struct RenderCmd
{
	unsigned int vao_id;
	unsigned int state_index = 0;

	std::vector<RenderState *> current_state_buffer;
	std::vector<RenderState *> previous_state_buffer;

	RenderCmd(unsigned int init_vao_id, std::vector<RenderState *> init_render_state_buffers)
	: vao_id(init_vao_id), current_state_buffer(init_render_state_buffers), previous_state_buffer(init_render_state_buffers)
	{}
};

/*struct RenderStorageCmd
{
	std::vector<GLfloat> vertices = {0.0f};
	std::vector<GLuint> indices = {0};
	unsigned int vao_id;
	unsigned int indices_amount;
	unsigned int VBO = 0;
	unsigned int EBO = 0;

	RenderStorageCmd() // The default constructor should load the verts and indices of the ERROR mesh... when it exists
	: vao_id(VAO_ERR)
	{}
	RenderStorageCmd(unsigned int new_vao_id, std::vector<GLfloat> new_vertices, std::vector<GLuint> new_indices)
	: vertices(new_vertices), indices(new_indices), vao_id(new_vao_id), indices_amount(new_indices.size())
	{}
};*/

struct Mesh
{
	unsigned int vao_id;
	const char *texture_image;
	std::vector<GLfloat> vertices = {0.0f};
	std::vector<GLuint> indices = {0};
	unsigned int VBO = 0;
	unsigned int EBO = 0;
	unsigned int indices_amount = 0;

	// Note: there should be multiple constructors; one for raw vertex data, one for .obj files, etc.

	Mesh() // Default constructor handles ERR meshes (i.e: missing/no mesh). For now, ERR meshes don't exist and are dropped entirely. There should be a default mesh for missing meshes, though
	: vao_id(VAO_ERR)
	{}

	Mesh(unsigned int init_vao_id, std::vector<GLfloat> new_vertices, std::vector<GLuint> new_indices)
	: vao_id(init_vao_id), vertices(new_vertices), indices(new_indices), indices_amount(new_indices.size())
	{}
};

GLuint T_GenerateTexture(const char *filepath);

extern std::array<GLuint, VAOS_AMOUNT> vertex_array_objects;
extern std::vector<Mesh *> meshes; // RenderStorageCmd now changed to Mesh (functionally the same)
extern std::vector<RenderCmd *> render_commands;

GLFWwindow *W_CreateWindow(u_int16_t width, u_int16_t height, const char *title = "Fucking GraphX", bool make_context_current = true);
void W_SwapAndClear(GLFWwindow *w_window, float clear_color_r = 0.3f, float clear_color_g = 0.4f, float clear_color_b = 0.7f, float clear_color_a = 1.0f);

void R_StoreBuffers();
void R_Render(GLShader current_shader);
#endif