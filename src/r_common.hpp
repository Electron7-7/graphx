#include <memory>
#ifndef GRAPHX_RENDERING_COMMON
#include <models.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <map>
#define GRAPHX_RENDERING_COMMON

#ifdef COMPILER_FORWARD_DECLARATIONS // This is to keep forward declarations from causing issues when including header files
struct Material; // REMOVE THIS LATER
#endif

struct Character
{
    unsigned int texture_id;
    int size_x;
    int size_y;
    int bearing_x; // Offset from baseline to left of glyph
    int bearing_y; // Offset from baseline to top of glyph
    int advance;   // Offset to advance to next glyph

    Character() = default;
    Character(unsigned int init_texture_id, int init_size_x, int init_size_y, int init_bearing_x, int init_bearing_y, int init_advance);
    Character(unsigned int init_texture_id, glm::vec2 init_size, glm::vec2 init_bearing, int init_advance);
};

struct Font
{
    std::string font_name;
    std::map<char, Character> character_set;
    unsigned int texture_array_id;
    unsigned int VBO;

    Font() = default;
    Font(std::string init_font_name);
};

struct RenderState
{
    glm::vec3 render_position = glm::vec3(0.0f);
    glm::quat render_quaternion = glm::quat();
    glm::vec3 render_scale = glm::vec3(1.0f);
};

struct RenderCmd
{
public:
    glm::vec4 debug_highlight_color = glm::vec4(0.0f);
    bool is_light_debug_mesh = false;
    std::string mesh_data_name = ERROR_MODEL;
    RenderState current_render_state = RenderState();
    RenderState previous_render_state = RenderState();
    std::shared_ptr<Material> mesh_material = nullptr; // REPLACE THIS WITH MATERIAL UID
    // int mesh_material = -1;

    RenderCmd() = default;

    const bool isValid() const;
};

struct LightRenderCmd
{
public:
    unsigned int light_type = POINT_LIGHT;

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

    LightRenderCmd(const unsigned int = POINT_LIGHT);
    
    static const unsigned int POINT_LIGHT       = 1;
    static const unsigned int SPOT_LIGHT        = 2;
    static const unsigned int DIRECTIONAL_LIGHT = 3;
};

struct TextRenderCmd
{
public:
    std::string text = "";
    float position_x = 0.0f;
    float position_y = 0.0f;
    float scale = 0.0f;
    glm::vec3 color = glm::vec3(1.0f);
    bool is_debug_label = false;
    RenderState* render_state = nullptr; // Along with is3D(), find out a way to avoid using a pointer here

    TextRenderCmd() = default;
    TextRenderCmd(std::string text_to_render, std::string font_name = "Arial", float position_x = 0.0f, float position_y = 0.0f, float scale = 1.0f, glm::vec3 color = glm::vec3(1.0f));

    std::string getFontName() const; // Will always return a valid font name
    void setFontName(const std::string&);

    bool is3D() const; // Find a way to avoid this, too; idea: the default RenderState will result in a non-perspective view (orthographic/2D)

private:
    std::string font_name = "Arial";
};

struct RenderCommands
{
    RenderCmd render_command;
    LightRenderCmd light_render_command;
    TextRenderCmd text_render_command;
};

// This is used by Actors and Devices to buffer their debug label; I'll probably phase out using this function directly, but for now it's gotta go in here as well as in `r_rendering.hpp`
void R_BufferRenderCmd(TextRenderCmd text_render_command);
#endif