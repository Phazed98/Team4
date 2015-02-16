#pragma once
#include"MyGame.h"

class Vehicle {
public:
	Vehicle();
	~Vehicle(void);
	GameEntity* BuildPlayerEntity(float size, Vector3 pos);
	void UpdatePlayer(float msec);
	GameEntity* GetPlayer(){return Player;}
	void SetSpeedPlayer(int speed){Speed_Player = speed;}
	float GetSize(){return size;}

	Vector3 tempPosition;
protected:
	Vector3 PlayerPosition;
	Vector3 temp,temp1,temp2,temp3; 

	Mesh* PlayerMesh;
	GameEntity* Player; 

	float size;
	int Speed_Player;
	float Speed_Rotate;
	float Speed_Reduce;
};