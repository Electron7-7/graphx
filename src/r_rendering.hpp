#ifndef GRAPHX_RENDERING
#include "r_common.hpp"
#include "g_devices.hpp"
#include "graphx_namespace.hpp"
#include <images.h>
#include <models.hpp>
#include <glfw_fwd.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <string>
#include <mutex>
#define GRAPHX_RENDERING

#define GLSHADER_TYPE_VERTEX   0
#define GLSHADER_TYPE_FRAGMENT 1
#define GLSHADER_TYPE_PROGRAM  2

struct GLShader
{
public:
    unsigned int id = 0;

    GLShader() = default;
    GLShader(std::string, std::string);

    template<typename T> void setUniform(const std::string &name, T value) const;

private:
    void GLShaderErrorHandler(const unsigned int&, const bool = false);
};

struct Mesh
{
    unsigned int base_vertex = 0;
    unsigned int base_index = 0;
    std::string debug_name = ""; // Debugging variable; remove later (watch me forget this)
    bool is_in_use = false;

    // Todo: Find a better/more efficient way of holding these values
    std::vector<glm::vec3> vertex_positions;
    std::vector<glm::vec3> vertex_normals;
    std::vector<glm::vec2> vertex_uvs;
    std::vector<glm::vec3> vertex_colors;
    std::vector<gmath::vec3uint> vertex_indices;

    Mesh();
    Mesh(int init_vao_index, std::vector<glm::vec3> init_positions, std::vector<glm::vec3> init_normals = {}, std::vector<glm::vec2> init_uvs = {}, std::vector<glm::vec3> init_colors = {}, std::vector<gmath::vec3uint> init_indices = {});
    Mesh(int init_vao_index, std::vector<float> init_positions, std::vector<float> init_normals = {}, std::vector<float> init_uvs = {}, std::vector<float> init_colors = {}, std::vector<unsigned int> init_indices = {});

    // This implementation of `Mesh::addVertex` assumes that the floats contained in `vertex` are in this order:
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
    void addIndex(gmath::vec3uint indices);
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

extern std::array<unsigned int, graphx::rendering::VAOS_AMOUNT> VAOs; // Todo: change to std::vector or move to graphx::rendering (would make the forward declarations nicer)
extern std::array<GLShader, graphx::rendering::SHADERS_AMOUNT> shaders;
extern std::map<std::string, Mesh> mesh_data_storage;
extern std::map<std::string, Texture> texture_storage;
extern std::map<std::string, Font> font_storage;
extern bool time_to_render;
extern bool time_to_store_buffers;
extern FT_Library freetype;
extern bool enable_default_shader;
// Todo: make this better or remove it
#define USE_DEFAULT       0
#define USE_FULLBRIGHT    1
#define USE_NORMALS       2
#define USE_VERTEX_COLORS 3
extern int debug_render_switches;

GLFWwindow *W_CreateWindow(int width, int height, const char *title = "Fucking GraphX", bool make_context_current = true);
void        W_SwapAndClear(GLFWwindow *w_window, glm::vec3 w_clear_color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
void        F_InitializeFreeType();
void        F_LoadFont(std::string ttf_file_path, std::string font_name);
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