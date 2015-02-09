#pragma once
#include "MyGame.h"
class Bullets {
public:
	Bullets(Vector3 pos);
	~Bullets(void);
	GameEntity* BuildBulletEntity(float radius, Vector3 pos);

	void ShootBullets(); 

protected:
	Vector3 BulletsPosition;
	Vector3 temp,temp1,temp2,temp3; 

	Mesh* BulletsMesh;
	GameEntity* Bullet0; 


	int Speed_Bullets;

};