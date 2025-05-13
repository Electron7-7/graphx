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

// Functions
Theatre *getCurrentTheatre(bool print_note);
#endif