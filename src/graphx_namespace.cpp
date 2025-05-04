#include "graphx_namespace.hpp"
#include "g_actors.hpp"
#include "g_device.hpp"
#include "g_theatre.hpp"
#include "t_interpreter.hpp"
#include "g_device.hpp"
#include "g_actor.hpp"

// GraphX
GraphXTheatreInterpreter graphx::Interpreter = GraphXTheatreInterpreter();

// Current
Theatre graphx::current::theatre = Theatre("current_theatre Not Yet Set!");
std::shared_ptr<GraphXPlayer> graphx::current::player()
{
	if(std::shared_ptr<Actor> player = graphx::current::theatre.getActor(graphx::current::uids::player))
		return std::dynamic_pointer_cast<GraphXPlayer>(player);

	return std::make_shared<GraphXPlayer>("Empty Player");
}
std::shared_ptr<Environment> graphx::current::environment()
{
	if(std::shared_ptr<Device> environment = graphx::current::theatre.getDevice(graphx::current::uids::environment))
		return std::dynamic_pointer_cast<Environment>(environment);

	return std::make_shared<Environment>("Empty Environment");
}

int graphx::current::uids::player = -1;
int graphx::current::uids::environment = -1;

// Orientation
glm::vec3 graphx::orientation::up(0.0f, 1.0f, 0.0f);
glm::vec3 graphx::orientation::front(0.0f, 0.0f, -1.0f);
glm::vec3 graphx::orientation::right(1.0f, 0.0f, 0.0f);

// State
bool graphx::state::loading_new_main_theatre = true; // Definitely wanna replace this with something a little more sophisticated.

// Debug
bool  graphx::debug::actor_debug_menu_open       = false;
float graphx::debug::actor_debug_menu_text_scale = 1.8f;
