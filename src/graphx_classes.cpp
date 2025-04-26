#define AL ACTOR_LIMIT
namespace classes
{
//-------------------------------------------------------------------------------------------------------------------------------
//               variable         name                    ID    class constructor function         debugging color (optional)
//-------------------------------------------------------------------------------------------------------------------------------
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
}