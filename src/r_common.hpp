// r_common.hpp - rendering declarations
#ifndef GRAPHX_RENDERING
#define GRAPHX_RENDERING
#include "sanity.hpp"
#include "quad.graphxmodel"
#include "ERROR.graphxmodel"
#include <mutex>
#include <vector>
#include <array>
#include <atomic>

#define GLSHADER_TYPE_VERTEX	0
#define GLSHADER_TYPE_FRAGMENT	1
#define GLSHADER_TYPE_PROGRAM	2

#define VAOS_AMOUNT		5
//-----------------------
#define VAO_ERR			0
#define VAO_TESTING		1
#define VAO_FLATS		2
#define VAO_ACTORS		3
#define VAO_PROPS		4

#define MISSING_TEXTURE_PATH 		"src/images/MISSING.png"	// Todo: make/use a default MISSING texture

class Actor;	// Forward-declare Actor
struct Theatre;	// Forward-declare Theatre

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
	glm::vec3 render_position = {0.0f, 0.0f, 0.0f};
	glm::vec3 render_rotation_euler = {0.0f, 0.0f, 0.0f};	// x (pitch), y (yaw), z (roll)

	RenderState(glm::vec3 init_position = {0.0f, 0.0f, 0.0f}, glm::vec3 init_euler_rotation = {0.0f, 0.0f, 0.0f})
	: render_position(init_position), render_rotation_euler(init_euler_rotation)
	{}
};

struct Mesh
{
	Actor *owner;

	const int vao_id;
	const std::vector<GLfloat> vertices;
	const std::vector<GLuint> indices;
	const unsigned int indices_amount;

	std::string texture_path;
	unsigned int m_texture = 0;

	unsigned int VBO = 0;
	unsigned int EBO = 0;

	Mesh(Actor *init_owner = NULL, const int init_vao_id = VAO_ERR, const std::vector<GLfloat> init_vertices = ERROR_VERTS, const std::vector<GLuint> init_indices = ERROR_INDICES, std::string init_texture_path = MISSING_TEXTURE_PATH)
	: vao_id(init_vao_id), vertices(init_vertices), indices(init_indices), indices_amount(init_indices.size()), texture_path(init_texture_path)
	{owner = init_owner;}

	void generateTexture();
};

struct Sprite : Mesh // Differentiating 3D meshes and 2D sprites, even though they're extremely similar (for sanity reasons)
{
	// All sprites (even missing ones) always use the default quad mesh, hence the unique constructor
	Sprite(Actor *init_owner = NULL, const int init_vao_id = VAO_ERR, std::string init_texture = MISSING_TEXTURE_PATH)
	: Mesh(init_owner, init_vao_id, QUAD_VERTS, QUAD_INDICES, init_texture)
	{}
};

extern std::array<GLuint, VAOS_AMOUNT> vertex_array_objects;
extern std::vector<Mesh> meshes;
extern std::vector<Sprite> sprites;
extern std::atomic_bool time_to_render;
extern std::atomic_bool time_to_store_buffers;

GLFWwindow *W_CreateWindow(int width, int height, const char *title = "Fucking GraphX", bool make_context_current = true);
void		W_SwapAndClear(GLFWwindow *w_window, float clear_color_r = 0.3f, float clear_color_g = 0.4f, float clear_color_b = 0.7f, float clear_color_a = 1.0f);
void 		R_StoreBuffers(bool changing_to_new_theatre = true);
void 		R_Render(std::mutex &state_mutex, GLShader &current_shader, double interpolation_time, glm::mat4 projection, glm::mat4 camera_view);
void		T_LoadTheatre(Theatre current_theatre);
#endif