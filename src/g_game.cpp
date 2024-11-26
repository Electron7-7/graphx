// g_game - Temporary(?) game logic
#include <iostream>
#include "g_game.hpp"

GameHandler::GameHandler(float current_tick)
{
	g_currenttick = current_tick;
}

void GameHandler::Update()
{
	std::cout << "Game Tick: " << g_currenttick << std::endl;
}