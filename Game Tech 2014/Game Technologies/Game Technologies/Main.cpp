/******************************************************************************
Class:
Implements:
Author:Rich Davison	<richard.davison4@newcastle.ac.uk> and YOU!
Description: We have a revised 'main' file this time around. The old main file
was sometimes quite hard to debug when classes failed to initialise, so I 
have made a dedicated 'Quit' function, which will destroy everything, and 
will halt and post a quit message if anything has gone wrong. The Window class
now also brings the console window into focus ini ts destructor, so that we
can immediately see which class has caused initialisation to end. 

You may like to take this further and have each class initialised in the 
main file to have a number of 'error strings' to output in this function in 
order to quickly track down what has gone wrong - it shouldn't be too hard to
implement this, so I'll leave it up to you!

Note that this time the Window is a single class, instantiated and destroyed
via static functions. Noone ever used the ability to have multiple renderers
and windows to good effect, and it made the code more complex, so I have 
removed these abilities in the code download for this module! The less things
that can go wrong, the more time you potentially have to have fun with physics!

-_-_-_-_-_-_-_,------,   
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""   

*//////////////////////////////////////////////////////////////////////////////
#include "NetworkServer.h"
#include "NetworkClient.h"
#include <thread>
#include "../../nclgl/Window.h"
#include "MyGame.h"


#pragma comment(lib, "nclgl.lib")

MyGame* game;

int Quit(bool pause = false, const string &reason = "") 
{
	PhysicsSystem::Destroy();
	Window::Destroy();
	Renderer::Destroy();

	if(pause) 
	{
		std::cout << reason << std::endl;
		system("PAUSE");
	}

	return 0;
}

void physicsLoop(GameClass* game, bool& running) 
{
	GameTimer timer;
	while (running) 
	{
		float msec = timer.GetTimedMS();
		game->UpdatePhysics(msec);
	}
}

int main() 
{
	SoundSystem::Initialise(); //Build SoundSystem

	if(!Window::Initialise("Game Technologies", 1000,800,true)) 
	{
		return Quit(true, "Window failed to initialise!");
	}

	if(!Renderer::Initialise()) 
	{
		return Quit(true, "Renderer failed to initialise!");
	}

	Window::GetWindow().LockMouseToWindow(true);
	Window::GetWindow().ShowOSPointer(false);

	buttonPressed bp;
	bp = NO_PRESSED;

	while (Window::GetWindow().UpdateWindow() && bp == NO_PRESSED)
	{
		bp = Renderer::GetRenderer().RenderMainMenu();
	}

	if (bp == EXIT)
		exit(0);


	Renderer::GetRenderer().fullyInit();
	//Start Loading Screen Draw
	Renderer::GetRenderer().RenderLoading(0, "Hello World");

	PhysicsSystem::Initialise();


	MyGame* game;
	switch (bp)
	{
	case SOLO:
		game = new MyGame(false, false, false, 0);
		break;
	case HOST_2:
		game = new MyGame(true, true, true, 2);
		break;
	case HOST_3:
		game = new MyGame(true, true, true, 3);
		break;
	case HOST_4:
		game = new MyGame(true, true, true, 4);
		break;
	case CLIENT:
		game = new MyGame(false, true, true, 0);
		break;
	case EXIT:
		exit(0);
	}

	//std::thread Loading(loadGame);
//	MyGame* game = new MyGame();




	bool running = true;
	std::thread physics(physicsLoop, game, std::ref(running));

	while(Window::GetWindow().UpdateWindow() && GameClass::GetGameClass().getCurrentState() != GAME_EXIT)
	{
		float msec = Window::GetWindow().GetTimer()->GetTimedMS();	//How many milliseconds since last update?

		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_2)) {
			SoundSystem::GetSoundSystem()->PlaySound(
				SoundManager::GetSound("../../Sounds/56900__syna-max__war.wav"), Vector3());
		}

		game->UpdateRendering(msec);	//Update our 'sybsystem' logic (renderer and physics!)
		game->UpdateGame(msec);	//Update our game logic	
		Window::GetWindow().updateController(msec);
	}

	running = false;

	physics.join();

	SoundManager::DeleteSounds();
	SoundSystem::Destroy();

	delete game;	//Done with our game now...bye bye!
	return Quit();
}

