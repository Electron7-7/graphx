// r_common.hpp - rendering
#ifndef GRAPHX_RENDERING
#define GRAPHX_RENDERING
#include "graphx_namespace.hpp"
#include "t_settings.hpp"
#include <images.h>
#include <models.hpp>
#include <glfw_fwd.hpp>
#include <Jolt/Jolt.h>
#include <Jolt/Core/Color.h>
#include <Jolt/Renderer/DebugRendererSimple.h>
#include <array>
#include <mutex>

#define GLSHADER_TYPE_VERTEX   0
#define GLSHADER_TYPE_FRAGMENT 1
#define GLSHADER_TYPE_PROGRAM  2

struct GLShader
{
	unsigned int id;

	GLShader(std::string vertex_shader_code, std::string fragment_shader_code);

	template<typename T> void setUniform(const std::string &name, T value) const;
	void buildShader(std::string vertex_shader_code, std::string fragment_shader_code);
};

struct Device
{
	std::string name = "Untitled Device";
	graphx::gSettings settings = empty_settings;

	Device();
	virtual ~Device() = default;

	graphx::gClass &getType();
	void setName(std::string new_name);
	void setName(char *new_name);
	std::string getName();

	virtual void initialize();
	virtual void loadSettings(graphx::gSettings new_settings = empty_settings);
	virtual void prepForDestruction();
	virtual long getUID();
	virtual void setUID(long manual_uid);

protected:
	graphx::gClass my_type;
	long UID = -1; // A UID of -1 means it's not been set yet
	bool ready_to_destroy = false;
};

struct Environment final : public Device // Will be extended
{
	glm::vec3 ambient_light_color = glm::vec3(1.0f);
	float ambient_light_amount = 0.0f;
	// bool ambient_lighting_enabled = true;
	// float ambient_light_strength = 0.05f;

	Environment(std::string init_name = "UNTITLED_ENVIRONMENT", bool enable_ambient_light = false, float init_ambient_light_amount = 0.05f, glm::vec3 init_ambient_light_color = glm::vec3(1.0f));

	// float getAmbientLight();
	void loadSettings(graphx::gSettings new_settings = empty_settings) override;
};

struct Texture final : public Device
{
public:
	bool is_in_use = false;
	unsigned int texture_id = 0;
	std::vector<unsigned char *> texture_data = {MISSING_TEXTURE_jpg};
	std::vector<unsigned int> texture_size = {MISSING_TEXTURE_jpg_len};

	Texture();
	Texture(std::vector<unsigned char *> init_texture_data, std::vector<unsigned int> init_texture_size);
	Texture(std::vector<const char *> init_texture_data, std::vector<unsigned int> init_texture_size);
	Texture(std::vector<std::string > init_texture_data, std::vector<unsigned int> init_texture_size);
	Texture(unsigned char * init_texture_data, unsigned int init_texture_size);
	Texture(const char * init_texture_data, unsigned int init_texture_size);
	Texture(std::string  init_texture_data, unsigned int init_texture_size);

	void loadSettings(graphx::gSettings new_settings = empty_settings) override;
};

struct Material final : public Device
{
	std::string diffuse_texture_name = MISSING_TEXTURE;
	std::string specular_texture_name = MISSING_TEXTURE;

	glm::vec3 color = glm::vec3(1.0f);
	int specular_sharpness = 16;
	float specular_strength = 1.0f;
	bool mat_fullbright = false;
	bool use_texture = true;

	Material();
	Material(bool is_fullbright, glm::vec3 init_color = glm::vec3(1.0f));
	Material(std::string init_diffuse_texture_name, std::string init_specular_texture_name = NO_TEXTURE, int init_specular_sharpness = 16, float init_specular_strength = 0.0f, glm::vec3 init_color = glm::vec3(1.0f));
	Material(glm::vec3 init_color, float init_specular_strength = 0.5f, unsigned int init_specular_sharpness = 32);

	void loadSettings(graphx::gSettings new_settings = empty_settings) override;
};


struct MeshData
{
	unsigned int base_vertex = -1;
	unsigned int base_index = -1;
	std::string debug_name = ""; // Debugging variable; remove later (watch me forget this)
	bool is_in_use = false;

	std::vector<glm::vec3> vertex_positions;
	std::vector<glm::vec3> vertex_normals;
	std::vector<glm::vec2> vertex_uvs;
	std::vector<glm::vec3> vertex_colors;
	std::vector<gmath::uintvec3> vertex_indices;

	MeshData();
	MeshData(int init_vao_index, std::vector<glm::vec3> init_positions, std::vector<glm::vec3> init_normals = {}, std::vector<glm::vec2> init_uvs = {}, std::vector<glm::vec3> init_colors = {}, std::vector<gmath::uintvec3> init_indices = {});
	MeshData(int init_vao_index, std::vector<float> init_positions, std::vector<float> init_normals = {}, std::vector<float> init_uvs = {}, std::vector<float> init_colors = {}, std::vector<unsigned int> init_indices = {});

	// This implementation of `MeshData::addVertex` assumes that the floats contained in `vertex` are in this order:
	//
	//   `vertex[0-2]`  - position - (X, Y, Z)
	//
	//   `vertex[3-5]`  - normal   - (X, Y, X)
	//
	//   `vertex[6-7]`  - uv       - (X, Y)
	//
	//   `vertex[8-10]` - color    - (R, G, B)
	void addVertex(std::vector<float> vertex);
	void addVertex(glm::vec3 position, glm::vec3 normal = glm::vec3(0.0f), glm::vec2 uv = glm::vec2(0.0f), glm::vec3 color = glm::vec3(1.0f));
	void addVertex(float position_x, float position_y, float position_z, float normal_x, float normal_y, float normal_z, float uv_x, float uv_y, float color_x, float color_y, float color_z);
	void addIndex(gmath::uintvec3 indices);
	void addIndex(unsigned int index_1, unsigned int index_2, unsigned int index_3);
	void fixOBJData();
	const std::vector<float> vertices();
	const std::vector<unsigned int> indices();
	// Simple functions to abstract simple math that I always fuck up (I still end up using the wrong function, anyways)
	size_t vertices_count();
	size_t vertices_size();
	size_t indices_count();
	size_t indices_size();
};

struct Mesh : public Device
{
	Material *material = new Material();

	unsigned int VBO = 0;
	unsigned int IBO = 0;
	bool is_buffered = false;
	glm::vec3 mesh_scale = glm::vec3(1.0f);
	std::string mesh_data_name = ERROR_MODEL;

	Mesh();
	Mesh(Material *new_material, std::string init_mesh_data_name = ERROR_MODEL);
	Mesh(std::string init_mesh_data_name);

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
	graphx::gClass light_type;

	bool renderDebugMesh();
};

struct RenderCmd
{
public:
	bool is_light_debug_mesh = false;
	std::string mesh_data_name = ERROR_MODEL;
	RenderState *current_render_state = nullptr;
	RenderState *previous_render_state = nullptr;
	Material mesh_material;

	RenderCmd() = default;
	RenderCmd(LightRenderCmd &light_render_command, glm::vec3 light_debug_material_color);

	bool isRenderable();
};

struct PrimitiveRenderCmd
{
	int primitive_type = graphx::identifiers::primitive::FOO;
	glm::vec3 vertex_1 = glm::vec3(0.0f);
	glm::vec3 vertex_2 = glm::vec3(0.0f);
	glm::vec3 vertex_3 = glm::vec3(0.0f);
	// glm::vec3 normals_1 = glm::vec3(0.0f);
	// glm::vec3 normals_2 = glm::vec3(0.0f);
	// glm::vec3 normals_3 = glm::vec3(0.0f);
	// glm::vec2 uvs_1 = glm::vec2(0.0f);
	// glm::vec2 uvs_2 = glm::vec2(0.0f);
	// glm::vec2 uvs_3 = glm::vec2(0.0f);
	glm::vec3 colors_1 = glm::vec3(0.0f);
	glm::vec3 colors_2 = glm::vec3(0.0f);
	glm::vec3 colors_3 = glm::vec3(0.0f);
	Material *primitive_material_override = nullptr; // In case you want something other than vertex colors

	PrimitiveRenderCmd(glm::vec3 new_vertex_1, glm::vec3 new_vertex_2, glm::vec3 new_vertex_color = glm::vec3(0.0f));
	PrimitiveRenderCmd(glm::vec3 new_vertex_1, glm::vec3 new_vertex_2, glm::vec3 new_vertex_3, glm::vec3 new_vertex_color = glm::vec3(0.0f));
	PrimitiveRenderCmd(JPH::RVec3Arg new_vertex_1, JPH::RVec3Arg new_vertex_2, JPH::ColorArg new_vertex_color = JPH::ColorArg());
	PrimitiveRenderCmd(JPH::RVec3Arg new_vertex_1, JPH::RVec3Arg new_vertex_2, JPH::RVec3Arg new_vertex_3, JPH::ColorArg new_vertex_color = JPH::ColorArg());
};

extern std::array<unsigned int, graphx::rendering::VAOS_AMOUNT> VAOs; // Todo: change to std::vector? (would make the forward declarations nicer)
extern std::vector<GLShader *> shaders;
extern std::map<std::string, MeshData> mesh_data_storage;
extern std::map<std::string, Texture> texture_storage;
extern bool time_to_render;
extern bool time_to_store_buffers;

GLFWwindow *W_CreateWindow(int width, int height, const char *title = "Fucking GraphX", bool make_context_current = true);
void        W_SwapAndClear(GLFWwindow *w_window, glm::vec3 w_clear_color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
void        R_BufferMeshesAndTextures();
void        R_GL_BufferTextures();
void        R_GL_BufferMeshes();
void        R_DrawPrimitive(PrimitiveRenderCmd primitive);
void        R_GL_DrawSkybox();
void        R_Render(std::mutex &state_mutex, float interpolation_time, JPH::DebugRenderer *debug_renderer);
void        R_GL_Render(std::mutex &mutex, float interpolation_time);
void        R_RenderStage(glm::mat4 projection_matrix, unsigned int shader_index);
void        R_BufferRenderCmd(RenderCmd render_command);
void        R_BufferRenderCmd(LightRenderCmd light_render_command);
void        R_BufferRenderCmd(PrimitiveRenderCmd primitive_render_command);
void        R_InitializeRenderingAPI();
std::string T_LoadImageFile(std::string file_path);
std::string M_LoadModelFile(std::string file_path, std::string file_extension);
MeshData    M_LoadOBJ(std::string embedded_obj_file);

template<typename T> Device *createNewDevice() { return new T; }
#endif