#pragma once
#include "Renderer.h"
#include "PhysicsSystem.h"
#include "GameEntity.h"
#include "SoundSystem.h"

#include <vector>

#define RENDER_HZ	60
#define PHYSICS_HZ	120

#define PHYSICS_TIMESTEP (1000.0f / (float)PHYSICS_HZ)


class ChaseCamera;

enum GameState { 
	GAME_PLAYING, 
	GAME_PAUSED,
	Game_LOADING,
	GAME_OVER,
	GAME_EXIT };


class GameClass	{
public:
	GameClass();
	~GameClass(void);
	
	virtual void UpdatePhysics(float msec);
	virtual void UpdateRendering(float msec);

	virtual void UpdateGame(float msec) = 0;

	static GameClass& GetGameClass() { return *instance;}
	void setCurrentState(GameState newGameState);
	GameState getCurrentState() { return currentGameState; }
	vector<GameEntity*> getAllentitities() { return allEntities; }

protected:
	float renderCounter;
	float physicsCounter;

	vector<GameEntity*> allEntities;

	ChaseCamera* gameCamera;
	static GameClass* instance;
	GameState currentGameState;

	//added by Sam to pause game on controller disconnect
	bool controllerConnected = false;
};

