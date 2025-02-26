// r_common.hpp - rendering declarations
#ifndef GRAPHX_RENDERING
#define GRAPHX_RENDERING
#include "sanity.hpp"
#include "graphx_namespace.hpp"
#include "ERROR.graphxmodel"
#include <stb_image.h>
#include <array>
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

extern int shader_debug_value;
extern unsigned int shader_index;

struct GLShader
{
	unsigned int id;

	GLShader(std::string vertex_shader_code, std::string fragment_shader_code);

	template<typename T> void setUniform(const std::string &name, T value) const;

	void buildShader(std::string vertex_shader_code, std::string fragment_shader_code);
};

struct Device
{
	graphx::gSettings settings;

	Device();
	virtual ~Device() = default;

	bool isType(int class_type);
	long getType();
	std::string getTypeName();
	void setName(std::string new_name);
	void setName(char *new_name);
	std::string getName();

	virtual void initialize();
	virtual void loadSettings(graphx::gSettings new_settings = {{"FUCKYOU", {}}});
	virtual void prepForDestruction();
	virtual long getUID();
	virtual void setUID(long manual_uid);

protected:
	int my_type;
	long UID = -1; // A UID of -1 means it's not been set yet
	std::string name = "Untitled Device";
};

struct Environment final : public Device // Will be extended
{
	bool ambient_lighting_enabled;
	glm::vec3 ambient_light_color;
	float ambient_light_strength;

	Environment(bool enable_ambient_lighting = true, glm::vec3 init_ambient_color = glm::vec3(1.0f), float init_ambient_strength = 0.05f);

	glm::vec3 getAmbientLight();
	void loadSettings(graphx::gSettings new_settings = {{"FUCKYOU", {}}}) override;
};

struct Material final : public Device
{
	unsigned int texture_diffuse;
	unsigned int texture_specular;

	unsigned char* embedded_texture_diffuse = MISSING_jpg;
	unsigned char* embedded_texture_specular = FLAT_SPEC_jpg;

	glm::vec3 color = glm::vec3(1.0f);
	int specular_sharpness = 16;
	float specular_strength = 1.0f;
	bool mat_fullbright = false;

	Material();
	Material(bool is_fullbright, glm::vec3 init_color = glm::vec3(1.0f));
	Material(unsigned char *init_diffuse_texture, unsigned char *init_specular_texture = NO_TEXTURE_jpg, int init_specular_sharpness = 16, float init_specular_strength = 0.0f, glm::vec3 init_color = glm::vec3(1.0f));
	Material(glm::vec3 init_color, float init_specular_strength = 0.5f, unsigned int init_specular_sharpness = 32);

	unsigned int bufferTextureFromMemory(unsigned char* texture_buffer);

	void loadSettings(graphx::gSettings new_settings = {{"FUCKYOU", {}}}) override;
};

struct Mesh : public Device
{
	std::string name = "Untitled Mesh";
	Material *material = new Material();
	int vao_index = VAO_HANDMADE;
	std::vector<GLfloat> vertices = ERROR_VERTS;
	std::vector<GLuint> indices = ERROR_INDICES;
	unsigned int VBO = 0;
	unsigned int IBO = 0;
	bool is_buffered = false;

	Mesh(Material *init_material = new Material(), std::vector<float> init_vertices = ERROR_VERTS, std::vector<unsigned int> init_indices = ERROR_INDICES, int init_vao_index = VAO_HANDMADE, std::string init_name = "Untitled Mesh");
	~Mesh() override;

	void loadSettings(graphx::gSettings new_settings = {{"FUCKYOU", {}}}) override;
};

// Differentiating 3D meshes and 2D sprites, even though they're extremely similar (for sanity reasons)
struct Sprite : public Mesh
{
	Sprite(Material *init_material = new Material(), int init_vao_index = VAO_HANDMADE);

	void loadSettings(graphx::gSettings new_settings = {{"FUCKYOU", {}}}) override;
};

extern std::array<GLuint, VAOS_AMOUNT> VAOs; // Only one VAO for now but I expect to need more down the line
extern std::vector<GLShader *> shaders; // Same for shaders
extern bool time_to_render;
extern bool time_to_store_buffers;
extern bool do_interpolation;
extern std::map<int, Device*(*)()> device_map;

template<typename T> Device *createNewDevice() { return new T; }

GLFWwindow *W_CreateWindow(int width, int height, const char *title = "Fucking GraphX", bool make_context_current = true);
void		W_SwapAndClear(GLFWwindow *w_window, glm::vec3 w_clear_color = glm::vec3(0.0f));
void		R_GL_BufferMeshData(Mesh *mesh);
void 		R_StoreBuffers();
void 		R_Render(std::mutex &state_mutex, float interpolation_time, glm::mat4 projection_matrix);
void		R_RenderStage(glm::mat4 projection_matrix, unsigned int shader_index);
void		R_TroupeChanged(); // Bad way of buffering new Meshes when new Actors are added to a Theatre
#endif