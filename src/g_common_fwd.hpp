#ifndef GRAPHX_COMMON_FORWARD_DECLARATIONS
#define GRAPHX_COMMON_FORWARD_DECLARATIONS

// Environment Forward Declaration
struct Environment;

// Theatre
struct Theatre;

// Actors
class Actor;
class PhysicsActor;
class RigidBodyActor;
class StaticBodyActor;
class Camera;
class GraphXPlayer;
class Ramiel;
// Light Actors
class Light;
class LightDirectional;
class LightSpot;
class LightFlashlight;
class LightTesterMover;

// Other
struct RenderState;

// Variables
extern bool loading_new_main_theatre;
extern Theatre current_theatre;

// Functions
template<typename T> T iKnowWhatActorIWant(auto identifier);
template<typename T> T iKnowWhatDeviceIWant(auto identifier);
Theatre *getCurrentTheatre(bool print_note);
Environment *getCurrentEnvironment();
GraphXPlayer *getCurrentPlayer();
#endif