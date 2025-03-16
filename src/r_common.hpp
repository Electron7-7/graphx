// r_common.hpp - rendering declarations
#ifndef GRAPHX_RENDERING
#define GRAPHX_RENDERING
#include "Jolt/Jolt.h"
#include "Jolt/Core/Color.h"
#include "graphx_namespace.hpp"
#include "sanity.hpp"
#include "graphx_namespace.hpp"
#include "t_settings.hpp"
#include <models.hpp>
#include <stb_image.h>
#include <array>
#include <mutex>


// Rendering APIs (When I support other APIs, more of these will be added)
#define GRAPHX_OPENGL 917


#define GLSHADER_TYPE_VERTEX	0
#define GLSHADER_TYPE_FRAGMENT	1
#define GLSHADER_TYPE_PROGRAM	2


#define SHADERS_AMOUNT		1
//---------------------------
#define SHADER_BLINN_PHONG	0
#define SHADER_PHONG		1


// Le secret dev texture
#define DOOM_TEXTURE_DIFF				COMP04_5_png
#define DOOM_TEXTURE_SPEC				COMP04_5_SPECULAR_jpg


// BUFFERS ARENT IMPLEMENTED YET, SO THESE ARENT USED!
#define BUFFERS_AMOUNT		5
//---------------------------
#define BUFFER_ERR			0
#define BUFFER_TESTING		1
#define BUFFER_FLATS		2
#define BUFFER_ACTORS		3
#define BUFFER_PROPS		4


#define VAOS_AMOUNT         1
//---------------------------
#define VAO_DEFAULT         0


// #define VBO_SIZE_BYTES      0xA00000 // Equal to 10 MiB (10485760 Bytes)
#define VBO_CATEGORIES      2
//---------------------------
#define VBOS_MESH           0
#define VBOS_UI             1


#define MESH_WAS_BUFFERED   false
#define MESH_IS_BUFFERED    true


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
	unsigned int texture_diffuse = 0;
	unsigned int texture_specular = 0;

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

	std::string mesh_data_name = "";

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

struct RenderState; // Forward Declaration
struct gMeshData; // Forward Declaration

struct RenderCmd
{
public:
	std::string mesh_data_name = "";
	RenderState *current_render_state = nullptr;
	RenderState *previous_render_state = nullptr;
	Material *mesh_material = nullptr;
	Actor *actor_pointer = nullptr;

	bool isRenderable();
};

struct PrimitiveRenderCmd
{
	glm::vec3 vertex_1 = glm::vec3(0.0f);
	glm::vec3 vertex_2 = glm::vec3(0.0f);
	glm::vec3 vertex_3 = glm::vec3(0.0f);
	glm::vec3 normals_1 = glm::vec3(0.0f);
	glm::vec3 normals_2 = glm::vec3(0.0f);
	glm::vec3 normals_3 = glm::vec3(0.0f);
	glm::vec2 uvs_1 = glm::vec2(0.0f);
	glm::vec2 uvs_2 = glm::vec2(0.0f);
	glm::vec2 uvs_3 = glm::vec2(0.0f);
	glm::vec3 colors_1 = glm::vec3(0.0f);
	glm::vec3 colors_2 = glm::vec3(0.0f);
	glm::vec3 colors_3 = glm::vec3(0.0f);

	graphx::types::gPrimitive primitive_type = graphx::types::primitive::FOO;
	Material *primitive_material_override = nullptr; // In case you want something other than vertex colors

	PrimitiveRenderCmd(glm::vec3 new_vertex_1, glm::vec3 new_vertex_2, glm::vec3 new_vertex_color = glm::vec3(0.0f));
	PrimitiveRenderCmd(glm::vec3 new_vertex_1, glm::vec3 new_vertex_2, glm::vec3 new_vertex_3, glm::vec3 new_vertex_color = glm::vec3(0.0f));
	PrimitiveRenderCmd(JPH::RVec3Arg new_vertex_1, JPH::RVec3Arg new_vertex_2, JPH::ColorArg new_vertex_color = JPH::ColorArg());
	PrimitiveRenderCmd(JPH::RVec3Arg new_vertex_1, JPH::RVec3Arg new_vertex_2, JPH::RVec3Arg new_vertex_3, JPH::ColorArg new_vertex_color = JPH::ColorArg());
};

struct gMeshData
{
public:
	// Buffered mesh data
	bool needed_by_current_theatre = false;
	int vao_index = VAO_DEFAULT;
	unsigned int VBO = 0;
	unsigned int IBO = 0;

	std::vector<float> vertex_positions = {0.0f, 0.0f, 0.0f};
	std::vector<float> vertex_normals = {0.0f, 0.0f, 0.0f};
	std::vector<float> vertex_uvs = {0.0f, 0.0f};
	std::vector<float> vertex_colors = {1.0f, 1.0f, 1.0f};
	std::vector<unsigned int> indices = {};

	gMeshData();
	gMeshData(int init_vao_index, std::vector<unsigned int> init_indices, std::vector<float> init_positions, std::vector<float> init_normals = {0.0f, 0.0f, 0.0f}, std::vector<float> init_uvs = {0.0f, 0.0f}, std::vector<float> init_colors = {1.0f, 1.0f, 1.0f});
	gMeshData(int init_vao_index, std::vector<float> init_positions, std::vector<float> init_normals = {0.0f, 0.0f, 0.0f}, std::vector<float> init_uvs = {0.0f, 0.0f}, std::vector<float> init_colors = {1.0f, 1.0f, 1.0f});

	std::vector<float> getVertexData();
	unsigned long getVertexDataSize();
	bool operator==(const gMeshData &compared_with) const;

private:
	// For the time being, I'm removing indices from the rendering process
	// std::vector<unsigned int> vertex_indices;
};

// Variables found in r_renderer.cpp
extern std::array<unsigned int, VAOS_AMOUNT> VAOs; // Only one VAO for now but I expect to need more down the line
extern std::vector<GLShader *> shaders; // Same for shaders
extern std::map<std::string, gMeshData> mesh_data_map;
extern int graphx_api;
extern bool time_to_render;
extern bool time_to_store_buffers;
extern bool do_interpolation;
extern int shader_debug_value;
extern unsigned int shader_index;
extern glm::vec2 main_window_size;
extern float camera_near;
extern float camera_far;
extern int shader_debug_value;
extern unsigned int shader_index;
extern bool jolt_debug_render;
// Variables found in r_common.cpp
extern std::map<int, Device*(*)()> device_map;

template<typename T> Device *createNewDevice() { return new T; }

GLFWwindow *W_CreateWindow(int width, int height, const char *title = "Fucking GraphX", bool make_context_current = true);
void        W_SwapAndClear(GLFWwindow *w_window, glm::vec3 w_clear_color = glm::vec3(0.0f));
void        R_BufferMeshes();
void        R_Render(std::mutex &state_mutex, float interpolation_time);
void        R_RenderStage(glm::mat4 projection_matrix, unsigned int shader_index);
void        R_BufferRenderCmd(RenderCmd render_command);
void        R_InitializeRenderingAPI();
std::string M_LoadModelFile(std::string file_path, std::string file_extension);
gMeshData   M_LoadOBJ(std::string embedded_obj_file);
std::string M_GetOBJName(std::string file_as_string);
#endif
