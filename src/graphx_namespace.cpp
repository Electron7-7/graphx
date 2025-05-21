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
    Theatre("Main Theatre Buffer: Theatre 1"),
    Theatre("Main Theatre Buffer: Theatre 2")
};

// GraphX
GraphXTheatreInterpreter graphx::Interpreter = GraphXTheatreInterpreter();
graphx::CurrentTheatreHandler graphx::TheatreHandler = graphx::CurrentTheatreHandler();

// Debug
bool graphx::debug::actor_debug_menu_open = false;
float graphx::debug::actor_debug_menu_text_scale = 1.8f;