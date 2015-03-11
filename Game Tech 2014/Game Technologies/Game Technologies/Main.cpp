
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

	if(!Window::Initialise("Game Technologies", 1000,800,false )) 
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

	GameTimer menuTimer;

	while (Window::GetWindow().UpdateWindow() && bp == NO_PRESSED)
	{
		float msec = menuTimer.GetTimedMS();
		bp = Renderer::GetRenderer().RenderMainMenu();
		Window::GetWindow().updateController(msec);
		if (Window::GetinputScrollCDing)
		{
			Window::updateInputScrollLock(msec);
		}
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

	bool running = true;
	std::thread physics(physicsLoop, game, std::ref(running));

	while(Window::GetWindow().UpdateWindow() && GameClass::GetGameClass().getCurrentState() != GAME_EXIT)
	{
		float msec = Window::GetWindow().GetTimer()->GetTimedMS();	//How many milliseconds since last update?

		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_2)) {
			SoundSystem::GetSoundSystem()->PlaySound(
				SoundManager::GetSound("../../Sounds/56900__syna-max__war.wav"), Vector3());
		}


		//if (PhysicsSystem::GetPhysicsSystem().GetVehicle()->GetCurrentPlaneID() == 0){
		//	SoundSystem::GetSoundSystem()->ChangeSceneSound(SoundManager::GetSound("../../Sounds/void.wav"));
		//}
		/*if (PhysicsSystem::GetPhysicsSystem().GetVehicle()->GetCurrentPlaneID() == 1){
			SoundSystem::GetSoundSystem()->ChangeSceneSound(SoundManager::GetSound("../../Sounds/thunder.wav"));
		}
		if (PhysicsSystem::GetPhysicsSystem().GetVehicle()->GetCurrentPlaneID() == 2){
			SoundSystem::GetSoundSystem()->ChangeSceneSound(SoundManager::GetSound("../../Sounds/water.wav"));
		}
		if (PhysicsSystem::GetPhysicsSystem().GetVehicle()->GetCurrentPlaneID() == 3){
			SoundSystem::GetSoundSystem()->ChangeSceneSound(SoundManager::GetSound("../../Sounds/wind.wav"));
		}*/
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_4))
		{
			SoundSystem::GetSoundSystem()->ChangeSceneSound(SoundManager::GetSound("../../Sounds/thunder.wav"));
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

