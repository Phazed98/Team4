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
	delete gameCamera;
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
		Window::GetWindow().LockMouseToWindow(true);
		Window::GetWindow().ShowOSPointer(false);
	}

	else if (newGameState == GAME_PAUSED)
	{
		Window::GetWindow().LockMouseToWindow(false);
		Window::GetWindow().ShowOSPointer(true);
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

	if(renderCounter <= 0.0f) //Update our rendering logic
	{	
		if (currentGameState == GAME_PLAYING)
		{
			Renderer::GetRenderer().UpdateScene(1000.0f / (float)RENDER_HZ);
			Renderer::GetRenderer().RenderScene();

			if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_P) || Window::GetKeyboard()->KeyTriggered(KEYBOARD_ESCAPE))
			{
				setCurrentState(GAME_PAUSED);
			}
		}

		else if (currentGameState == GAME_PAUSED)
		{
			Renderer::GetRenderer().RenderMenu();

			if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_P) || Window::GetKeyboard()->KeyTriggered(KEYBOARD_ESCAPE))
			{
				setCurrentState(GAME_PLAYING);
			}
		}
		renderCounter += (1000.0f / (float)RENDER_HZ);

		SoundSystem::GetSoundSystem()->Update(msec); //SoundSystem update 25.2.2015
	}
}