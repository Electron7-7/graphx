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


#define MISSING_TEXTURE_DIFF 		SRC_DIR(std::string("src/images/MISSING.jpg"))
#define MISSING_TEXTURE_SPEC		SRC_DIR(std::string("src/images/MISSING_SPECULAR.jpg"))
#define TOOL_TEXTURE_LIGHT			SRC_DIR(std::string("src/images/LIGHT.jpg"))

// Le secret dev texture
#define DOOM_TEXTURE_DIFF			SRC_DIR(std::string("src/images/COMP04_5.png"))
#define DOOM_TEXTURE_SPEC			SRC_DIR(std::string("src/images/COMP04_5_SPECULAR.jpg"))

class Actor;		// Forward-declare Actor
class GraphXPlayer;	// Forward-declare GraphXPlayer
struct Theatre;		// Forward-declare Theatre

struct GLShader
{
	unsigned int id;

	GLShader(std::string vertex_shader_path, std::string fragment_shader_path);
	// GLShader(std::string shader_path);
	// GLShader(const char *vertex_shader_code, const char *fragment_shader_code);
	// GLShader(const char *shader_code);

	template<typename T> void setUniform(const std::string &name, T value) const;
};

struct Environment // Will be extended
{
	bool ambient_lighting_enabled;
	glm::vec3 ambient_light_color;
	float ambient_light_strength;

	Environment(bool enable_ambient_lighting, glm::vec3 init_ambient_color = glm::vec3(1.0f), float init_ambient_strength = 0.1f)
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

	std::string texture_path_diffuse = MISSING_TEXTURE_DIFF;
	std::string texture_path_specular = MISSING_TEXTURE_SPEC;

	glm::vec3 color;
	int specular_sharpness;
	float specular_strength;

	Material(std::string init_diffuse_texture = MISSING_TEXTURE_DIFF, std::string init_specular_texture = MISSING_TEXTURE_SPEC, int init_specular_sharpness = 32, float init_specular_stregth = 1.0f)
	: texture_path_diffuse(init_diffuse_texture), texture_path_specular(init_specular_texture), specular_sharpness(init_specular_sharpness), specular_strength(init_specular_stregth)
	{}

	Material(glm::vec3 init_color, float init_specular_strength = 0.5f, unsigned int init_specular_sharpness = 32)
	: color(init_color), specular_sharpness(init_specular_sharpness), specular_strength(init_specular_strength)
	{}

	unsigned int bufferTexture(std::string path);
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
	
	Mesh(Actor *init_owner = NULL, Material init_material = Material(), const unsigned int init_vao_index = VAO_HANDMADE, const std::vector<GLfloat> init_vertices = ERROR_VERTS, const std::vector<GLuint> init_indices = ERROR_INDICES)
	: owner(init_owner), material(init_material), vao_index(init_vao_index), vertices(init_vertices), indices(init_indices)
	{}
};

struct Sprite : Mesh // Differentiating 3D meshes and 2D sprites, even though they're extremely similar (for sanity reasons)
{
	// All sprites (even missing ones) always use the default quad mesh, hence the unique constructor
	Sprite(Actor *init_owner = NULL, Material init_material = Material(), const unsigned int init_vao_index = VAO_HANDMADE)
	: Mesh(init_owner, init_material, init_vao_index, QUAD_VERTS, QUAD_INDICES)
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
#endif