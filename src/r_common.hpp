// r_common.hpp - rendering declarations
#ifndef GRAPHX_RENDERING
#define GRAPHX_RENDERING
#include "sanity.hpp"
#include "quad.graphxmodel"
#include "ERROR.graphxmodel"
#include <vector>
#include <array>
#include <string>
#include <mutex>

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

#define VAOS_AMOUNT			1
//---------------------------
#define VAO_HANDMADE		0

#define MISSING_TEXTURE_PATH 		SRC_DIR(std::string("src/images/MISSING.jpg"))
#define DOOM_TEXTURE_PATH			SRC_DIR(std::string("src/images/COMP04_5.png"))

class Actor;		// Forward-declare Actor
class GraphXPlayer;	// Forward-declare GraphXPlayer
struct Theatre;		// Forward-declare Theatre

class GLShader
{
public:
	unsigned int ID;

	GLShader(const char *vertex_path, const char *fragment_path);

	void use();

	void setBool(const std::string &name, bool value) const;
	void setInt(const std::string &name, int value) const;
	void setFloat(const std::string &name, float value) const;
	void setVec3(const std::string &name, glm::vec3	value) const;
	void setMat3(const std::string &name, glm::mat3 value) const;
	void setMat4(const std::string &name, glm::mat4 value) const;

private:
	void shaderErrorHandler(int thing, int type);
};

struct Environment // Will be extended
{
	bool do_ambient_lighting = true;
	
	glm::vec3 ambient_light_color;
	float ambient_light_strength;

	Environment(bool does_ambient_lighting, glm::vec3 init_ambient_color = glm::vec3(0.5f), float init_ambient_strength = 0.2f)
	: do_ambient_lighting(does_ambient_lighting), ambient_light_color(init_ambient_color), ambient_light_strength(init_ambient_strength)
	{}

	glm::vec3 getAmbientLight();
};

struct Material // Will be extended
{
	bool do_specular;
	bool do_vertex_colors; // Would replace albedo if toggled on(?)

	glm::vec3 albedo;
	float specular_strength;
	int specular_sharpness;

	Material(bool does_specular = true, bool does_vertex_colors = false, glm::vec3 init_albedo = glm::vec3(1.0f), float init_specular_strength = 0.5f, int init_specular_sharpness = 32)
	: do_specular(does_specular), do_vertex_colors(does_vertex_colors), albedo(init_albedo), specular_strength(init_specular_strength), specular_sharpness(init_specular_sharpness)
	{}
};

struct Mesh
{
	Actor *owner;
	Material material;

	const unsigned int vao_index;
	const std::vector<GLfloat> vertices;
	const std::vector<GLuint> indices;
	unsigned int VBO;
	unsigned int IBO;
	unsigned int m_texture;
	
	std::string texture_path = MISSING_TEXTURE_PATH;
	bool buffer_data = true;

	Mesh(Actor *init_owner = NULL, const unsigned int init_vao_index = VAO_HANDMADE, const std::vector<GLfloat> init_vertices = ERROR_VERTS, const std::vector<GLuint> init_indices = ERROR_INDICES, std::string init_texture_path = MISSING_TEXTURE_PATH)
	: owner(init_owner), vao_index(init_vao_index), vertices(init_vertices), indices(init_indices), texture_path(init_texture_path)
	{}

	void generateTexture();
	bool bufferData();
};

struct Sprite : Mesh // Differentiating 3D meshes and 2D sprites, even though they're extremely similar (for sanity reasons)
{
	// All sprites (even missing ones) always use the default quad mesh, hence the unique constructor
	Sprite(Actor *init_owner = NULL, const unsigned int init_vao_index = VAO_HANDMADE, std::string init_texture = MISSING_TEXTURE_PATH)
	: Mesh(init_owner, init_vao_index, QUAD_VERTS, QUAD_INDICES, init_texture)
	{}
};

extern std::array<GLuint, VAOS_AMOUNT> VAOs; // Only one VAO for now but I expect to need more down the line
extern std::vector<GLShader *> shaders;
extern bool time_to_render;
extern bool time_to_store_buffers;
extern bool do_interpolation;

GLFWwindow *W_CreateWindow(int width, int height, const char *title = "Fucking GraphX", bool make_context_current = true);
void		W_SwapAndClear(GLFWwindow *w_window, glm::vec3 w_clear_color = glm::vec3(0.0f));
void 		R_StoreBuffers();
void 		R_Render(std::mutex &state_mutex, double interpolation_time, glm::mat4 projection_matrix, GraphXPlayer *current_player, Environment *current_environment);
#endif