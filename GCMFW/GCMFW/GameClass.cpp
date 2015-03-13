#include "GameClass.h"

GameClass* GameClass::instance = NULL;

GameClass::GameClass()
{
	renderCounter = 0.0f;
	physicsCounter = 0.0f;
	delayCounter = 0;
	pad = JOYPAD_A;
	instance = this;
	currentGameState = GAME_PLAYING;
}

GameClass::~GameClass(void)
{
	for (std::vector<GameEntity*>::iterator i = allEntities.begin(); i != allEntities.end(); ++i)
	{
		delete (*i);
	}
	delete gameCamera;
}

void GameClass::UpdatePhysics(float msec)
{
	physicsCounter += msec;

	while (physicsCounter >= 0.0f)
	{
		if (currentGameState == GAME_PLAYING)
		{
			PhysicsSystem::GetPhysicsSystem().Update(PHYSICS_TIMESTEP);
		}

		physicsCounter -= PHYSICS_TIMESTEP;
	}
}

void GameClass::UpdateRendering(float msec)
{
	renderCounter -= msec;

	if (delayCounter > 0)
		delayCounter--;

	float y, p;
	if (pad == JOYPAD_NONE)
	{
		return;
	}

	Input::UpdateJoypad();
	Input::GetJoypadMovement(y, p, pad);
	

	if (renderCounter <= 0.0f) //Update our rendering logic
	{
		if (currentGameState == GAME_PLAYING)
		{
			Renderer::GetRenderer().UpdateScene(1000.0f / (float)RENDER_HZ);
			Renderer::GetRenderer().RenderScene();;

			if (Input::ButtonDown(INPUT_START, pad))
			{
				setCurrentState(GAME_PAUSED);
			}
		}

		else if (currentGameState == GAME_PAUSED)
		{
			Renderer::GetRenderer().RenderPauseMenu();

			if (Input::ButtonDown(INPUT_TRIANGLE, pad))
			{
				setCurrentState(GAME_PLAYING);
			}

		}

		else if (currentGameState == GAME_OVER)
		{
		//	Renderer::GetRenderer().RenderGameOver();

			Renderer::GetRenderer().RenderGameOver();


			if (Input::ButtonDown(INPUT_TRIANGLE, pad))
			{
				setCurrentState(GAME_EXIT);
			}


		}
		renderCounter += (1000.0f / (float)RENDER_HZ);
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
		
	}
	else if (newGameState == GAME_EXIT)
	{

	}
	else if (newGameState == GAME_OVER)
	{

	}
	else
	{

	}
}
