#ifndef GRAPHXnameSPACE
#define GRAPHXnameSPACE
#include "g_common_fwd.hpp"
#include "r_common_fwd.hpp"
#include <array>
#include <any>
#include <string>

#define GRAPHXTHEATRE_EXTENSION std::string(".gt")

namespace graphx
{
	struct gClass
	{
	public:
		// Feel free to expand these limits if needed; just remember to update the values in graphx::classes accordingly
		static constexpr int ACTOR_ID_LIMIT  = 999;
		static constexpr int DEVICE_ID_LIMIT = 1999;
	private:
		inline static constexpr int INVALID_TYPE_ID = -481516;
		inline static constexpr char INVALID_TYPE_NAME[13] = "INVALID_TYPE";
		inline static constexpr int NAME_MAX_SIZE_BYTES = 80; // Raise this at your memory's peril
		static const std::array<gClass, (ACTOR_ID_LIMIT + DEVICE_ID_LIMIT)> &classes; // Defined in `graphx_classes_namespace.hpp`
	public:
		static const gClass INVALID_TYPE;      // Defined in `graphx_classes_namespace.hpp`

		int id = INVALID_TYPE_ID;              // `id` is usually more important than `name`... usually...
		const char *name = INVALID_TYPE_NAME;  // `name` is only really used by the interpreter (and for debugging)
		Actor *(*create_new_actor)() = nullptr;
		Device *(*create_new_device)() = nullptr;

		inline gClass() = default;
		inline constexpr gClass(const char init_name[NAME_MAX_SIZE_BYTES], int init_id, Actor*(*new_actor_function)())   : id(init_id), name(init_name), create_new_actor(new_actor_function)   {}
		inline constexpr gClass(const char init_name[NAME_MAX_SIZE_BYTES], int init_id, Device*(*new_device_function)()) : id(init_id), name(init_name), create_new_device(new_device_function) {}
		inline constexpr gClass(const gClass &to_copy) : id(to_copy.id), name(to_copy.name), create_new_actor(to_copy.create_new_actor), create_new_device(to_copy.create_new_device) {}

		inline gClass(std::string init_name)
		{
			for(const gClass &valid_class : classes)
				if(valid_class == init_name)
				{
					*this = valid_class;
					return;
				}
			*this = INVALID_TYPE;
		}

		inline gClass(int init_id)
		{
			for(const gClass &valid_class : classes)
				if(valid_class == init_id)
				{
					*this = valid_class;
					return;
				}
			*this = INVALID_TYPE;
		}

		inline static bool isValidClass(gClass type)
		{
			for(auto valid_class = classes.begin() ; *valid_class != INVALID_TYPE ; valid_class++)
				if(*valid_class == type)
					return true;
			return false;
		}

		inline static const gClass &getClassType(gClass type)
		{
			for(auto valid_class = classes.begin() ; *valid_class != INVALID_TYPE ; valid_class++)
				if(*valid_class == type)
					return *valid_class;
			return INVALID_TYPE;
		}

		// Overloading Comparison Operators
		//---------------------------------
		// 1: Comparing gClass to gClass
		const bool operator==(const gClass &compare_against) const { return (id == compare_against.id);  }
		const bool operator!=(const gClass &compare_against) const { return !(*this == compare_against); }
		const bool operator< (const gClass &compare_against) const { return (id < compare_against.id);   }
		const bool operator> (const gClass &compare_against) const { return (id > compare_against.id);   }
		const bool operator<=(const gClass &compare_against) const { return !(*this > compare_against);  }
		const bool operator>=(const gClass &compare_against) const { return !(*this < compare_against);  }
		// 2: Comparing gClass to int
		const bool operator==(const int &compare_against) const { return (id == compare_against);     }
		const bool operator!=(const int &compare_against) const { return !(*this == compare_against); }
		const bool operator< (const int &compare_against) const { return (id < compare_against);      }
		const bool operator> (const int &compare_against) const { return (id > compare_against);      }
		const bool operator<=(const int &compare_against) const { return !(*this > compare_against);  }
		const bool operator>=(const int &compare_against) const { return !(*this < compare_against);  }
		// 4: Comparing gClass to std::string
		const bool operator==(const std::string &compare_against) const { return (name == compare_against);   }
		const bool operator!=(const std::string &compare_against) const { return !(*this == compare_against); }
		const bool operator< (const std::string &compare_against) const { return (name < compare_against);    }
		const bool operator> (const std::string &compare_against) const { return (name > compare_against);    }
		const bool operator<=(const std::string &compare_against) const { return !(*this > compare_against);  }
		const bool operator>=(const std::string &compare_against) const { return !(*this < compare_against);  }

		// Overloading Conversion Operators
		//---------------------------------
		// 1: Conversion from gClass to int
		constexpr operator int() const { return id; }
		// 2: Conversion from gClass to long
		constexpr operator long() const { return static_cast<long>(id); }
		// 3: Conversion from gClass to std::string
		constexpr operator std::string() const { return std::string(name); }
	};

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

		inline GLShader *SHADER_DEFAULT             = nullptr;
		inline GLShader *SHADER_DEBUG_FULLBRIGHT    = nullptr;
		inline GLShader *SHADER_DEBUG_NORMALS       = nullptr;
		inline GLShader *SHADER_DEBUG_VERTEX_COLORS = nullptr;
		inline GLShader *SHADER_SKYBOX              = nullptr;

		inline constexpr unsigned int VAOS_AMOUNT        = 2;
		//---------------------------------------------------
		inline constexpr unsigned int VAO_DEFAULT        = 0;
		inline constexpr unsigned int VAO_PRIMITIVES     = 1;


		inline bool  do_interpolation = true; // For testing when I change the interpolation method to be more like GZDoom
		inline int   graphx_api = GRAPHX_OPENGL;
		inline GLShader *current_shader = nullptr;
		inline float main_window_width = 1280.0f;
		inline float main_window_height = 720.0f;
		inline float camera_near = 0.1f;
		inline float camera_far = 10000.0f;
		inline bool  jolt_debug_render = false;
		inline bool  lighting_switch_diffuse = true;
		inline bool  lighting_switch_specular = true;
		inline bool  lighting_switch_ambient = true;
	};

	/// The `int` in `graphx::gSetting` identifies the type; type identifiers can be found in `t_common.hpp`.
	typedef std::pair<int, std::any> gSetting;
	typedef std::unordered_map<std::string, gSetting> gSettings;
}
#endif
