#pragma once
#include "ObjectType.h"


class Obstacle : public ObjectType
{
public:
	Obstacle(ObjectType* _tile, SceneNode* s, PhysicsNode* p, int _type, int _subType, int _obstacleType);
	~Obstacle();

	virtual void	Update(float msec);

	void SetLane(int _lane);

	int getObstacleType(){ return obstacleType; }
	void resetObstacle(){
		state = 0;
		physicsNode->SetPosition(Vector3(0, 0, 0));
	}

	void SetTile(ObjectType* _tile) { tile = _tile; }

	void SetBullet(GameEntity* _bullet) { bullet = _bullet; }

	void SetPlayer(PhysicsNode* _player) { player = _player; }

	void Shoot();

	void ResetBullet();

private:

	int lane; // Lanes are from 0 t 7 starting from top, two lanes per tile

	Vector3 offset;

	int obstacleType; //0 for obstacle, 1 for enemy, 2 for coins, 3 for powerups, 4 for moving obstacle

	ObjectType* tile;

	bool goingLeft;

	GameEntity* bullet;

	PhysicsNode* player;


	int count_time;

};

