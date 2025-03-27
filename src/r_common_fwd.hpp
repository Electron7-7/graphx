#ifndef GRAPHX_COMMON_RENDERING_FORWARD_DECLARATIONS
#define GRAPHX_COMMON_RENDERING_FORWARD_DECLARATIONS
#include <Jolt/Jolt.h>
#include <Jolt/Renderer/DebugRendererSimple.h>
#include <glfw_fwd.hpp>
#include <glm/fwd.hpp>
#include <glm/vec3.hpp>
#include <vector>
#include <string>
#include <mutex>
#include <map>

// Devices
struct Device;
struct Environment;
struct Texture;
struct Material;
struct Mesh;
struct Sprite;

// RenderCmds (none of these are derived)
struct RenderCmd;
struct LightRenderCmd;
struct PrimitiveRenderCmd;

// Other
struct MeshData;
struct GLShader;

// Variables
// extern std::array<unsigned int, VAOS_AMOUNT> VAOs;
extern std::vector<GLShader *> shaders;
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
extern std::map<int, Device*(*)()> device_map;

// Functions
template<typename T> Device *createNewDevice();

GLFWwindow *W_CreateWindow(int width, int height, const char *title, bool make_context_current);
void        W_SwapAndClear(GLFWwindow *w_window, glm::vec3 w_clear_color);
void        R_BufferMeshesAndTextures();
void        R_DrawPrimitive(PrimitiveRenderCmd primitive);
void        R_GradientBackground(glm::vec4 top, glm::vec4 bottom);
void        R_Render(std::mutex &state_mutex, float interpolation_time);
void        R_GL_BufferTextures();
void        R_GL_BufferMeshes();
void        R_GL_RenderPrimitives(RenderCmd *render_command);
void        R_GL_Render(std::mutex &mutex, float interpolation_time, JPH::DebugRenderer *debug_renderer);
void        R_RenderStage(glm::mat4 projection_matrix, unsigned int shader_index);
void        R_BufferRenderCmd(RenderCmd render_command);
void        R_BufferRenderCmd(LightRenderCmd light_render_command);
void        R_BufferRenderCmd(PrimitiveRenderCmd primitive_render_command);
void        R_InitializeRenderingAPI();
std::string T_LoadImageFile(std::string file_path);
void        M_GL_BufferMaterialTexture(unsigned int &texture_id, unsigned char *texture_buffer);
std::string M_LoadModelFile(std::string file_path, std::string file_extension);
MeshData    M_LoadOBJ(std::string embedded_obj_file);
#endif