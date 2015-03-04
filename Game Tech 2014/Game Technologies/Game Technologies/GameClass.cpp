#include "GameClass.h"

GameClass* GameClass::instance = NULL;

GameClass::GameClass()	
{
	renderCounter	= 0.0f;
	physicsCounter	= 0.0f;
	currentGameState = GAME_PLAYING;
	instance		= this;
}

GameClass::~GameClass(void)	
{
	for(vector<GameEntity*>::iterator i = allEntities.begin(); i != allEntities.end(); ++i) 
	{
		delete (*i);
	}
//	delete gameCamera;
}

void GameClass::UpdatePhysics(float msec) 
{
	physicsCounter	+= msec;

	while(physicsCounter >= 0.0f)
	{
		if (currentGameState == GAME_PLAYING)
		{
			PhysicsSystem::GetPhysicsSystem().Update(PHYSICS_TIMESTEP);
		}
		physicsCounter -= PHYSICS_TIMESTEP;
	}
}

void GameClass::setCurrentState(GameState newGameState)
{
	currentGameState = newGameState;

	if (newGameState == GAME_PLAYING)
	{
		
	}

	else if (newGameState == GAME_PAUSED)
	{
		/*Window::GetWindow().LockMouseToWindow(false);
		Window::GetWindow().ShowOSPointer(true);*/
	}
	else if (newGameState == GAME_EXIT)
	{

	}
	else
	{

	}
}

void GameClass::UpdateRendering(float msec)
{
	renderCounter	-= msec;

	if (renderCounter <= 0.0f) //Update our rendering logic
	{
		if (currentGameState == GAME_PLAYING)
		{
			Renderer::GetRenderer().UpdateScene(1000.0f / (float)RENDER_HZ);
			Renderer::GetRenderer().RenderScene();

			if (!(Window::GetinputScrollCDing()) && (Window::GetKeyboard()->KeyDown(KEYBOARD_P) || Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE) ||
				(Window::GetControllerConnected() && Window::GetWindow().GetController()->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_START)))
			{
				setCurrentState(GAME_PAUSED);
				Window::enableInputScrollLock();
			}
		}

		else if (currentGameState == GAME_PAUSED)
		{
			Renderer::GetRenderer().RenderPauseMenu();

			if (!(Window::GetinputScrollCDing()) && (Window::GetKeyboard()->KeyDown(KEYBOARD_P) || Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE) ||
				(Window::GetControllerConnected() && Window::GetWindow().GetController()->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_START)))
			{
				setCurrentState(GAME_PLAYING);
				Window::enableInputScrollLock();
			}
		}
		renderCounter += (1000.0f / (float)RENDER_HZ);

		SoundSystem::GetSoundSystem()->Update(msec); //SoundSystem update 25.2.2015
	}
}