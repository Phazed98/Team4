#pragma once
#include "GameEntity.h"
class ObjectType : 	public GameEntity
{
public:
	ObjectType(SceneNode* s, PhysicsNode* p, int _type, int _subType);

	~ObjectType();

	virtual void	Update(float msec);

	int getState(){ return state; }
	void setState(int _state){ state = _state; }

	int getSubType(){ return subType; }

	int getRandom(){ return random; }
	void setRandom(int _r){ random = _r; }

	void reset();



protected:

	int type; // 0- Plane, 1- Obstacle

	int subType; // Clockwise assignment of planes 0- top plane, 1- right plane, 2- bottom plane, 3- left plane

	int state; // 0- Default, 1-

	void SetInitialAttributes(); // Initialise the position of an object depending on its type and subtype
	float length;
	int random;
};

