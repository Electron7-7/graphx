// r_common.hpp - rendering declarations
#ifndef GRAPHX_RENDERING
#define GRAPHX_RENDERING
#include "sanity.hpp"
#include "ERROR.graphxmodel"
#include <any>
#include <array>
#include <mutex>
#include <vector>
#include <string>
#include <filesystem>
#include <unordered_map>

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


#define MISSING_TEXTURE_DIFF 			MISSING_jpg
#define MISSING_TEXTURE_SPEC			MISSING_SPECULAR_jpg
#define TOOL_TEXTURE_LIGHT				LIGHT_jpg
#define NO_TEXTURE						NO_TEXTURE_jpg
// Le secret dev texture
#define DOOM_TEXTURE_DIFF				COMP04_5_png
#define DOOM_TEXTURE_SPEC				COMP04_5_SPECULAR_jpg


#define VAOS_AMOUNT			1
//---------------------------
#define VAO_HANDMADE		0


#define DEVICE_DEVICE		0
#define DEVICE_ENVIRONMENT	1
#define DEVICE_MATERIAL		2
#define DEVICE_MESH			3
#define DEVICE_SPRITE		3
#define DEVICE_COLLIDER		4

class Actor;		// Forward-declare Actor
class GraphXPlayer;	// Forward-declare GraphXPlayer
struct Theatre;		// Forward-declare Theatre

typedef std::unordered_map<std::string, std::any> gSettings;

struct GLShader
{
	unsigned int id;

	GLShader(std::filesystem::path vertex_shader_path, std::filesystem::path fragment_shader_path);
	// GLShader(std::filesystem::path shader_path);
	GLShader(std::string vertex_shader_code, std::string fragment_shader_code);
	// GLShader(const char *shader_code);

	template<typename T> void setUniform(const std::string &name, T value) const;
	void buildShader(std::string vertex_shader_code, std::string fragment_shader_code);
};

struct Device
{
	int device_type;
	gSettings settings;

	virtual void loadSettings();
	virtual long getUID();
	virtual void setUID(long manual_uid = -1); // If manual_uid is -1, a new UID is generated instead

protected:
	long UID = -1; // A UID of -1 means it's not been set yet
};

struct Environment : public Device // Will be extended
{
	bool ambient_lighting_enabled;
	glm::vec3 ambient_light_color;
	float ambient_light_strength;

	Environment(bool enable_ambient_lighting = true, glm::vec3 init_ambient_color = glm::vec3(1.0f), float init_ambient_strength = 0.05f);

	glm::vec3 getAmbientLight();

	void loadSettings() override;
};

struct Material : public Device
{
	unsigned int texture_diffuse;
	unsigned int texture_specular;

	// std::filesystem::path texture_path_diffuse = MISSING_TEXTURE_DIFF;
	// std::filesystem::path texture_path_specular = MISSING_TEXTURE_SPEC;
	unsigned char* embedded_texture_diffuse = NO_TEXTURE;
	unsigned char* embedded_texture_specular = NO_TEXTURE;

	glm::vec3 color;
	int specular_sharpness;
	float specular_strength;
	bool mat_fullbright;

	Material(bool is_fullbright, glm::vec3 init_color);
	Material(unsigned char *init_diffuse_texture = MISSING_TEXTURE_DIFF, unsigned char *init_specular_texture = MISSING_TEXTURE_SPEC, int init_specular_sharpness = 16, float init_specular_strength = 0.0f, glm::vec3 init_color = glm::vec3(1.0f));
	Material(glm::vec3 init_color, float init_specular_strength = 0.5f, unsigned int init_specular_sharpness = 32);

	// unsigned int bufferTexture(std::filesystem::path path);
	unsigned int bufferTextureFromMemory(unsigned char* texture_buffer);

	void loadSettings() override;
};

struct Mesh : public Device
{
	std::string name = "Untitled Mesh";
	Material material = Material();
	int vao_index = VAO_HANDMADE;
	std::vector<GLfloat> vertices = ERROR_VERTS;
	std::vector<GLuint> indices = ERROR_INDICES;
	unsigned int VBO = 0;
	unsigned int IBO = 0;
	bool is_buffered = false;

	Mesh(Material init_material = Material(), std::vector<GLfloat> init_vertices = ERROR_VERTS, std::vector<GLuint> init_indices = ERROR_INDICES, int init_vao_index = VAO_HANDMADE, std::string init_name = "Untitled Mesh");

	void loadSettings() override;
};

struct Sprite : public Mesh // Differentiating 3D meshes and 2D sprites, even though they're extremely similar (for sanity reasons)
{
	// All sprites (even missing ones) always use the default quad mesh, hence the unique constructor
	Sprite(Material init_material = Material(), int init_vao_index = VAO_HANDMADE);

	void loadSettings() override;
};

extern std::array<GLuint, VAOS_AMOUNT> VAOs; // Only one VAO for now but I expect to need more down the line
extern std::vector<GLShader *> shaders; // Same for shaders
extern bool time_to_render;
extern bool time_to_store_buffers;
extern bool do_interpolation;

GLFWwindow  *W_CreateWindow(int width, int height, const char *title = "Fucking GraphX", bool make_context_current = true);
void		 W_SwapAndClear(GLFWwindow *w_window, glm::vec3 w_clear_color = glm::vec3(0.0f));
void		 R_GL_BufferMeshData(Mesh *mesh);
void 		 R_StoreBuffers();
void 		 R_Render(std::mutex &state_mutex, float interpolation_time, glm::mat4 projection_matrix);
void		 R_RenderFlats(glm::mat4 projection_matrix, glm::mat4 model_matrix, unsigned int shader_index);
void		 R_TroupeChanged();
Environment *getCurrentEnvironment();
#endif