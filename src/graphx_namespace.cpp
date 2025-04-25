#include "graphx_namespace.hpp"
#include "graphx_classes_namespace.hpp"

//--------------------------------------
// Definitions for: graphx_namespace.hpp
//--------------------------------------
namespace graphx
{
    bool debug::actor_debug_menu_open = false;
    float debug::actor_debug_menu_text_scale = 1.8f;
    const gUID gUID::INVALID_UID = gUID(); // INVALID_UID is the default constructor

    //-----
    // gUID
    //-----
    gUID::gUID(const int new_id, const std::string& new_name)
    : id(new_id), name(new_name)
    {}

    gUID::gUID(const gUID& copy_me)
    : gUID(copy_me.id, copy_me.name)
    {}

    gUID::gUID()
    : gUID(-1, "A Class With No Name")
    {}

    // Comparing gUID to gUID
    const bool gUID::operator==(const gUID& compare_against) const { return (id == compare_against.id);  }
    const bool gUID::operator!=(const gUID& compare_against) const { return !(*this == compare_against); }
    const bool gUID::operator< (const gUID& compare_against) const { return (id < compare_against.id);   }
    const bool gUID::operator> (const gUID& compare_against) const { return (id > compare_against.id);   }
    const bool gUID::operator<=(const gUID& compare_against) const { return !(*this > compare_against);  }
    const bool gUID::operator>=(const gUID& compare_against) const { return !(*this < compare_against);  }
    // Comparing gUID to int
    const bool gUID::operator==(const int&  compare_against) const { return (id == compare_against);     }
    const bool gUID::operator!=(const int&  compare_against) const { return !(*this == compare_against); }
    const bool gUID::operator< (const int&  compare_against) const { return (id < compare_against);      }
    const bool gUID::operator> (const int&  compare_against) const { return (id > compare_against);      }
    const bool gUID::operator<=(const int&  compare_against) const { return !(*this > compare_against);  }
    const bool gUID::operator>=(const int&  compare_against) const { return !(*this < compare_against);  }
    // Comparing gUID to std::string
    const bool gUID::operator==(const std::string& compare_against) const { return (name == compare_against);   }
    const bool gUID::operator!=(const std::string& compare_against) const { return !(*this == compare_against); }

    const std::string gUID::toString() const
    {
        return std::string("{" + std::to_string(id) + ", " + name + "}");
    }

    const graphx::gClass& gClassFromString(const std::string& to_copy)
    {
        for(auto& valid_class : classes::valid_classes)
            if(!to_copy.compare(valid_class->name))
                return *valid_class;
        return graphx::gClass::INVALID_TYPE;
    }

    const graphx::gClass& gClassFromInt(const int& to_copy)
    {
        for(auto& valid_class : classes::valid_classes)
            if(valid_class->id == to_copy)
                return *valid_class;
        return graphx::gClass::INVALID_TYPE;
    }

    //-------
    // gClass
    //-------
    gClass::gClass(const std::string& to_copy):
    id(gClassFromString(to_copy).id),
    name(to_copy.c_str()),
    new_actor(gClassFromString(to_copy).new_actor),
    new_device(gClassFromString(to_copy).new_device),
    debugging_color(gClassFromString(to_copy).debugging_color)
    {}

    gClass::gClass(const int& to_copy):
    id(to_copy),
    name(gClassFromInt(to_copy).name),
    new_actor(gClassFromInt(to_copy).new_actor),
    new_device(gClassFromInt(to_copy).new_device),
    debugging_color(gClassFromInt(to_copy).debugging_color)
    {}

    // The rest of the constructors are all constexpr, so they have to be defined in the header file
    //-------
    // Comparing gClass to gClass
    const bool gClass::operator==(const gClass& compare_against) const { return (id == compare_against.id);  }
    const bool gClass::operator!=(const gClass& compare_against) const { return !(*this == compare_against); }
    const bool gClass::operator< (const gClass& compare_against) const { return (id < compare_against.id);   }
    const bool gClass::operator> (const gClass& compare_against) const { return (id > compare_against.id);   }
    const bool gClass::operator<=(const gClass& compare_against) const { return !(*this > compare_against);  }
    const bool gClass::operator>=(const gClass& compare_against) const { return !(*this < compare_against);  }
    // Comparing gClass to gClass* (left and right sided)
    const bool operator==(const gClass& object,  const gClass* pointer) { return (pointer && object == *pointer); }
    const bool operator!=(const gClass& object,  const gClass* pointer) { return (pointer && object != *pointer); }
    const bool operator==(const gClass* pointer, const gClass& object)  { return (object == *pointer);            }
    const bool operator!=(const gClass* pointer, const gClass& object)  { return (object != *pointer);            }
    // Comparing gClass to int
    const bool gClass::operator==(const int&    compare_against) const { return (id == compare_against);     }
    const bool gClass::operator!=(const int&    compare_against) const { return !(*this == compare_against); }
    const bool gClass::operator< (const int&    compare_against) const { return (id < compare_against);      }
    const bool gClass::operator> (const int&    compare_against) const { return (id > compare_against);      }
    const bool gClass::operator<=(const int&    compare_against) const { return !(*this > compare_against);  }
    const bool gClass::operator>=(const int&    compare_against) const { return !(*this < compare_against);  }
    // Comparing gClass to std::string
    const bool gClass::operator==(const std::string& compare_against) const { return (name == compare_against); }
    const bool gClass::operator!=(const std::string& compare_against) const { return !(*this == compare_against); }

    // Conversions
    // constexpr gClass::operator int() const { return id; }
    // constexpr gClass::operator std::string() const { return std::string(name); }
}

//----------------------------------------------
// Definitions for: graphx_classes_namespace.hpp
//----------------------------------------------
#define AL ACTOR_LIMIT // This helps make the list below look a lot nicer
namespace graphx
{
    namespace classes
    {
//---------------------------------------------------------------------------------------------------------------------------------------
//                       variable         name                    ID    class constructor function         debugging color (optional)
//---------------------------------------------------------------------------------------------------------------------------------------
        constexpr gClass ACTOR            ( "Actor",              1,    &createNewActor<Actor>,            glm::vec3(0.4f, 0.6f, 0.8f) );
        constexpr gClass PHYSICSACTOR     ( "PhysicsActor",       2,    &createNewActor<PhysicsActor>,     glm::vec3(1.0f, 0.2f, 0.1f) );
        constexpr gClass STATICBODYACTOR  ( "StaticBodyActor",    3,    &createNewActor<StaticBodyActor>,  glm::vec3(1.0f, 0.6f, 0.4f) );
        constexpr gClass RIGIDBODYACTOR   ( "RigidBodyActor",     4,    &createNewActor<RigidBodyActor>,   glm::vec3(1.0f, 0.6f, 0.1f) );
        constexpr gClass CAMERA           ( "Camera",             5,    &createNewActor<Camera>,           glm::vec3(0.0f, 0.0f, 0.0f) );
        constexpr gClass GRAPHXPLAYER     ( "GraphXPlayer",       6,    &createNewActor<GraphXPlayer>,     glm::vec3(0.0f, 0.0f, 0.0f) );
        constexpr gClass RAMIEL           ( "Ramiel",             7,    &createNewActor<Ramiel>,           glm::vec3(0.3f, 0.1f, 1.0f) );
        constexpr gClass LABEL            ( "Label",              8,    &createNewActor<Label>,            glm::vec3(0.0f, 0.0f, 0.0f) );

        // ALL LIGHT DERIVED CLASSES MUST USE NEGATIVE TYPE IDS IN ORDER FOR graphx::classes::isLight TO WORK
        constexpr gClass LIGHT            ( "Light",             -1,    &createNewActor<Light>,            glm::vec3(1.0f, 1.0f, 1.0f) );
        constexpr gClass LIGHTDIRECTIONAL ( "LightDirectional",  -2,    &createNewActor<LightDirectional>, glm::vec3(1.0f, 1.0f, 1.0f) );
        constexpr gClass LIGHTSPOT        ( "LightSpot",         -3,    &createNewActor<LightSpot>,        glm::vec3(0.5f, 1.0f, 0.5f) );
        constexpr gClass LIGHTFLASHLIGHT  ( "LightFlashlight",   -4,    &createNewActor<LightFlashlight>,  glm::vec3(1.0f, 1.0f, 1.0f) );
        constexpr gClass LIGHTTESTERMOVER ( "LightTesterMover",  -5,    &createNewActor<LightTesterMover>, glm::vec3(1.0f, 1.0f, 1.0f) );

        constexpr gClass DEVICE           ( "Device",      AL+    1,    &createNewDevice<Device>,          glm::vec3(0.0f, 0.0f, 0.0f) );
        constexpr gClass ENVIRONMENT      ( "Environment", AL+    2,    &createNewDevice<Environment>,     glm::vec3(0.0f, 0.0f, 0.0f) );
        constexpr gClass MATERIAL         ( "Material",    AL+    3,    &createNewDevice<Material>,        glm::vec3(0.0f, 0.0f, 0.0f) );
        constexpr gClass MESH             ( "Mesh",        AL+    4,    &createNewDevice<Mesh>,            glm::vec3(0.0f, 0.0f, 0.0f) );
        constexpr gClass SPRITE           ( "Sprite",      AL+    5,    &createNewDevice<Sprite>,          glm::vec3(0.0f, 0.0f, 0.0f) );
        constexpr gClass COLLIDER         ( "Collider",    AL+    6,    &createNewDevice<Collider>,        glm::vec3(0.0f, 0.0f, 0.0f) );
        constexpr gClass TEXTURE          ( "Texture",     AL+    7,    &createNewDevice<Texture>,         glm::vec3(0.0f, 0.0f, 0.0f) );

        // Don't forget to add your gClass variable to the `valid_classes` array!
        constexpr std::array<const gClass*, (ACTOR_LIMIT + DEVICE_LIMIT)> valid_classes =
        {
            // Actors
            &ACTOR,
            &PHYSICSACTOR,
            &STATICBODYACTOR,
            &RIGIDBODYACTOR,
            &CAMERA,
            &GRAPHXPLAYER,
            &RAMIEL,
            &LABEL,
            &LIGHT,
            &LIGHTDIRECTIONAL,
            &LIGHTSPOT,
            &LIGHTFLASHLIGHT,
            &LIGHTTESTERMOVER,
            // Devices
            &DEVICE,
            &ENVIRONMENT,
            &MATERIAL,
            &MESH,
            &SPRITE,
            &COLLIDER,
            &TEXTURE,
        };

        //-----------------
        // Helper Functions
        //-----------------

        const bool isValid(const gClass& type) noexcept
        {
            for(auto valid_class = valid_classes.begin() ; *valid_class != gClass::INVALID_TYPE ; valid_class++)
                if(*valid_class == type)
                    return true;
            return false;
        }

        const gClass& getClassType(const gClass& type) noexcept
        {
            for(auto valid_class = valid_classes.begin() ; *valid_class != gClass::INVALID_TYPE ; valid_class++)
                if(*valid_class == type)
                    return **valid_class;
            return gClass::INVALID_TYPE;
        }

        const gClass& getBaseType(const gClass& type) noexcept
        {
            if(gClass::INVALID_TYPE == type)
                return gClass::INVALID_TYPE;

            const int type_id = abs(type.id); // Because Lights use negative values

            if(type_id <= ACTOR_LIMIT && ACTOR <= type_id)
                return ACTOR;

            if(type_id <= DEVICE_LIMIT && DEVICE <= type_id)
                return DEVICE;

            return gClass::INVALID_TYPE;
        }
    }
}