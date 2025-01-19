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
#include <filesystem>

#define GLSHADER_TYPE_VERTEX	0
#define GLSHADER_TYPE_FRAGMENT	1
#define GLSHADER_TYPE_PROGRAM	2


#define SHADERS_AMOUNT		1
//---------------------------
#define SHADER_PHONG		0


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


#define MISSING_TEXTURE_DIFF 		std::filesystem::path("src/images/MISSING.jpg")
#define MISSING_TEXTURE_SPEC		std::filesystem::path("src/images/MISSING_SPECULAR.jpg")
#define TOOL_TEXTURE_LIGHT			std::filesystem::path("src/images/LIGHT.jpg")
#define NO_TEXTURE					std::filesystem::path("src/images/NO_TEXTURE.jpg")

// Le secret dev texture
#define DOOM_TEXTURE_DIFF			std::filesystem::path("src/images/COMP04_5.png")
#define DOOM_TEXTURE_SPEC			std::filesystem::path("src/images/COMP04_5_SPECULAR.jpg")

class Actor;		// Forward-declare Actor
class GraphXPlayer;	// Forward-declare GraphXPlayer
struct Theatre;		// Forward-declare Theatre

struct GLShader
{
	unsigned int id;

	GLShader(std::filesystem::path vertex_shader_path, std::filesystem::path fragment_shader_path);
	// GLShader(std::filesystem::path shader_path);
	// GLShader(const char *vertex_shader_code, const char *fragment_shader_code);
	// GLShader(const char *shader_code);

	template<typename T> void setUniform(const std::string &name, T value) const;
};

struct Environment // Will be extended
{
	bool ambient_lighting_enabled;
	glm::vec3 ambient_light_color;
	float ambient_light_strength;

	Environment(bool enable_ambient_lighting, glm::vec3 init_ambient_color = glm::vec3(1.0f), float init_ambient_strength = 0.05f)
	: ambient_lighting_enabled(enable_ambient_lighting), ambient_light_color(init_ambient_color), ambient_light_strength(init_ambient_strength)
	{}

	glm::vec3 getAmbientLight();
};

struct Material
{
	// std::vector<unsigned int> textures;
	// std::vector<std::string> texture_paths;
	unsigned int texture_diffuse;
	unsigned int texture_specular;

	std::filesystem::path texture_path_diffuse = MISSING_TEXTURE_DIFF;
	std::filesystem::path texture_path_specular = MISSING_TEXTURE_SPEC;

	glm::vec3 color;
	int specular_sharpness;
	float specular_strength;
	bool mat_fullbright;

	Material(bool is_fullbright, glm::vec3 init_color)
	: texture_path_diffuse(NO_TEXTURE), color(init_color), mat_fullbright(is_fullbright)
	{}

	Material(std::filesystem::path init_diffuse_texture = MISSING_TEXTURE_DIFF, std::filesystem::path init_specular_texture = MISSING_TEXTURE_SPEC, int init_specular_sharpness = 16, float init_specular_strength = 0.0f)
	: texture_path_diffuse(init_diffuse_texture), texture_path_specular(init_specular_texture), color(glm::vec3(1.0f)), specular_sharpness(init_specular_sharpness), specular_strength(init_specular_strength), mat_fullbright(false)
	{}

	Material(glm::vec3 init_color, float init_specular_strength = 0.5f, unsigned int init_specular_sharpness = 32)
	: color(init_color), specular_sharpness(init_specular_sharpness), specular_strength(init_specular_strength), mat_fullbright(false)
	{}

	unsigned int bufferTexture(std::filesystem::path path);
};

struct Mesh
{
	Actor *owner;
	Material material; // Change to vector of materials later(?)

	const unsigned int vao_index;
	const std::vector<GLfloat> vertices;
	const std::vector<GLuint> indices;
	unsigned int VBO;
	unsigned int IBO;
	bool is_buffered = false;
	
	Mesh(Actor *init_owner = NULL, Material init_material = Material(), const std::vector<GLfloat> init_vertices = ERROR_VERTS, const std::vector<GLuint> init_indices = ERROR_INDICES, const unsigned int init_vao_index = VAO_HANDMADE)
	: owner(init_owner), material(init_material), vao_index(init_vao_index), vertices(init_vertices), indices(init_indices)
	{}
};

struct Sprite : Mesh // Differentiating 3D meshes and 2D sprites, even though they're extremely similar (for sanity reasons)
{
	// All sprites (even missing ones) always use the default quad mesh, hence the unique constructor
	Sprite(Actor *init_owner = NULL, Material init_material = Material(), const unsigned int init_vao_index = VAO_HANDMADE)
	: Mesh(init_owner, init_material, QUAD_VERTS, QUAD_INDICES, init_vao_index)
	{}
};

extern std::array<GLuint, VAOS_AMOUNT> VAOs; // Only one VAO for now but I expect to need more down the line
extern std::vector<GLShader *> shaders; // Same for shaders
extern bool time_to_render;
extern bool time_to_store_buffers;
extern bool do_interpolation;

GLFWwindow *W_CreateWindow(int width, int height, const char *title = "Fucking GraphX", bool make_context_current = true);
void		W_SwapAndClear(GLFWwindow *w_window, glm::vec3 w_clear_color = glm::vec3(0.0f));
void		R_GL_BufferMeshData(Mesh *mesh);
void 		R_StoreBuffers();
void 		R_Render(std::mutex &state_mutex, double interpolation_time, glm::mat4 projection_matrix, Environment *current_environment);
void		R_RenderFlats(glm::mat4 projection_matrix, glm::mat4 model_matrix, Environment *current_environment, unsigned int shader_index);
void		R_TroupeChanged();
#endif