// r_common.hpp - rendering declarations
#ifndef GRAPHX_RENDERING
#define GRAPHX_RENDERING
#include "graphx_namespace.hpp"
#include "t_settings.hpp"
#include <images.h>
#include <models.hpp>
#include <glfw_fwd.hpp>
#include <Jolt/Jolt.h>
#include <Jolt/Core/Color.h>
#include <array>
#include <mutex>


#define SHADER_DEBUG_NORMALS       1
#define SHADER_DEBUG_VERTEX_COLORS 2


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
#define DOOM_TEXTURE_DIFF   COMP04_5_png
#define DOOM_TEXTURE_SPEC   COMP04_5_SPECULAR_jpg


#define VAOS_AMOUNT			2
//---------------------------
#define VAO_DEFAULT         0
#define VAO_PRIMITIVES      1


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

	long getType();
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

struct Texture final : public Device
{
public:
	unsigned int texture_id = 0;
	unsigned char *texture_data = MISSING_TEXTURE_jpg;

	Texture();
	Texture(unsigned char *init_texture_data);
	Texture(const char *init_texture_data);
	Texture(std::string init_texture_data);

	void loadSettings(graphx::gSettings new_settings = empty_settings) override;
};

struct Material final : public Device
{
	std::string diffuse_texture_name = MISSING_TEXTURE;
	std::string specular_texture_name = MISSING_TEXTURE;

	unsigned int texture_diffuse = 0;
	unsigned int texture_specular = 0;
	unsigned char* embedded_texture_diffuse = MISSING_TEXTURE_jpg;
	unsigned char* embedded_texture_specular = FLAT_SPEC_jpg;

	glm::vec3 color = glm::vec3(1.0f);
	int specular_sharpness = 16;
	float specular_strength = 1.0f;
	bool mat_fullbright = false;

	Material();
	Material(bool is_fullbright, glm::vec3 init_color = glm::vec3(1.0f));
	Material(std::string init_diffuse_texture_name, std::string init_specular_texture_name = NO_TEXTURE, int init_specular_sharpness = 16, float init_specular_strength = 0.0f, glm::vec3 init_color = glm::vec3(1.0f));
	// Material(unsigned char *init_diffuse_texture, unsigned char *init_specular_texture = NO_TEXTURE_jpg, int init_specular_sharpness = 16, float init_specular_strength = 0.0f, glm::vec3 init_color = glm::vec3(1.0f));
	Material(glm::vec3 init_color, float init_specular_strength = 0.5f, unsigned int init_specular_sharpness = 32);

	void loadSettings(graphx::gSettings new_settings = empty_settings) override;
};

struct Mesh : public Device
{
	std::string name = "Untitled Mesh";
	// Material *material = new Material();
	Material *material = new Material();

	unsigned int VBO = 0;
	unsigned int IBO = 0;
	bool is_buffered = false;
	glm::vec3 mesh_scale = glm::vec3(1.0f);
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

struct RenderState;                                   // Forward Declaration
struct LightData;                                     // Forward Declaration
std::string M_GetOBJName(std::string file_as_string); // Forward Declaration

struct LightRenderCmd
{
public:
	LightData *light_data = nullptr;
	RenderState *current_render_state = nullptr;
	RenderState *previous_render_state = nullptr;
	int light_type = graphx::classes::LIGHT;

	bool renderDebugMesh();
};

struct RenderCmd
{
public:
	bool is_light_debug_mesh = false;
	std::string mesh_data_name = M_GetOBJName(ERROR_obj);
	RenderState *current_render_state = nullptr;
	RenderState *previous_render_state = nullptr;
	Material mesh_material;

	RenderCmd() = default;
	RenderCmd(LightRenderCmd &light_render_command, glm::vec3 light_debug_material_color);

	bool isRenderable();
};

struct PrimitiveRenderCmd
{
	unsigned int array_offset;
	int primitive_type = graphx::identifiers::primitive::FOO;
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
	Material *primitive_material_override = nullptr; // In case you want something other than vertex colors

	PrimitiveRenderCmd(glm::vec3 new_vertex_1, glm::vec3 new_vertex_2, glm::vec3 new_vertex_color = glm::vec3(0.0f));
	PrimitiveRenderCmd(glm::vec3 new_vertex_1, glm::vec3 new_vertex_2, glm::vec3 new_vertex_3, glm::vec3 new_vertex_color = glm::vec3(0.0f));
	PrimitiveRenderCmd(JPH::RVec3Arg new_vertex_1, JPH::RVec3Arg new_vertex_2, JPH::ColorArg new_vertex_color = JPH::ColorArg());
	PrimitiveRenderCmd(JPH::RVec3Arg new_vertex_1, JPH::RVec3Arg new_vertex_2, JPH::RVec3Arg new_vertex_3, JPH::ColorArg new_vertex_color = JPH::ColorArg());

	std::vector<float> getVertices();
	unsigned int numberOfVertices();
};

struct MeshData
{
	int is_in_use = graphx::identifiers::mesh_data::NOT_CHECKED;
	int VAO_index = VAO_DEFAULT;
	unsigned int VBO;
	unsigned int IBO;

	std::string name = "";

	std::vector<glm::vec3> vertex_positions;
	std::vector<glm::vec3> vertex_normals;
	std::vector<glm::vec2> vertex_uvs;
	std::vector<glm::vec3> vertex_colors;
	std::vector<gmath::uintvec3> vertex_indices;

	MeshData();
	MeshData(int init_vao_index, std::vector<glm::vec3> init_positions, std::vector<glm::vec3> init_normals = {}, std::vector<glm::vec2> init_uvs = {}, std::vector<glm::vec3> init_colors = {}, std::vector<gmath::uintvec3> init_indices = {});
	MeshData(int init_vao_index, std::vector<float> init_positions, std::vector<float> init_normals = {}, std::vector<float> init_uvs = {}, std::vector<float> init_colors = {}, std::vector<unsigned int> init_indices = {});

	// This implementation of MeshData::addVertex assumes that the floats contained in the "vertex" argument are in this order:
	//
	//     vertex[0-2] : position (X, Y, Z)
	//
	//     vertex[3-5] : normal   (X, Y, X)
	//
	//     vertex[6-7] : uv       (X, Y)
	//
	//     vertex[8-10]: color    (R, G, B)
	void addVertex(std::vector<float> vertex);
	void addVertex(glm::vec3 position, glm::vec3 normal = glm::vec3(0.0f), glm::vec2 uv = glm::vec2(0.0f), glm::vec3 color = glm::vec3(1.0f));
	void addVertex(float position_x, float position_y, float position_z, float normal_x, float normal_y, float normal_z, float uv_x, float uv_y, float color_x, float color_y, float color_z);
	void addIndex(gmath::uintvec3 indices);
	void addIndex(unsigned int index_1, unsigned int index_2, unsigned int index_3);
	void fixOBJData();
	bool hasValidIndices();
	std::vector<float> vertices();
	std::vector<unsigned int> indices();
	size_t vertices_count();
	size_t vertices_size();
	size_t indices_count();
	size_t indices_size();
};

// Variables found in r_renderer.cpp
extern std::array<unsigned int, VAOS_AMOUNT> VAOs; // Only one VAO for now but I expect to need more down the line
extern std::vector<GLShader *> shaders; // Same for shaders
extern std::map<std::string, MeshData> mesh_data_storage;
extern std::map<std::string, Texture> texture_storage;
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
extern bool lighting_switch_diffuse;
extern bool lighting_switch_specular;
extern bool lighting_switch_ambient;
// Variables found in r_common.cpp
extern std::map<int, Device*(*)()> device_map;

template<typename T> Device *createNewDevice() { return new T; }

GLFWwindow *W_CreateWindow(int width, int height, const char *title = "Fucking GraphX", bool make_context_current = true);
void        W_SwapAndClear(GLFWwindow *w_window, glm::vec3 w_clear_color = glm::vec3(0.0f));
void        R_BufferMeshes();
void        R_GL_BufferMeshes();
void        R_Render(std::mutex &state_mutex, float interpolation_time);
void        R_GL_RenderPrimitives(RenderCmd *render_command);
void        R_GL_Render(std::mutex &mutex, float interpolation_time);
void        R_RenderStage(glm::mat4 projection_matrix, unsigned int shader_index);
void        R_BufferRenderCmd(RenderCmd render_command);
void        R_BufferRenderCmd(LightRenderCmd light_render_command);
void        R_BufferRenderCmd(PrimitiveRenderCmd primitive_render_command);
void        R_InitializeRenderingAPI();
std::string T_LoadImageFile(std::string file_path);
void        M_GL_BufferMaterialTexture(unsigned int &texture_id, unsigned char *texture_buffer);
std::string M_LoadModelFile(std::string file_path, std::string file_extension);
std::string M_GetOBJName(std::string file_as_string);
MeshData    M_LoadOBJ(std::string embedded_obj_file);
#endif
