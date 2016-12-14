// ShaderViewer.cpp : Defines the entry point for the console application.
//
#include "Game.h"
#include <windows.h>

Game* game = nullptr;
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

int main(int argc, char* argv[])
{
	game = new Game();
	// Initialize renderer
	int res = game->Init();
	if (res != 0)
	{
		printf("Failed to initialize game! Result %d\n", res);
	}
	else
	{
		GLFWwindow* window = game->GetRenderer()->GetRenderWindow();
		glfwSetMouseButtonCallback(window, mouse_button_callback);

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
				exit = glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS || glfwWindowShouldClose(game->GetRenderer()->GetRenderWindow());
				game->Draw();
			}
			Sleep(1);
		}

		game->Deinit();
	}

	delete game;
	game = nullptr;
	return 0;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		game->OnLbMouseClick(static_cast<float>(xpos), static_cast<float>(ypos));
	}
}

