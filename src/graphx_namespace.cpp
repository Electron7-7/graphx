#include "graphx_namespace.hpp"
#include "t_interpreter.hpp"
#include "g_theatre.hpp"

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