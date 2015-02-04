#pragma once
#include "GameEntity.h"
class ObjectType : 	public GameEntity
{
public:
	ObjectType(SceneNode* s, PhysicsNode* p, int _type, int _subType);

	~ObjectType();

	virtual void	Update(float msec);


protected:

	int type; // 1- Plane, 2- Obstacle

	int subType; // 1- top plane, 2- right plane, 3- down plane, 4- left plane

	int state; // 
};

