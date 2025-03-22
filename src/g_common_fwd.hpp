#ifndef GRAPHX_COMMON_FORWARD_DECLARATIONS
#define GRAPHX_COMMON_FORWARD_DECLARATIONS
#include <map>

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
struct LightData;
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
extern std::map<int, Actor*(*)()> actor_map;

// Functions
template<typename T> T iKnowWhatActorIWant(auto identifier);
template<typename T> T iKnowWhatDeviceIWant(auto identifier);
template<typename T> Actor *createNewActor();
Theatre *getCurrentTheatre(bool print_note);
Environment *getCurrentEnvironment();
GraphXPlayer *getCurrentPlayer();
#endif