#ifndef GRAPHX_NAMESPACE
#define GRAPHX_NAMESPACE
#include "g_common_fwd.hpp"
#include "r_common_fwd.hpp"
#include <map>
#include <any>
#include <string>

#define GRAPHXTHEATRE_EXTENSION std::string(".gt")

namespace graphx
{
	struct gClass
	{
	private:
		static constexpr int INVALID_TYPE_ID = -481516;
		int _id = INVALID_TYPE_ID; // `_id` is usually more important than `_name`... usually...
		static constexpr unsigned int NAME_MAX_SIZE_IN_BYTES = 80; // Feel free to change this... at your memory's peril
		char _name[NAME_MAX_SIZE_IN_BYTES] = "INVALID_TYPE"; // `name` is only used by the interpreter (and for debugging)
		static std::vector<gClass> valid_classes; // This could probably be a `std::array`, but I'd rather not think about the overhead...
		inline void updateValidClasses()
		{
			if(_id == INVALID_TYPE_ID) // This is an invalid class
				return;
			for(gClass &valid_class : valid_classes)
				if(valid_class == *this) // This is already a valid class
					return;
			valid_classes.insert(valid_classes.end(), *this); // Add this gClass to the list
		}
	public:
		inline gClass() = default;
		inline gClass(const char *init_name, int init_type, Actor*(*new_actor_function)())      : _id(init_type), _name(*init_name)        , create_new_actor(new_actor_function)   {}
		inline gClass(std::string init_name, int init_type, Actor*(*new_actor_function)())      : _id(init_type), _name(*init_name.c_str()), create_new_actor(new_actor_function)   {}
		inline gClass(const char *init_name, int init_type, Device*(*new_device_function)())    : _id(init_type), _name(*init_name)        , create_new_device(new_device_function) {}
		inline gClass(std::string init_name, int init_type, Device*(*new_device_function)())    : _id(init_type), _name(*init_name.c_str()), create_new_device(new_device_function) {}

		inline gClass(std::string init_name) : _name(*init_name.c_str())
		{
			for(gClass &valid_class : valid_classes)
				if(valid_class == _name)
					_id = valid_class.id();
		};

		inline const int id() const { return _id; }
		inline const std::string name() const { return const_cast<char *>(_name); }
		inline static bool isValidClass(gClass type)
		{
			if(type != INVALID_TYPE_ID)
				for(gClass &valid_class : valid_classes)
					if(valid_class == type)
						return true;
			return false;
		}
		inline static gClass &getClassType(gClass type)
		{
			for(gClass &valid_class : valid_classes)
				if(valid_class == type)
					return valid_class;
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

	namespace classes
	{
		template<typename T> Actor *createNewActor();   // FORWARD DECLARATION
		template<typename T> Device *createNewDevice(); // FORWARD DECLARATION

		inline const gClass INVALID_TYPE; // The default constructor for `gClass` is `INVALID_TYPE`

		inline const gClass ACTOR            ( "Actor",               1, &createNewActor<Actor>            );
		inline const gClass PHYSICSACTOR     ( "PhysicsActor",        2, &createNewActor<PhysicsActor>     );
		inline const gClass STATICBODYACTOR  ( "StaticBodyActor",     3, &createNewActor<RigidBodyActor>   );
		inline const gClass RIGIDBODYACTOR   ( "RigidBodyActor",      4, &createNewActor<StaticBodyActor>  );
		inline const gClass CAMERA           ( "Camera",              5, &createNewActor<Camera>           );
		inline const gClass GRAPHXPLAYER     ( "GraphXPlayer",        6, &createNewActor<GraphXPlayer>     );
		inline const gClass RAMIEL           ( "Ramiel",              7, &createNewActor<Light>            );

		// ALL LIGHT DERIVED CLASSES MUST USE NEGATIVE TYPE IDS IN ORDER FOR graphx::classes::isLight TO WORK
		inline const gClass LIGHT            ( "Light",              -1, &createNewActor<LightDirectional> );
		inline const gClass LIGHTDIRECTIONAL ( "LightDirectional",   -2, &createNewActor<LightSpot>        );
		inline const gClass LIGHTSPOT        ( "LightSpot",          -3, &createNewActor<LightFlashlight>  );
		inline const gClass LIGHTFLASHLIGHT  ( "LightFlashlight",    -4, &createNewActor<LightTesterMover> );
		inline const gClass LIGHTTESTERMOVER ( "LightTesterMover",   -5, &createNewActor<Ramiel>           );

		inline const gClass DEVICE           ( "Device",           1000, &createNewDevice<Device>          );
		inline const gClass ENVIRONMENT      ( "Environment",      1001, &createNewDevice<Environment>     );
		inline const gClass MATERIAL         ( "Material",         1002, &createNewDevice<Material>        );
		inline const gClass MESH             ( "Mesh",             1003, &createNewDevice<Mesh>            );
		inline const gClass SPRITE           ( "Sprite",           1004, &createNewDevice<Sprite>          );
		inline const gClass COLLIDER         ( "Collider",         1005, &createNewDevice<Collider>        );
		inline const gClass TEXTURE          ( "Texture",          1006, &createNewDevice<Texture>         );

		// Feel free to expand these limits if needed; just remember to update the above values accordingly
		inline const int ACTOR_ID_LIMIT    = 999;
		inline const int DEVICE_ID_LIMIT   = 1999;

		inline std::map<gClass, Actor*(*)()> actor_map =
		{
			{ ACTOR,            &createNewActor<Actor>            },
			{ PHYSICSACTOR,     &createNewActor<PhysicsActor>     },
			{ RIGIDBODYACTOR,   &createNewActor<RigidBodyActor>   },
			{ STATICBODYACTOR,  &createNewActor<StaticBodyActor>  },
			{ CAMERA,           &createNewActor<Camera>           },
			{ GRAPHXPLAYER,     &createNewActor<GraphXPlayer>     },
			{ LIGHT,            &createNewActor<Light>            },
			{ LIGHTDIRECTIONAL, &createNewActor<LightDirectional> },
			{ LIGHTSPOT,        &createNewActor<LightSpot>        },
			{ LIGHTFLASHLIGHT,  &createNewActor<LightFlashlight>  },
			{ LIGHTTESTERMOVER, &createNewActor<LightTesterMover> },
			{ RAMIEL,           &createNewActor<Ramiel>           },
		};

		inline std::map<gClass, Device*(*)()> device_map =
		{
			{ ENVIRONMENT,      &createNewDevice<Environment> },
			{ MATERIAL,         &createNewDevice<Material>    },
			{ MESH,             &createNewDevice<Mesh>        },
			{ SPRITE,           &createNewDevice<Sprite>      },
			{ COLLIDER,         &createNewDevice<Collider>    },
		};

		inline const gClass getBaseType(gClass type) noexcept
		{
			if(type <= ACTOR_ID_LIMIT && type >= ACTOR)
				return ACTOR;

			if(type <= DEVICE_ID_LIMIT && type >= DEVICE)
				return DEVICE;

			return INVALID_TYPE;
		}

		inline const bool isLight(gClass type) noexcept
		{
			if(type == INVALID_TYPE || getBaseType(type) != ACTOR)
				return false;

			if(type > 0)
				return false;

			return true;
		}
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
