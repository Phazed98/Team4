/******************************************************************************
Class:MyGame
Implements:GameEntity
Author:Rich Davison	<richard.davison4@newcastle.ac.uk>, Callum Rhodes <c.g.rhodes@ncl.ac.uk> and YOU!
Description: This is an example of a derived game class - all of your game 
specific things will go here. Such things would include setting up the game 
environment (level, enemies, pickups, whatever), and keeping track of what the
gameplay state is (Is the player dead? Has he/she won the game? And so on).

To get you started, there's a few static functions included that will create 
create some simple game objects - ideally your game object loading would be
handled by config files or somesuch, but for a simple demo game, these 
functions are a good start. Don't be afraid to modify/delete/add more of these!

Finally, there's some calls to the OGLRenderer debug draw functions that have
been added to the course this year. These are really useful for debugging the
state of your game in a graphical way - i.e drawing a line from a character
to the item it is aiming at, or pointing in the direction it is moving, or
for drawing debug bounding volumes to see when objects collide. Feel free
to add more of these debug functions - how they are implemented is covered
in the code definitions for them, and also in the extra introduction 
document on the research website.

Have fun!


-_-_-_-_-_-_-_,------,   
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""   

*//////////////////////////////////////////////////////////////////////////////



#pragma once
#include "GameEntity.h"
#include "SoundEntity.h"
#include "GameClass.h"
#include "../nclgl/Camera.h"
#include "../nclgl/CubeRobot.h"
#include "../nclgl/Sound.h"
#include "../nclgl/SoundEmitter.h"
#include "../nclgl/SoundManager.h"
#include "../nclgl/SoundSystem.h"
#include <queue>

class Powerups;
class Vehicle;
class Bullets;
class Coins;
class MyGame : public GameClass	{
public:
	MyGame();
	~MyGame(void);

	virtual void UpdateGame(float msec);

	void UpdatePlayer(float msec);

	void ShootBullets();  //new 4.2.2015 Daixi

protected:
	GameEntity* BuildRobotEntity();

	GameEntity* BuildCubeEntity(float size);

	GameEntity* BuildSphereEntity(float radius, Vector3 pos, Vector3 vel);

	GameEntity* BuildQuadEntity(float size);

	GameEntity* BuildBuffEntity(float radius, Vector3 pos); //new 6.2.2015 Daixi

	GameEntity* BuildPlayerEntity(float size, Vector3 pos);

	GameEntity* BuildSoundEntity();

	GameEntity* Enemy;  //new 4.2.2015 Daixi  just for test i create some enemy

	GameEntity* BuffEntity;

	GameEntity* sound0;

	Vehicle* Car;

	Bullets* bullet;

	Powerups* AllBuff;

	Coins* AllCoins;

	Vector3 PositionEnemy;  //new 4.2.2015 Daixi
	Vector3 Position0;
	Vector3 Position1;

	Vector3 temp;   // 5.2.2015 for caculate the length between Player and Buff
	Vector3 temp1,temp2,temp3;  //Control the player speed  6.2.2015

	Mesh* PlayerMesh; //new 3.2.2015 Daixi
	Mesh* cube;
	Mesh* quad;
	Mesh* sphere;

	int count_time;  //new 4.2.2015 Daixi
	int Speed_Player; //4.2.2015 Daixi
	int distance;

};


