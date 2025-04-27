#include "graphx_namespace.hpp"
#include "g_actors.hpp"
#include "g_device.hpp"
#include "g_theatre.hpp"
#include "t_interpreter.hpp"

// GraphX
GraphXTheatreInterpreter graphx::Interpreter = GraphXTheatreInterpreter();

// Current
Theatre graphx::current::theatre = Theatre("current_theatre Not Yet Set!");
GraphXPlayer* graphx::current::player = &graphx::safety::player;
Environment* graphx::current::environment = &graphx::safety::environment;

// Orientation
glm::vec3 graphx::orientation::up(0.0f, 1.0f, 0.0f);
glm::vec3 graphx::orientation::front(0.0f, 0.0f, -1.0f);
glm::vec3 graphx::orientation::right(1.0f, 0.0f, 0.0f);

// State
bool graphx::state::loading_new_main_theatre = true; // Definitely wanna replace this with something a little more sophisticated.

// Safety
Actor  graphx::safety::actor  = Actor("Safety Actor (if you see this, this is the engine trying its very best not to return a nullptr while also not leaking memory!");
Device graphx::safety::device = Device("Safety Device (if you see this, this is the engine trying its very best not to return a nullptr while also not leaking memory!");
GraphXPlayer graphx::safety::player = GraphXPlayer();
Environment graphx::safety::environment = Environment();

// Debug
bool  graphx::debug::actor_debug_menu_open       = false;
float graphx::debug::actor_debug_menu_text_scale = 1.8f;
