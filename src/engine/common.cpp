#include "common.hpp"
#include "engine/interpreter/interpreter.hpp"
#include "engine/theatre/theatre.hpp"
#include "engine/things/devices/devices.hpp"
#include "engine/things/actors/actors.hpp"
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
Device graphx::missing::device = Device("Missing Device");
Collider graphx::missing::collider = Collider("Missing Collider");
Mesh graphx::missing::mesh = Mesh(ERROR_MODEL, &graphx::missing::material);
Material graphx::missing::material = Material(true);
Actor graphx::missing::actor = Actor("Missing Actor");
LightFlashlight graphx::missing::temporary_backup_flashlight = LightFlashlight("Temporary Backup Flashlight"); // TEMPORARY


// Orientation
glm::vec3 graphx::orientation::up(0.0f, 1.0f, 0.0f);
glm::vec3 graphx::orientation::front(0.0f, 0.0f, -1.0f);
glm::vec3 graphx::orientation::right(1.0f, 0.0f, 0.0f);