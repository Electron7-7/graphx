#ifndef GRAPHX_RENDERING_COMMON
#define GRAPHX_RENDERING_COMMON
#include "g_devices.hpp"
#include <images.h>
#include <models.hpp>
#include <glfw_fwd.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <map>

#define GLSHADER_TYPE_VERTEX   0
#define GLSHADER_TYPE_FRAGMENT 1
#define GLSHADER_TYPE_PROGRAM  2

struct RenderState
{   // Used by Actors to store/send position, rotation, and scale data
	glm::vec3 render_position = glm::vec3(0.0f);
	glm::quat render_quaternion = glm::quat();
	glm::vec3 render_scale = glm::vec3(0.0f);
};

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

// Idea for later:
// Instead of using a struct to send data to a LightRenderCmd,
// what if I just used a float vector/data stream instead? I could
// access specific data like how OpenGL access vertex attributes!
// Pretty over-engineered, but could be a cool idea, I think
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
	// const graphx::gClass* light_type = &graphx::gClass::INVALID_TYPE;
	unsigned int light_type = EMPTY;

	static const unsigned int EMPTY             = 0;
    static const unsigned int POINT_LIGHT       = 1;
    static const unsigned int SPOT_LIGHT        = 2;
    static const unsigned int DIRECTIONAL_LIGHT = 3;

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
	Material mesh_material; // Todo: make this a reference

	bool isValid() const;
};

struct TextRenderCmd
{
public:
	std::string font_name = ""; // Temporary solution
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

// This is used by Actors and Devices to buffer their debug label; I'll probably phase out using this function directly, but for now it's gotta go in here as well as in `r_rendering.hpp`
void R_BufferRenderCmd(TextRenderCmd text_render_command);
// std::string M_LoadModelFile(std::string file_path, std::string file_extension);
#endif