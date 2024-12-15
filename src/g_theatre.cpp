#include "g_theatre.hpp"
#include "r_common.hpp" // Remove this once I have a system for loading spaces

Theatre *current_theatre;

void loadNewTheatre(Theatre *new_theatre)
{
	current_theatre = new_theatre;
	time_to_store_buffers = true; // Remove this once I have a system for loading spaces
}