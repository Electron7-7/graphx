#ifndef GRAPHXNAMESPACE
#define GRAPHXNAMESPACE
#include <glm/glm.hpp>
#include <any>
#include <unordered_map>
#include <string>

#ifdef COMPILER_FORWARD_DECLARATIONS // Forward Declarations
struct Theatre;
class GraphXPlayer;
struct Environment;
#endif

#define GRAPHXTHEATRE_EXTENSION std::string(".gt")

namespace graphx
{
	namespace current
	{
		extern Theatre theatre;
	}

	namespace debug
	{
		inline bool actor_debug_menu_open = false;
		inline float actor_debug_menu_text_scale = 1.8f;
	}

	namespace error
	{
		namespace rendercmd
		{
			inline constexpr int MISSING_VBO_NAME           = 1 << 0; // 1
			inline constexpr int MISSING_MESH_DATA_SIZE     = 1 << 1; // 2
			inline constexpr int MISSING_MESH_DATA_OFFSET   = 1 << 2; // 4
			inline constexpr int MISSING_BOTH_RENDER_STATES = 1 << 3; // 8

		};
	};

	namespace identifiers
	{
		namespace primitive
		{
			inline constexpr int FOO      = -1;
			inline constexpr int LINE     =  0;
			inline constexpr int TRIANGLE =  1;
			inline constexpr int TEXT     = -1; //< Text not supported yet!
		};
	};

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
	};

	/// The `int` in `graphx::gSetting` identifies the type; type identifiers can be found in `t_common.hpp`.
	typedef std::pair<int, std::any> gSetting;
	typedef std::unordered_map<std::string, gSetting> gSettings;
}
#endif
