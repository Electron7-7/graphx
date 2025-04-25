#ifndef GRAPHX_COMMON_RENDERING_FORWARD_DECLARATIONS
#define GRAPHX_COMMON_RENDERING_FORWARD_DECLARATIONS
#include <Jolt/Jolt.h>
#include <Jolt/Renderer/DebugRendererSimple.h>
#include <glfw_fwd.hpp>
#include <glm/fwd.hpp>
#include <string>
#include <mutex>
#include <map>

// Devices
struct Device;
struct Environment;
struct Texture;
struct Material;
struct Model;
struct Sprite;
struct Collider; // Collider declared here too, despite being in g_jolt.hpp (mainly bc it's a Device)

// Render Commands & Helpers (none of these are derived)
struct RenderCmd;
struct LightRenderCmd;
struct TextRenderCmd;
struct RenderCommands;

// Other
struct Mesh;
struct GLShader;
struct LightData;

// Variables
extern std::map<std::string, Mesh> mesh_data_storage;
extern std::map<std::string, Texture> texture_storage;
extern bool time_to_render;
extern bool time_to_store_buffers;

// Functions
GLFWwindow *W_CreateWindow(int width, int height, const char *title, bool make_context_current);
void        W_SwapAndClear(GLFWwindow *w_window, glm::vec4 w_clear_color);
void        R_InitializeRenderingAPI();
void        R_BufferMeshesAndTextures();
void        R_BufferRenderCommands(RenderCommands render_commands);
void        R_BufferRenderCmd(RenderCmd render_command);
void        R_BufferRenderCmd(LightRenderCmd light_render_command);
void        R_BufferRenderCmd(TextRenderCmd text_render_command);
void        R_Render(std::mutex &state_mutex, float interpolation_time);
std::string T_LoadImageFile(std::string file_path);
std::string M_LoadModelFile(std::string file_path, std::string file_extension);
Mesh    M_LoadOBJ(std::string embedded_obj_file);
#endif