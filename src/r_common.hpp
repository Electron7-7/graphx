// r_common.hpp - rendering declarations
#ifndef GRAPHX_RENDERING
#define GRAPHX_RENDERING
#include "sanity.hpp"
#include <mutex>
#include <vector>
#include <array>
#include <atomic>
#include <iostream>

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

#define DEFAULT_TEXTURE_PATH "src/images/COMP04_5.png"

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
	std::vector<GLfloat> vertices = {0.0f};
	std::vector<GLuint> indices = {0};
	std::string texture_path;
	unsigned int m_texture = 0;
	unsigned int VBO = 0;
	unsigned int EBO = 0;
	unsigned int indices_amount = 0;

	// Note: there should be multiple constructors; one for raw vertex data, one for .obj files, etc.
	Mesh(unsigned int init_vao_id = VAO_ERR, std::string init_texture = DEFAULT_TEXTURE_PATH) // Default constructor handles ERR meshes (i.e: missing/no mesh). For now, ERR meshes don't exist and are dropped entirely. There should be a default mesh for missing meshes, though
	: vao_id(init_vao_id), texture_path(init_texture)
	{}

	Mesh(unsigned int init_vao_id, std::vector<GLfloat> new_vertices, std::vector<GLuint> new_indices, std::string init_texture = DEFAULT_TEXTURE_PATH)
	: vao_id(init_vao_id), vertices(new_vertices), indices(new_indices), texture_path(init_texture), indices_amount(new_indices.size())
	{}

	void generateTexture();
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
extern std::atomic_bool time_to_render;
extern std::atomic_bool time_to_store_buffers;

GLFWwindow *W_CreateWindow(int width, int height, const char *title = "Fucking GraphX", bool make_context_current = true);
void		W_SwapAndClear(GLFWwindow *w_window, float clear_color_r = 0.3f, float clear_color_g = 0.4f, float clear_color_b = 0.7f, float clear_color_a = 1.0f);
void 		R_StoreBuffers(bool changing_to_new_theatre = true);
void 		R_Render(std::mutex &state_mutex, GLShader &current_shader, double interpolation_time, glm::mat4 projection, glm::mat4 camera_view);
#endif