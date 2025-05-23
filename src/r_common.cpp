#include "r_common.hpp"
#include "r_rendering.hpp"
#include <gmath.hpp>
#include <glm/gtx/component_wise.hpp>

//
// Character
//
Character::Character(unsigned int init_texture_id, int init_size_x, int init_size_y, int init_bearing_x, int init_bearing_y, int init_advance)
{
	texture_id = init_texture_id;
	size_x = init_size_x;
	size_y = init_size_y;
	bearing_x = init_bearing_x;
	bearing_y = init_bearing_y;
	advance = init_advance;
}

Character::Character(unsigned int init_texture_id, glm::vec2 init_size, glm::vec2 init_bearing, int init_advance)
: Character(init_texture_id, init_size.x, init_size.y, init_bearing.x, init_bearing.y, init_advance)
{}

//
// Font
//
Font::Font(std::string init_font_name)
: font_name(init_font_name)
{}

//
// LightRenderCmd
//
bool LightRenderCmd::isValid() const
{
	return (light_type != LightRenderCmd::EMPTY);
}

//
// TextRenderCmd
//
TextRenderCmd::TextRenderCmd(std::string init_text, float init_position_x, float init_position_y, float init_scale, glm::vec3 init_color)
{
	font_name = "Arial";
	text = init_text;
	position_x = init_position_x;
	position_y = init_position_y;
	scale = init_scale;
	color = init_color;
}

TextRenderCmd::TextRenderCmd(std::string init_font_name, std::string init_text, float init_position_x, float init_position_y, float init_scale, glm::vec3 init_color)
: TextRenderCmd(init_text, init_position_x, init_position_y, init_scale, init_color)
{
	font_name = (font_map.contains(init_font_name)) ? init_font_name : "Arial";
}

bool TextRenderCmd::isValid() const
{
	return (font_map.contains(font_name) && scale > 0.0f);
}

bool TextRenderCmd::is3D() const
{
	return (render_state != nullptr);
}
