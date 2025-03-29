#ifndef GRAPHX_NAMESPACE
#define GRAPHX_NAMESPACE
#include "g_common_fwd.hpp"
#include "r_common_fwd.hpp"
#include <set>
#include <any>
#include <string>

#define GRAPHXTHEATRE_EXTENSION std::string(".gt")

namespace graphx
{
	struct gClass
	{
	private:
		inline static std::set<gClass> valid_classes; // This could probably be a `std::array`, but I'd rather not think about the overhead...
		static constexpr int INVALID_TYPE_ID = -481516;
		int _id = INVALID_TYPE_ID; // `_id` is usually more important than `_name`... usually...
		const char *_name = "INVALID_TYPE"; // `name` is only used by the interpreter (and for debugging)
		inline void updateValidClasses()
		{
			if(_id == INVALID_TYPE_ID) // This is an invalid class
				return;
			if(valid_classes.contains(*this)) // This is already a valid class
				return;
			valid_classes.insert(*this); // Add this gClass to the list
		}
	public:
		inline gClass() = default;
		inline gClass(std::string init_name, int init_id, Actor*(*new_actor_function)())   : _id(init_id), _name(init_name.c_str()), create_new_actor(new_actor_function)   {}
		inline gClass(std::string init_name, int init_id, Device*(*new_device_function)()) : _id(init_id), _name(init_name.c_str()), create_new_device(new_device_function) {}

		inline gClass(std::string init_name) : _name(const_cast<char *>(init_name.c_str()))
		{
			if(valid_classes.contains(init_name))
			{
				_id = valid_classes.find(init_name)->_id;
				create_new_actor = valid_classes.find(init_name)->create_new_actor;
				create_new_device = valid_classes.find(init_name)->create_new_device;
			}
		}
		inline gClass(int init_id) : _id(init_id)
		{
			if(valid_classes.contains(init_id))
			{
				_name = valid_classes.find(init_id)->_name;
				create_new_actor = valid_classes.find(init_id)->create_new_actor;
				create_new_device = valid_classes.find(init_id)->create_new_device;
			}
		}

		inline const int id() const { return _id; }
		inline const std::string name() const { return const_cast<char *>(_name); }
		inline static bool isValidClass(gClass type)
		{
			if(type != INVALID_TYPE_ID)
				return valid_classes.contains(type);
			return false;
		}
		inline static const gClass &getClassType(gClass type)
		{
			if(valid_classes.contains(type))
				return *valid_classes.find(type);
			else
				return *valid_classes.find(INVALID_TYPE_ID);
		}

		Actor *(*create_new_actor)() = nullptr;
		Device *(*create_new_device)() = nullptr;

		// Overloading Comparison Operators
		//---------------------------------
		// 1: Comparing gClass to gClass
		inline const bool operator==(const gClass &compare_against) const { return (_id == compare_against.id()); }
		inline const bool operator!=(const gClass &compare_against) const { return !(*this == compare_against);       }
		inline const bool operator< (const gClass &compare_against) const { return (_id < compare_against.id());  }
		inline const bool operator> (const gClass &compare_against) const { return (_id > compare_against.id());  }
		inline const bool operator<=(const gClass &compare_against) const { return !(*this > compare_against);        }
		inline const bool operator>=(const gClass &compare_against) const { return !(*this < compare_against);        }
		// 2: Comparing gClass to int
		inline const bool operator==(const int &compare_against) const { return (_id == compare_against);  }
		inline const bool operator!=(const int &compare_against) const { return !(*this == compare_against); }
		inline const bool operator< (const int &compare_against) const { return (_id < compare_against);   }
		inline const bool operator> (const int &compare_against) const { return (_id > compare_against);   }
		inline const bool operator<=(const int &compare_against) const { return !(*this > compare_against);  }
		inline const bool operator>=(const int &compare_against) const { return !(*this < compare_against);  }
		// 4: Comparing gClass to std::string
		inline const bool operator==(const std::string &compare_against) const { return (_name == compare_against);  }
		inline const bool operator!=(const std::string &compare_against) const { return !(*this == compare_against); }
		inline const bool operator< (const std::string &compare_against) const { return (_name < compare_against);   }
		inline const bool operator> (const std::string &compare_against) const { return (_name > compare_against);   }
		inline const bool operator<=(const std::string &compare_against) const { return !(*this > compare_against);  }
		inline const bool operator>=(const std::string &compare_against) const { return !(*this < compare_against);  }

		// Overloading Conversion Operators
		//---------------------------------
		// 1: Conversion from gClass to int
		inline constexpr operator int() const { return _id; }
		// 2: Conversion from gClass to long
		inline constexpr operator long() const { return static_cast<long>(_id); }
		// 3: Conversion from gClass to std::string
		inline constexpr operator std::string() const { return static_cast<std::string>(_name); }
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

	/// The `int` in `graphx::gSetting` identifies the type; type identifiers can be found in `t_common.hpp`.
	typedef std::pair<int, std::any> gSetting;
	typedef std::unordered_map<std::string, gSetting> gSettings;
}
#endif
