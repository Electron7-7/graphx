#include "graphx_classes.hpp"
#include "g_actors.hpp"
#include "g_devices.hpp"

#define AL ACTOR_LIMIT // This helps make the list below look a lot nicer
namespace graphx
{
    namespace classes
    {
//-----------------------------------------------------------------------------------------------------------------------------------
//                   variable         name                    ID    class constructor function         debugging color (optional)
//-----------------------------------------------------------------------------------------------------------------------------------
        const gClass ACTOR            ( "Actor",              1,    &createNewActor<Actor>,            glm::vec3(0.4f, 0.6f, 0.8f) );
        const gClass PHYSICSACTOR     ( "PhysicsActor",       2,    &createNewActor<PhysicsActor>,     glm::vec3(1.0f, 0.2f, 0.1f) );
        const gClass STATICBODYACTOR  ( "StaticBodyActor",    3,    &createNewActor<StaticBodyActor>,  glm::vec3(1.0f, 0.6f, 0.4f) );
        const gClass RIGIDBODYACTOR   ( "RigidBodyActor",     4,    &createNewActor<RigidBodyActor>,   glm::vec3(1.0f, 0.6f, 0.1f) );
        const gClass CAMERA           ( "Camera",             5,    &createNewActor<Camera>,           glm::vec3(0.0f, 0.0f, 0.0f) );
        const gClass GRAPHXPLAYER     ( "GraphXPlayer",       6,    &createNewActor<GraphXPlayer>,     glm::vec3(0.0f, 0.0f, 0.0f) );
        const gClass RAMIEL           ( "Ramiel",             7,    &createNewActor<Ramiel>,           glm::vec3(0.3f, 0.1f, 1.0f) );
        const gClass LABEL            ( "Label",              8,    &createNewActor<Label>,            glm::vec3(0.0f, 0.0f, 0.0f) );

        // ALL LIGHT DERIVED CLASSES MUST USE NEGATIVE TYPE IDS IN ORDER FOR graphx::classes::isLight TO WORK
        const gClass LIGHT            ( "Light",             -1,    &createNewActor<Light>,            glm::vec3(1.0f, 1.0f, 1.0f) );
        const gClass LIGHTDIRECTIONAL ( "LightDirectional",  -2,    &createNewActor<LightDirectional>, glm::vec3(1.0f, 1.0f, 1.0f) );
        const gClass LIGHTSPOT        ( "LightSpot",         -3,    &createNewActor<LightSpot>,        glm::vec3(0.5f, 1.0f, 0.5f) );
        const gClass LIGHTFLASHLIGHT  ( "LightFlashlight",   -4,    &createNewActor<LightFlashlight>,  glm::vec3(1.0f, 1.0f, 1.0f) );
        const gClass LIGHTTESTERMOVER ( "LightTesterMover",  -5,    &createNewActor<LightTesterMover>, glm::vec3(1.0f, 1.0f, 1.0f) );

        const gClass DEVICE           ( "Device",      AL+    1,    &createNewDevice<Device>,          glm::vec3(0.0f, 0.0f, 0.0f) );
        const gClass ENVIRONMENT      ( "Environment", AL+    2,    &createNewDevice<Environment>,     glm::vec3(0.0f, 0.0f, 0.0f) );
        const gClass MATERIAL         ( "Material",    AL+    3,    &createNewDevice<Material>,        glm::vec3(0.0f, 0.0f, 0.0f) );
        const gClass MODEL            ( "Model",       AL+    4,    &createNewDevice<Model>,           glm::vec3(0.0f, 0.0f, 0.0f) );
        const gClass SPRITE           ( "Sprite",      AL+    5,    &createNewDevice<Sprite>,          glm::vec3(0.0f, 0.0f, 0.0f) );
        const gClass COLLIDER         ( "Collider",    AL+    6,    &createNewDevice<Collider>,        glm::vec3(0.0f, 0.0f, 0.0f) );
        const gClass TEXTURE          ( "Texture",     AL+    7,    &createNewDevice<Texture>,         glm::vec3(0.0f, 0.0f, 0.0f) );

        // Don't forget to add your gClass variable to the `valid_classes` array!
        const std::array<const gClass*, (ACTOR_LIMIT + DEVICE_LIMIT)> valid_classes =
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