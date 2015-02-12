#pragma once
#include"MyGame.h"

class Vehicle {
public:
	Vehicle();
	~Vehicle(void);
	GameEntity* BuildPlayerEntity(float size, Vector3 pos);
	void UpdatePlayer(float msec);
	GameEntity* GetPlayer(){return Player;}

	Vector3 tempPosition;
protected:
	Vector3 PlayerPosition;
	Vector3 temp,temp1,temp2,temp3; 

	Mesh* PlayerMesh;
	GameEntity* Player; 

	int Speed_Player;
	float Speed_Rotate;
	float Speed_Reduce;
};