#ifndef GRAPHX_INTERPRETER_LOOKUPS
#include "g_actors.hpp"
#include "g_devices.hpp"
// #include "g_theatre.hpp"
#define GRAPHX_INTERPRETER_LOOKUPS
/*
	This is where all the variable names and lookups for GraphXTheatre files (and the Interpreter)
	are declared and defined. Everything you need, all in one header file (that should only ever
	be included by "t_interpreter.cpp" and NOTHING else). When you want a new Actor, Device, or
	C++ variable to be referencable in a GraphXTheatre file, this is where you make it happen!
*/

template<typename T> std::shared_ptr<Actor>  createNewActor  (Theatre* parent_theatre, const int new_uid, const gSettings& new_settings) { return std::make_shared<Actor>(parent_theatre, new_uid, new_settings);  }
template<typename T> std::shared_ptr<Device> createNewDevice (Theatre* parent_theatre, const int new_uid, const gSettings& new_settings) { return std::make_shared<Device>(parent_theatre, new_uid, new_settings); }

inline const std::map<std::string, std::shared_ptr<Actor>(*)(Theatre*, const int, const gSettings&)>
valid_actors =
{
	{ "Actor",            &createNewActor<Actor>            },
	{ "Camera",           &createNewActor<Camera>           },
	{ "GraphXPlayer",     &createNewActor<GraphXPlayer>     },
	{ "Ramiel",           &createNewActor<Ramiel>           },
	{ "Label",            &createNewActor<Label>            },
	{ "Light",            &createNewActor<Light>            },
	{ "LightDirectional", &createNewActor<LightDirectional> },
	{ "LightSpot",        &createNewActor<LightSpot>        },
	{ "LightFlashlight",  &createNewActor<LightFlashlight>  },
	{ "LightTesterMover", &createNewActor<LightTesterMover> },
};

inline const std::map<std::string, std::shared_ptr<Device>(*)(Theatre*, const int, const gSettings&)>
valid_devices =
{
	{ "Device",           &createNewDevice<Device>      },
	{ "Collider",         &createNewDevice<Collider>    },
	{ "Environment",      &createNewDevice<Environment> },
	{ "Texture",          &createNewDevice<Texture>     },
	{ "Material",         &createNewDevice<Material>    },
	{ "Model",            &createNewDevice<Model>       },
	{ "Sprite",           &createNewDevice<Sprite>      },
};

inline std::map<std::string, std::any>
cpp_definitions =
{
    { "DOOM_TEXTURE_DIFF",     COMP04_5                       },
    { "DOOM_TEXTURE_SPEC",     COMP04_5_SPECULAR              },
    { "MISSING_TEXTURE_DIFF",  MISSING_TEXTURE                },
    { "NO_TEXTURE",            NO_TEXTURE                     },
    { "FLAT_SPEC",             FLAT_SPEC                      },
    { "SOURCE_ORANGE",         SOURCE_ORANGE                  },
    { "SOURCE_LIGHT_GREY",     SOURCE_LIGHT_GREY              },
    { "LIGHT_DEBUGGING",       LIGHT_DEBUGGING                },
    { "DEBUG",                 debug_checkers                 },
    { "GRAPHX_CUBE",           GRAPHX_CUBE                    },
    { "GRAPHX_PYRAMID",        GRAPHX_PYRAMID                 },
    { "GRAPHX_QUAD",           GRAPHX_QUAD                    },
    { "OBJ_ERROR",             ERROR_MODEL                    },
    { "OBJ_SUZANNE",           suzanne_MODEL                  },
    { "Ramiel",                ramiel_MODEL                   },
    { "notapenis",             purely_for_testing_MODEL       },
    { "Dynamic",               JPH::EMotionType::Dynamic      },
    { "Static",                JPH::EMotionType::Static       },
    { "Kinematic",             JPH::EMotionType::Kinematic    },
    { "Moving",                Layers::MOVING                 },
    { "NonMoving",             Layers::NON_MOVING             },
    { "Activate",              JPH::EActivation::Activate     },
    { "DontActivate",          JPH::EActivation::DontActivate },
    { "BoxShape",              graphx::jolt::shapes::BOX      },
    { "SphereShape",           graphx::jolt::shapes::SPHERE   },
    { "CapsuleShape",          graphx::jolt::shapes::CAPSULE  },
    { "CylinderShape",         graphx::jolt::shapes::CYLINDER },
};

// Some helper functions to keep code tidy-ish, lol
namespace gClasses
{
	inline const bool isDefined(const std::string& name) { return (valid_actors.contains(name) || valid_devices.contains(name)); }
	inline const bool isActor(const std::string& name) { return valid_actors.contains(name); }
	inline const bool isDevice(const std::string& name) { return valid_devices.contains(name); }
}
#endif