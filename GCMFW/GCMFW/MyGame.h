#pragma once

#include "GameEntity.h"
#include "ObjectType.h"
#include "Obstacle.h"
#include "GameClass.h"
#include "Camera.h"
#include "OBJMesh.h"

#include "ChaseCamera.h"
//#include "PlaneInfo.h"
#include "MathHelper.h"
//#include "../nclgl/CubeRobot.h"
//#include "Vehicle.h"
//#include "Bullets.h"

#define NUMBEROFPLANES 10
#define NUMBEROFOBSTACLES 20

class Vehicle;
class Bullets;
class MyGame : public GameClass	{
public:
	MyGame();
	~MyGame(void);

	virtual void UpdateGame(float msec);
	int getIndexOfAllEtities(GameEntity* _G);

	int getIndexOfElements(ObjectType* _G);

	int getDrawingPlanes(int);

	int getEmptyIndex(int subType);

	void handlePlanes(float msec);

	void CreateObstacle(ObjectType* _obj);

	int getObstacleEmptyIndex(int _subType, int);


protected:

	/* Basic Build Entities*/
	GameEntity* BuildCubeEntity(float size, Vector3 pos = Vector3(0,0,0));
	GameEntity* BuildSphereEntity(float radius, Vector3 pos, Vector3 vel);
	GameEntity* BuildQuadEntity(float size);
	GameEntity* BuildPlaneEntity(int position);


	/*Advanced Build Entities*/
	GameEntity* BuildBulletEntity(float radius, Vector3 pos);
	ObjectType* BuildObjectEntity(int type, int subType);
	Obstacle*	BuildObstacleEntity(float size, int type, int subType, ObjectType*, int);
	GameEntity*	BuildNewObstacleEntity(int plane);
	GameEntity* BuildBuffEntity(float radius, Vector3 pos); //new 6.2.2015 Daixi



	Mesh* cube;
	Mesh* quad;
	Mesh* sphere;

	Mesh* cubeWater;
	Mesh* cubeAir;
	Mesh* cubeFire;
	Mesh* cubeEarth;

	GameEntity* upperPlanes[NUMBEROFPLANES];
	GameEntity* lowerPlanes[NUMBEROFPLANES];
	GameEntity* rightPlanes[NUMBEROFPLANES];
	GameEntity* leftPlanes[NUMBEROFPLANES];

	GameEntity* upperObstacle[20];
	GameEntity* lowerObstacle[20];
	GameEntity* rightObstacle[20];
	GameEntity* leftObstacle[20];

	Vehicle* Car;

	std::vector<ObjectType*> top;
	std::vector<ObjectType*> right;
	std::vector<ObjectType*> bottom;
	std::vector<ObjectType*> left;

	std::vector<std::vector<ObjectType*> > elements;
	std::vector<std::vector<ObjectType*> > reference;

	std::vector<std::vector<Obstacle*> > obstacleElements;
	std::vector<Obstacle*> obstacleReference;

	float timer;
	int count_time;  //new 4.2.2015 Daixi
	int Speed_Player; //4.2.2015 Daixi


};

