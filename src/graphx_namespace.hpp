#ifndef GRAPHXNAMESPACE
#define GRAPHXNAMESPACE
#include <glm/fwd.hpp>
#include <array>

#ifdef COMPILER_FORWARD_DECLARATIONS // Forward Declarations
struct Theatre;
class Actor;
class GraphXPlayer;
class LightFlashlight;
struct Device;
struct Collider;
struct Mesh;
struct Material;
struct Environment;
class GraphXTheatreInterpreter;
#endif

#define GRAPHXTHEATRE_EXTENSION std::string(".gt")

namespace graphx
{
	struct CurrentTheatreHandler
	{
	public:
		Theatre* getCurrentTheatre() const;
		void swapBuffer();

	private:
		friend GraphXTheatreInterpreter;

		static unsigned int theatre_buffer_index;
		static std::array<Theatre, 2> theatre_buffer;
	};

	extern GraphXTheatreInterpreter Interpreter;
	extern CurrentTheatreHandler TheatreHandler;

	namespace debug
	{
		extern bool actor_debug_menu_open;
		extern float actor_debug_menu_text_scale;
		extern Mesh light_debug_mesh;
	}

	namespace missing
	{
		extern Device device;
		extern Collider collider;
		extern Mesh mesh;
		extern Material material;
		extern Actor actor;
		extern LightFlashlight temporary_backup_flashlight;
	}

	namespace identifiers
	{
		namespace primitive
		{
			constexpr int FOO      = -1;
			constexpr int LINE     =  0;
			constexpr int TRIANGLE =  1;
			constexpr int TEXT     = -1; //< Text not supported yet!
		}
	}

	namespace orientation
	{
		extern glm::vec3 up;
		extern glm::vec3 front;
		extern glm::vec3 right;
	}

	namespace rendering
	{
		constexpr unsigned int GRAPHX_OPENGL = 917; // API identifier (more to be added)

		constexpr unsigned int SHADERS_AMOUNT             = 7;
		//-----------------------------------------------------------
		constexpr unsigned int SHADER_DEFAULT             = 0;
		constexpr unsigned int SHADER_FONTS_2D            = 1;
		constexpr unsigned int SHADER_FONTS_3D            = 2;
		constexpr unsigned int SHADER_SKYBOX              = 3;
		constexpr unsigned int SHADER_DEBUG_FULLBRIGHT    = 4;
		constexpr unsigned int SHADER_DEBUG_NORMALS       = 5;
		constexpr unsigned int SHADER_DEBUG_VERTEX_COLORS = 6;

		inline unsigned int current_shader = SHADER_DEFAULT;

		constexpr unsigned int VAOS_AMOUNT        = 3;
		//---------------------------------------------------
		constexpr unsigned int VAO_DEFAULT        = 0;
		constexpr unsigned int VAO_SKYBOX         = 1;
		constexpr unsigned int VAO_TEXT           = 2;


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
