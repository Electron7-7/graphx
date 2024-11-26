// g_game - Temporary(?) game logic

class GameHandler
{
public:
	GameHandler(float current_tick = 0.0f);

	void Update();
	void ProcessInput();

private:
	float g_currenttick;
};