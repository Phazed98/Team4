
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
	if(!Window::Initialise("Eternal Elements ", 1920,1080, true )) 
	{
		return Quit(true, "Window failed to initialise!");
	}

	if(!Renderer::Initialise()) 
	{
		return Quit(true, "Renderer failed to initialise!");
	}
	Renderer::GetRenderer().RenderLoading(0, "Initializing Sound");
	SoundSystem::Initialise(); //Build SoundSystem
	Renderer::GetRenderer().RenderLoading(1, "Displaying Main Menu");

	Window::GetWindow().LockMouseToWindow(true);
	Window::GetWindow().ShowOSPointer(false);

	buttonPressed bp;
	bp = NO_PRESSED;

	GameTimer menuTimer;

	while (bp == NO_PRESSED)
	{
		Window::GetWindow().UpdateWindow();
		float msec = menuTimer.GetTimedMS();
		bp = Renderer::GetRenderer().RenderMainMenu();
		Window::GetWindow().updateController(msec);
		if (Window::GetinputScrollCDing)
		{
			Window::updateInputScrollLock(msec);
		}
		SoundSystem::GetSoundSystem()->Update(msec);
	}



	if (bp == EXIT)
	{
		//Window::Destroy();
		Renderer::Destroy();
		return 0;
	}


	Renderer::GetRenderer().fullyInit();

	PhysicsSystem::Initialise();

	Renderer::GetRenderer().RenderLoading(20, "Physics System Initialized");
	SoundSystem::GetSoundSystem()->GameStart();

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

	Renderer::GetRenderer().RenderLoading(96, "Starting Physics Loop");
	Renderer::GetRenderer().RenderLoading(98, "Playing Game? ...");
	bool running = true;
	std::thread physics(physicsLoop, game, std::ref(running));

	while(Window::GetWindow().UpdateWindow() && GameClass::GetGameClass().getCurrentState() != GAME_EXIT)
	{
		float msec = Window::GetWindow().GetTimer()->GetTimedMS();	//How many milliseconds since last update?

		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_0)){
			SoundSystem::GetSoundSystem()->SetMasterVolume(0);
		}
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_1)){
			SoundSystem::GetSoundSystem()->SetMasterVolume(1);
		}

		game->UpdateRendering(msec);	//Update our 'sybsystem' logic (renderer and physics!)
		game->UpdateGame(msec);	//Update our game logic	
		Window::GetWindow().updateController(msec);
		if (Window::GetinputScrollCDing)
		{
			Window::updateInputScrollLock(msec);
		}
	}

	running = false;

	physics.join();

	SoundManager::DeleteSounds();
	SoundSystem::Destroy();

	delete game;	//Done with our game now...bye bye!
	return Quit();
}

