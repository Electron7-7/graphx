// r_common.hpp - rendering declarations
#ifndef GRAPHX_RENDERING
#define GRAPHX_RENDERING
#include "sanity.hpp"
#include <mutex>
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

struct Mesh
{
	unsigned int vao_id = VAO_ERR;
	// const char *texture_image;
	std::vector<GLfloat> vertices = {0.0f};
	std::vector<GLuint> indices = {0};
	unsigned int VBO = 0;
	unsigned int EBO = 0;
	unsigned int indices_amount = 0;

	// Note: there should be multiple constructors; one for raw vertex data, one for .obj files, etc.

	Mesh(unsigned int init_vao_id = VAO_ERR) // Default constructor handles ERR meshes (i.e: missing/no mesh). For now, ERR meshes don't exist and are dropped entirely. There should be a default mesh for missing meshes, though
	: vao_id(init_vao_id)
	{}

	Mesh(unsigned int init_vao_id, std::vector<GLfloat> new_vertices, std::vector<GLuint> new_indices)
	: vao_id(init_vao_id), vertices(new_vertices), indices(new_indices), indices_amount(new_indices.size())
	{}
};

struct RenderState
{
	glm::vec3 render_position = {0.0f, 0.0f, 0.0f};
	glm::vec3 render_rotation_euler = {0.0f, 0.0f, 0.0f};	// x (pitch), y (yaw), z (roll)

	RenderState(glm::vec3 init_position = {0.0f, 0.0f, 0.0f}, glm::vec3 init_euler_rotation = {0.0f, 0.0f, 0.0f})
	: render_position(init_position), render_rotation_euler(init_euler_rotation)
	{}
};

extern std::array<GLuint, VAOS_AMOUNT> vertex_array_objects;
extern std::vector<Mesh *> meshes;
extern std::vector<GLuint> shaders;

GLFWwindow *W_CreateWindow(int width, int height, const char *title = "Fucking GraphX", bool make_context_current = true);
void		W_SwapAndClear(GLFWwindow *w_window, float clear_color_r = 0.3f, float clear_color_g = 0.4f, float clear_color_b = 0.7f, float clear_color_a = 1.0f);
GLuint 		T_GenerateTexture(const char *filepath);
void 		R_StoreBuffers();
void 		R_Render(GLShader &current_shader, double interpolation_time, glm::mat4 projection, glm::mat4 camera_view);
#endif