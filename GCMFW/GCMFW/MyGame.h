#pragma once

#include "GameEntity.h"
//#include "ObjectType.h"
//#include "Obstacle.h"
#include "GameClass.h"
#include "Camera.h"
#include "OBJMesh.h"
//#include "../nclgl/CubeRobot.h"
//#include "Vehicle.h"
//#include "Bullets.h"

class Vehicle;
class Bullets;
class MyGame : public GameClass	{
public:
	MyGame();
	~MyGame(void);

	virtual void UpdateGame(float msec);


protected:
	GameEntity* BuildRobotEntity();

	GameEntity* BuildCubeEntity(float size, Vector3 pos = Vector3(0,0,0));

	GameEntity* BuildSphereEntity(float radius, Vector3 pos, Vector3 vel);

	GameEntity* BuildQuadEntity(float size);

	Mesh* cube;
	Mesh* quad;
	Mesh* sphere;

	float timer;
};

