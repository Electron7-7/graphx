#ifndef GRAPHXNAMESPACE
#define GRAPHXNAMESPACE
#include "g_common_fwd.hpp"
#include "r_common_fwd.hpp"
#include <glm/vec3.hpp>
#include <any>
#include <string>

#define GRAPHXTHEATRE_EXTENSION std::string(".gt")

namespace graphx
{
	typedef std::pair<int, std::any> gSetting; // The `int` in `graphx::gSetting` identifies the type; type identifiers can be found in `t_common.hpp`
	typedef std::unordered_map<std::string, gSetting> gSettings;
	inline const std::string empty_settings_identifier = "FUCKYOU";
	inline const graphx::gSettings empty_settings = {{empty_settings_identifier, graphx::gSetting(-1, {})}};

	namespace current
	{
		extern Theatre theatre; // This guy is defined in `g_theatre.cpp`
	}

	namespace debug
	{
		extern bool actor_debug_menu_open;
		extern float actor_debug_menu_text_scale;
	}

	struct gUID
	{
	public:
		static const gUID INVALID_UID; // The default constructor is INVALID_UID

		const int id = -1;
		std::string name = "A Class With No Name";

		gUID(const int, const std::string&);
		gUID(const gUID&);
		gUID();

		const std::string toString() const;
		// gUID& operator=(const gUID& other) { return *this; } // Pretty sure this'll fail bc of the const int

		// Comparing gUID to gUID
		const bool operator==(const gUID&) const;
		const bool operator!=(const gUID&) const;
		const bool operator< (const gUID&) const;
		const bool operator> (const gUID&) const;
		const bool operator<=(const gUID&) const;
		const bool operator>=(const gUID&) const;

		// Comparing gUID to int
		const bool operator==(const int&) const;
		const bool operator!=(const int&) const;
		const bool operator< (const int&) const;
		const bool operator> (const int&) const;
		const bool operator<=(const int&) const;
		const bool operator>=(const int&) const;

		// Comparing gUID to std::string
		const bool operator==(const std::string&) const;
		const bool operator!=(const std::string&) const;
	};

	#define CLASS_NAME_MAX_SIZE_BYTES 80 // Raise this at your memory's peril

	struct gClass
	{
	public:
		static const gClass INVALID_TYPE;

		const int id = 0;
		const char* name = "INVALID_TYPE";
		Actor*(*new_actor)(const graphx::gUID&, const graphx::gSettings&) = nullptr;
		Device*(*new_device)(const graphx::gUID&, const graphx::gSettings&) = nullptr;
		const glm::vec3 debugging_color = glm::vec3(1.0f, 0.0f, 0.0f);

		inline constexpr gClass():
		id(0),
		name("INVALID_TYPE"),
		new_actor(nullptr),
		new_device(nullptr),
		debugging_color(glm::vec3(1.0f, 0.0f, 0.0f))
		{}

		inline constexpr gClass(const char init_name[CLASS_NAME_MAX_SIZE_BYTES], const int init_id, Actor*(*new_actor_function)(const graphx::gUID&, const graphx::gSettings&), const glm::vec3& init_debugging_color):
		id(init_id),
		name(init_name),
		new_actor(new_actor_function),
		new_device(nullptr),
		debugging_color(init_debugging_color)
		{}

		inline constexpr gClass(const char init_name[CLASS_NAME_MAX_SIZE_BYTES], const int init_id, Device*(*new_device_function)(const graphx::gUID&, const graphx::gSettings&), const glm::vec3& init_debugging_color):
		id(init_id),
		name(init_name),
		new_actor(nullptr),
		new_device(new_device_function),
		debugging_color(init_debugging_color)
		{}

		inline constexpr gClass(const gClass& to_copy):
		id(to_copy.id),
		name(to_copy.name),
		new_actor(to_copy.new_actor),
		new_device(to_copy.new_device),
		debugging_color(to_copy.debugging_color)
		{}

		gClass(const std::string&);
		gClass(const int&);

		// Comparing gClass to gClass
		const bool operator==(const gClass&) const;
		const bool operator!=(const gClass&) const;
		const bool operator< (const gClass&) const;
		const bool operator> (const gClass&) const;
		const bool operator<=(const gClass&) const;
		const bool operator>=(const gClass&) const;

		// Comparing gClass to int
		const bool operator==(const int&) const;
		const bool operator!=(const int&) const;
		const bool operator< (const int&) const;
		const bool operator> (const int&) const;
		const bool operator<=(const int&) const;
		const bool operator>=(const int&) const;

		// Comparing gClass to std::string
		const bool operator==(const std::string&) const;
		const bool operator!=(const std::string&) const;

		// Conversions
		// constexpr operator int() const;
		// constexpr operator std::string() const;
	};

	// Comparing gClass to gClass* (left and right sided)
	const bool operator==(const gClass&, const gClass*);
	const bool operator!=(const gClass&, const gClass*);
	const bool operator==(const gClass*, const gClass&);
	const bool operator!=(const gClass*, const gClass&);

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
}
#endif
