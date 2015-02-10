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

private:

	int lane; // Lanes are from 0 t 7 starting from top, two lanes per tile

	Vector3 offset;

	int obstacleType;

	ObjectType* tile;

};

