//#include <stdio.h>
//#include <stdlib.h>	
//#include "Renderer.h"
//#include <string>
//#include <iostream>


#include "Input.h"
#include "Timer.h"
#include "MyGame.h"
#include <sys/ppu_thread.h>
#include <sys/process.h>

SYS_PROCESS_PARAM(1001, 0x10000)

bool done = false;

Camera* camera;

void start_button()		
{
	done = true;
	std::cout << "Pressed start button!" << std::endl;
}

void select_button()		
{
	camera->SetPosition(Vector3(0, 0, 300));
	camera->SetPitch(0.0f);
	camera->SetYaw(0.0f);
	done = true;
}


/*
Here's a quick example program. It'll load up a single SceneNode
scene, containing a textured quad, with a camera controllable by
the joypad. Pretty basic, but as with Graphics for Games, its
everything you need to get started and doing something more
interesting with the Playstation!

*/
void physicsLoop(uint64_t arg)
{
	Timer timer;
	bool running = true;
	GameClass* game = (GameClass*)arg;

	while (running)
	{
		float msec = timer.GetTimedMS();
		game->UpdatePhysics(msec);
	}
}

int main(void)	
{
	std::cout << "All-New intro to GCM V1.0!\n" << std::endl;

	if (!Renderer::Initialise())
	{
		return;
	}

	PhysicsSystem::Initialise();

	//Start off by initialising the Input system
	Input::Initialise();
	Input::SetPadFunction(INPUT_START, start_button);
	Input::SetPadFunction(INPUT_START, select_button);


	MyGame* game = new MyGame();

	bool running = true;
	Timer gameTime;

	sys_ppu_thread_t id;
	uint64_t priority = 1000;
	size_t stack_size = 0x1000;
	char *thread_name = "My Thread";
	sys_ppu_thread_create(&id, physicsLoop, (uint64_t)game, priority, stack_size,SYS_PPU_THREAD_CREATE_JOINABLE, thread_name);

	while (!done)
	{
		Input::UpdateJoypad();	//Receive latest joypad input for all joypads

		float msec = (float)gameTime.GetTimedMS();	//How many milliseconds since last update?
		game->UpdateRendering(msec);	//Update our 'sybsystem' logic (renderer and physics!)
		game->UpdateGame(msec);	//Update our game logic
		//game->UpdatePhysics(msec);
	}


	Input::Destroy();
	delete game;

	return 0;
}