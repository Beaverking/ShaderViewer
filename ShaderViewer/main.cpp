// ShaderViewer.cpp : Defines the entry point for the console application.
//
#include "Game.h"
#include <windows.h>

int main(int argc, char* argv[])
{
	Game *game = new Game();
	// Initialize renderer
	int res = game->Init();
	if (res != 0)
	{
		printf("Failed to initialize game! Result %d\n", res);
	}
	else
	{
		//start main loop
		unsigned long long oldTime = GetCurrentMS();
		unsigned int acc = 0;
		bool exit = false;
		while (!exit)
		{
			unsigned long long elapsed = GetCurrentMS() - oldTime;
			oldTime = GetCurrentMS();
			acc += elapsed;
			if (acc > MS_PER_FRAME)
			{
				//printf("time %llu\n", oldTime);
				game->Update(MS_PER_FRAME);
				glfwPollEvents();
				acc -= MS_PER_FRAME;
				exit = glfwGetKey(game->GetRenderer()->GetRenderWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS || glfwWindowShouldClose(game->GetRenderer()->GetRenderWindow());
				game->Draw();
			}
			Sleep(1);
		}

		game->Deinit();
	}

	delete game;
	return 0;
}

