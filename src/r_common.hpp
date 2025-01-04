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

#define BUFFERS_AMOUNT		5
//---------------------------
#define BUFFER_ERR			0
#define BUFFER_TESTING		1
#define BUFFER_FLATS		2
#define BUFFER_ACTORS		3
#define BUFFER_PROPS		4

#define VAOS_AMOUNT			2
#define VAO_DEFAULT			0
//---------------------------
#define VAO_COLOR			0
#define VAO_TEXTURE			1

#define MISSING_TEXTURE_PATH 		SRC_DIR(std::string("src/images/MISSING.jpg"))
#define DOOM_TEXTURE_PATH			SRC_DIR(std::string("src/images/COMP04_5.png"))

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

struct Mesh
{
	Actor *owner;

	const unsigned int vao_index;
	const std::vector<GLfloat> vertices;
	const std::vector<GLuint> indices;

	unsigned int VBO;
	unsigned int IBO;
	std::string texture_path = MISSING_TEXTURE_PATH;
	unsigned int m_texture;

	Mesh(Actor *init_owner = NULL, const unsigned int init_vao_index = VAO_DEFAULT, const std::vector<GLfloat> init_vertices = ERROR_VERTS, const std::vector<GLuint> init_indices = ERROR_INDICES, std::string init_texture_path = MISSING_TEXTURE_PATH)
	: vao_index(init_vao_index), vertices(init_vertices), indices(init_indices), texture_path(init_texture_path)
	{owner = init_owner;}

	void generateTexture();
};

struct Sprite : Mesh // Differentiating 3D meshes and 2D sprites, even though they're extremely similar (for sanity reasons)
{
	// All sprites (even missing ones) always use the default quad mesh, hence the unique constructor
	Sprite(Actor *init_owner = NULL, const unsigned int init_vao_index = VAO_DEFAULT, std::string init_texture = MISSING_TEXTURE_PATH)
	: Mesh(init_owner, init_vao_index, QUAD_VERTS, QUAD_INDICES, init_texture)
	{}
};

extern std::array<GLuint, VAOS_AMOUNT> VAOs; // Only one VAO for now but I expect to need more down the line
extern std::vector<GLShader *> shaders;
extern std::atomic_bool time_to_render;
extern std::atomic_bool time_to_store_buffers;
extern std::atomic_bool do_interpolation;

GLFWwindow *W_CreateWindow(int width, int height, const char *title = "Fucking GraphX", bool make_context_current = true);
void		W_SwapAndClear(GLFWwindow *w_window, float clear_color_r = 0.3f, float clear_color_g = 0.4f, float clear_color_b = 0.7f, float clear_color_a = 1.0f);
void 		R_StoreBuffers();
void 		R_Render(std::mutex &state_mutex, double interpolation_time, glm::mat4 projection, glm::mat4 camera_view);
void		T_LoadTheatre(Theatre *new_theatre);
#endif