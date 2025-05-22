#include "graphx_namespace.hpp"
#include "t_interpreter.hpp"
#include "g_theatre.hpp"
#include "g_devices.hpp"
#include "g_actors.hpp"
#include <glm/glm.hpp>

//---------------
// TheatreHandler
//---------------
Theatre* graphx::CurrentTheatreHandler::getCurrentTheatre() const
{ return &theatre_buffer.at(theatre_buffer_index); }

unsigned int graphx::CurrentTheatreHandler::theatre_buffer_index = 0;
std::array<Theatre, 2> graphx::CurrentTheatreHandler::theatre_buffer =
{
    Theatre("Main Theatre Buffer Index: 0"),
    Theatre("Main Theatre Buffer Index: 1")
};

void graphx::CurrentTheatreHandler::swapBuffer()
{
    theatre_buffer_index = 1 - theatre_buffer_index;
    unsigned int old_index = 1 - theatre_buffer_index;
    theatre_buffer.at(old_index).dropCurtains();
    theatre_buffer.at(old_index) = Theatre("Main Theatre Buffer Index: " + std::to_string(old_index));
}

// GraphX
GraphXTheatreInterpreter graphx::Interpreter = GraphXTheatreInterpreter();
graphx::CurrentTheatreHandler graphx::TheatreHandler = graphx::CurrentTheatreHandler();

// Debug
bool graphx::debug::actor_debug_menu_open = false;
float graphx::debug::actor_debug_menu_text_scale = 1.8f;
Material light_debug_material = Material("Light Debug Material", LIGHT_DEBUGGING, true);
Mesh graphx::debug::light_debug_mesh = Mesh(GRAPHX_CUBE, &light_debug_material);

// Error
Material graphx::error::missing_material = Material(true);
Mesh graphx::error::missing_mesh = Mesh(ERROR_MODEL, &graphx::error::missing_material);
Device graphx::error::missing_device = Device("Missing Device");
Actor graphx::error::missing_actor = Actor("Missing Actor");

// Orientation
glm::vec3 graphx::orientation::up(0.0f, 1.0f, 0.0f);
glm::vec3 graphx::orientation::front(0.0f, 0.0f, -1.0f);
glm::vec3 graphx::orientation::right(1.0f, 0.0f, 0.0f);