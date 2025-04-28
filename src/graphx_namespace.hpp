#ifndef GRAPHX_NAMESPACE
#define GRAPHX_NAMESPACE
#include <glm/vec3.hpp>
#include <unordered_map>
#include <any>
#include <string>

#define GRAPHXTHEATRE_EXTENSION std::string(".gt")
#define UID_EMPTY -1 // Just to keep things consistent

namespace graphx
{
	typedef std::pair<int, std::any> gSetting; // The `int` in `graphx::gSetting` identifies the type; type identifiers can be found in `t_common.hpp`
	typedef std::unordered_map<std::string, gSetting> gSettings;

	namespace orientation
	{
		extern glm::vec3 up;
		extern glm::vec3 front;
		extern glm::vec3 right;
	}

	namespace state
	{
		extern bool loading_new_main_theatre;
	}

	namespace debug
	{
		extern bool actor_debug_menu_open;
		extern float actor_debug_menu_text_scale;
	}

	namespace error
	{
		namespace rendercmd
		{
			inline constexpr int MISSING_VBO_NAME           = 1 << 0; // 1
			inline constexpr int MISSING_MESH_DATA_SIZE     = 1 << 1; // 2
			inline constexpr int MISSING_MESH_DATA_OFFSET   = 1 << 2; // 4
			inline constexpr int MISSING_BOTH_RENDER_STATES = 1 << 3; // 8

		}
	}

	namespace rendering
	{
		inline constexpr unsigned int GRAPHX_OPENGL = 917; // API identifier (more to be added)

		inline constexpr unsigned int SHADERS_AMOUNT             = 7;
		//-----------------------------------------------------------
		inline constexpr unsigned int SHADER_DEFAULT             = 0;
		inline constexpr unsigned int SHADER_FONTS_2D            = 1;
		inline constexpr unsigned int SHADER_FONTS_3D            = 2;
		inline constexpr unsigned int SHADER_SKYBOX              = 3;
		inline constexpr unsigned int SHADER_DEBUG_FULLBRIGHT    = 4;
		inline constexpr unsigned int SHADER_DEBUG_NORMALS       = 5;
		inline constexpr unsigned int SHADER_DEBUG_VERTEX_COLORS = 6;

		inline unsigned int current_shader = SHADER_DEFAULT;

		inline constexpr unsigned int VAOS_AMOUNT        = 3;
		//---------------------------------------------------
		inline constexpr unsigned int VAO_DEFAULT        = 0;
		inline constexpr unsigned int VAO_SKYBOX         = 1;
		inline constexpr unsigned int VAO_TEXT           = 2;


		inline bool  do_interpolation = true; // For testing when I change the interpolation method to be more like GZDoom
		inline int   graphx_api = GRAPHX_OPENGL;
		inline float main_window_width = 1280.0f;
		inline float main_window_height = 720.0f;
		inline float camera_near = 0.1f;
		inline float camera_far = 10000.0f;
		inline bool  lighting_switch_diffuse = true;
		inline bool  lighting_switch_specular = true;
		inline bool  lighting_switch_ambient = true;
	}
}
#endif