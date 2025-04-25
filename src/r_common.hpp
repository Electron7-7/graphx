#ifndef GRAPHX_RENDERING_COMMON
#include "graphx_namespace.hpp"
#include <models.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#define GRAPHX_RENDERING_COMMON
struct LightData
{
    glm::vec3 color = glm::vec3(0.0f);
    float specular_strength = 0.0f;
    float ambient_strength = 0.0f;
    float energy = 0.0f;

    float attenuation = 0.0f;
    float range = 0.0f;

    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 direction = glm::vec3(0.0f);

    float spot_cutoff = 0.0f;
    float spot_cutoff_fade = 0.0f;

    // Texture *projection_texture; // For later...
};

struct LightRenderCmd
{
public:
    LightData light_data;
    const graphx::gClass* light_type = &graphx::gClass::INVALID_TYPE;

    bool isValid() const;
};

struct RenderCmd
{
public:
    glm::vec4 debug_highlight_color = glm::vec4(0.0f);
    bool is_light_debug_mesh = false;
    std::string mesh_data_name = ERROR_MODEL;
    RenderState *current_render_state = nullptr;
    RenderState *previous_render_state = nullptr;
    Material* mesh_material = nullptr;

    bool isValid() const;
};

struct TextRenderCmd
{
public:
    std::string font_name = "";
    std::string text = "";
    float position_x = 0.0f;
    float position_y = 0.0f;
    float scale = 0.0f;
    glm::vec3 color = glm::vec3(0.0f);
    bool is_debug_label = false;
    RenderState *render_state = nullptr;

    TextRenderCmd() = default;
    TextRenderCmd(std::string init_text, float init_position_x, float init_position_y, float init_scale, glm::vec3 init_color);
    TextRenderCmd(std::string init_font_name, std::string init_text, float init_position_x, float init_position_y, float init_scale, glm::vec3 init_color);

    bool isValid() const;
    bool is3D() const;
};

struct RenderCommands
{
    RenderCmd render_command;
    LightRenderCmd light_render_command;
    TextRenderCmd text_render_command;
};
#endif