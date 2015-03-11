/******************************************************************************
Class:GameClass
Implements:
Author:Rich Davison	<richard.davison4@newcastle.ac.uk>, Callum Rhodes <c.g.rhodes@ncl.ac.uk> and YOU!
Description: A very simple 'base class' with which to base your game projects
on. This will simply update all of the 'sub systems' of the program
(ie the Renderer, the PhysicsSystem, and later on the SoundSystem).

This class also holds a vector of GameEntities - these are the classes that
represent all of the objects in your game.

There's also a Camera in here, so that the 'game' component of the program has
control over what the Renderer 'sub system' looks at.

-_-_-_-_-_-_-_,------,
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""

*//////////////////////////////////////////////////////////////////////////////

#pragma once
#include "Renderer.h"
#include "PhysicsSystem.h"
#include "GameEntity.h"
#include "Input.h"
#include "ChaseCamera.h"

#include <vector>

#define RENDER_HZ	60
#define PHYSICS_HZ	120

#define PHYSICS_TIMESTEP (1000.0f / (float)PHYSICS_HZ)

enum GameState {
	GAME_PLAYING,
	GAME_PAUSED,
	Game_LOADING,
	GAME_EXIT
};

class GameClass	{
public:
	GameClass();
	~GameClass(void);

	virtual void UpdatePhysics(float msec);
	virtual void UpdateRendering(float msec);

	virtual void UpdateGame(float msec) = 0;

	static GameClass& GetGameClass() { return *instance; }
	void setCurrentState(GameState newGameState);
	GameState getCurrentState() { return currentGameState; }


protected:
	float renderCounter;
	float physicsCounter;

	std::vector<GameEntity*> allEntities;
	ChaseCamera* gameCamera;
	//Camera* gameCamera;
	static GameClass* instance;
	GameState currentGameState;
	JoyPadNum	pad;

	int delayCounter;
};

