// r_common.hpp - rendering declarations
#ifndef GRAPHX_RENDERING
#define GRAPHX_RENDERING
#include "graphx_namespace.hpp"
#include "sanity.hpp"
#include "graphx_namespace.hpp"
#include "t_settings.hpp"
#include <models.hpp>
#include <stb_image.h>
#include <array>
#include <mutex>

#define GLSHADER_TYPE_VERTEX	0
#define GLSHADER_TYPE_FRAGMENT	1
#define GLSHADER_TYPE_PROGRAM	2

#define PREEMPTIVE_OBJ_SCALE 10.0f // In case the vertices of an imported OBJ file aren't between -1 and 1, dividing every vertex in the file by 10 (and multiplying the scale by 10) should fix most cases (I think)


#define SHADERS_AMOUNT		1
//---------------------------
#define SHADER_BLINN_PHONG	0
#define SHADER_PHONG		1


// BUFFERS ARENT IMPLEMENTED YET, SO THESE ARENT USED!
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


#define VAOS_AMOUNT			3
//---------------------------
#define VAO_HANDMADE		0
#define VAO_OBJ				1


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
	graphx::gSettings settings = empty_settings;

	Device();
	virtual ~Device() = default;

	bool isType(int class_type);
	long getType();
	std::string getTypeName();
	void setName(std::string new_name);
	void setName(char *new_name);
	std::string getName();

	virtual void initialize();
	virtual void loadSettings(graphx::gSettings new_settings = empty_settings);
	virtual void prepForDestruction();
	virtual long getUID();
	virtual void setUID(long manual_uid);

	std::string name = "Untitled Device";

protected:
	int my_type;
	long UID = -1; // A UID of -1 means it's not been set yet
	bool ready_to_destroy = false;
};

struct Environment final : public Device // Will be extended
{
	bool ambient_lighting_enabled = true;
	glm::vec3 ambient_light_color = glm::vec3(1.0f);
	float ambient_light_strength = 0.05f;

	Environment(bool enable_ambient_lighting = true, glm::vec3 init_ambient_color = glm::vec3(1.0f), float init_ambient_strength = 0.05f);

	glm::vec3 getAmbientLight();
	void loadSettings(graphx::gSettings new_settings = empty_settings) override;
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

	void loadSettings(graphx::gSettings new_settings = empty_settings) override;
};

struct Mesh : public Device
{
	std::string name = "Untitled Mesh";
	Material *material = new Material();
	int vao_index = VAO_HANDMADE;
	std::vector<float> vertices = CUBE_VERTS;
	std::vector<unsigned int> indices = CUBE_INDICES;
	unsigned int VBO = 0;
	unsigned int IBO = 0;
	bool is_buffered = false;
	glm::vec3 mesh_scale = glm::vec3(1.0f);

	Mesh();
	Mesh(Material *new_material);

	void loadSettings(graphx::gSettings new_settings = empty_settings) override;
	void prepForDestruction() override;
};

// Differentiating 3D meshes and 2D sprites, even though they're extremely similar (for sanity reasons)
struct Sprite : public Mesh
{
	Sprite();

	void loadSettings(graphx::gSettings new_settings = empty_settings) override;
};


#define GRAPHX_OPENGL 917 // When I support other APIs, more of these will be added
// Found in r_renderer.cpp
extern std::array<GLuint, VAOS_AMOUNT> VAOs; // Only one VAO for now but I expect to need more down the line
extern std::vector<GLShader *> shaders; // Same for shaders
extern int graphx_api;
extern bool time_to_render;
extern bool time_to_store_buffers;
extern bool do_interpolation;
extern int shader_debug_value;
extern unsigned int shader_index;
extern glm::vec2 main_window_size;
extern float camera_near;
extern float camera_far;
extern int current_vao_index;
// Found in r_common.cpp
extern std::map<int, Device*(*)()> device_map;

template<typename T> Device *createNewDevice() { return new T; }

GLFWwindow       *W_CreateWindow(int width, int height, const char *title = "Fucking GraphX", bool make_context_current = true);
void              W_SwapAndClear(GLFWwindow *w_window, glm::vec3 w_clear_color = glm::vec3(0.0f));
void              R_StoreBuffers();
void              R_GL_BufferMeshes();
void              R_Render(std::mutex &state_mutex, float interpolation_time);
void              R_GL_Render(std::mutex &mutex, float interpolation_time);
void              R_RenderStage(glm::mat4 projection_matrix, unsigned int shader_index);
graphx::gMeshData M_LoadOBJ(std::string embedded_obj_file);
#endif