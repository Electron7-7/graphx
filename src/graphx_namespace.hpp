#ifndef GRAPHX_NAMESPACE
#define GRAPHX_NAMESPACE
#include "g_common_fwd.hpp"
#include "r_common_fwd.hpp"
#include <glm/vec3.hpp>
#include <any>
#include <string>

#define GRAPHXTHEATRE_EXTENSION std::string(".gt")
#define SAFETY_ID -42 // Just to keep things consistent

namespace graphx
{
	namespace safety
	{   // Example use-case: Theatre::getActor should always return a valid Actor pointer, so the worst case scenario is that it returns &graphx::safety::actor
		extern Actor actor;
		extern Device device;
	}

	namespace current
	{
		extern Theatre theatre; // This guy is defined in `g_theatre.cpp`
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

	namespace debug
	{
		extern bool actor_debug_menu_open;
		extern float actor_debug_menu_text_scale;
	}

	namespace global
	{
		extern glm::vec3 orientation_up;
		extern glm::vec3 orientation_front;
		extern glm::vec3 orientation_right;
	}

	namespace identifiers
	{
		namespace primitive
		{
			inline constexpr int FOO      = -1;
			inline constexpr int LINE     =  0;
			inline constexpr int TRIANGLE =  1;
			inline constexpr int TEXT     = -1; //< Text not supported yet!
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

	typedef std::pair<int, std::any> gSetting; // The `int` in `graphx::gSetting` identifies the type; type identifiers can be found in `t_common.hpp`
	typedef std::unordered_map<std::string, gSetting> gSettings;

	struct gID
	{
	public:
		static const gID EMPTY;
		int uid = -1;
		std::string name = "A Class With No Name";

		gID(const int, const std::string&);
		gID(const std::string&);
		gID(const char*);
		gID(const int);
		gID(const gID&);
		gID();

		const std::string toString() const;

		// Comparing gID to gID
		const bool operator==(const gID&) const;
		const bool operator!=(const gID&) const;
		const bool operator< (const gID&) const;
		const bool operator> (const gID&) const;
		const bool operator<=(const gID&) const;
		const bool operator>=(const gID&) const;

		constexpr operator std::string() { return name; }
	};

	struct gClass
	{
	public:
		static const gClass INVALID_TYPE;

		const int id = 0;
		const char* name = "INVALID_TYPE";
		Actor*(*new_actor)(const graphx::gID&, const graphx::gSettings&) = nullptr;
		Device*(*new_device)(const graphx::gID&, const graphx::gSettings&) = nullptr;
		const glm::vec3 debugging_color = glm::vec3(1.0f, 0.0f, 0.0f);

		static constexpr unsigned int CLASS_NAME_MAX_SIZE_BYTES = 80; // Raise this at your memory's peril
		gClass(const char[CLASS_NAME_MAX_SIZE_BYTES], const int, Actor*(*)(const graphx::gID&, const graphx::gSettings&), Device*(*)(const graphx::gID&, const graphx::gSettings&), const glm::vec3&);
		gClass(const char[CLASS_NAME_MAX_SIZE_BYTES], const int, Actor*(*)(const graphx::gID&, const graphx::gSettings&), const glm::vec3&);
		gClass(const char[CLASS_NAME_MAX_SIZE_BYTES], const int, Device*(*)(const graphx::gID&, const graphx::gSettings&), const glm::vec3&);
		gClass();

		// These constructors are specifically for the interpreter to use
		gClass(const std::string& lookup_by_name);
		gClass(const char* lookup_by_name);
		gClass(const int& lookup_by_id);

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
	private:
		const graphx::gClass& lookupByName(const std::string&);
		const graphx::gClass& lookupById(const int&);
	};

	// Comparing gClass to gClass* (left and right sided)
	const bool operator==(const gClass&, const gClass*);
	const bool operator!=(const gClass&, const gClass*);
	const bool operator==(const gClass*, const gClass&);
	const bool operator!=(const gClass*, const gClass&);

	inline const gClass gClass::INVALID_TYPE = gClass();  // The default constructor is INVALID_TYPE
}
#endif