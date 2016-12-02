// ShaderViewer.cpp : Defines the entry point for the console application.
//
#include "Renderer.h"
#include <windows.h>

int main(int argc, char* argv[])
{
	Renderer *renderer = new Renderer();
	// Initialize renderer
	if (renderer->Initialize() == -1)
	{
		printf("Failed to initialize renderer!\n");
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
				renderer->OnDraw();
				glfwPollEvents();
				acc -= MS_PER_FRAME;
				exit = glfwGetKey(renderer->GetRenderWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS || glfwWindowShouldClose(renderer->GetRenderWindow());
			}
			Sleep(1);
		}

		renderer->Deinit();
	}

	delete renderer;
	return 0;
}

