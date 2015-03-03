#pragma once
#include "GameEntity.h"
#include "PlaneInfo.h"

class ObjectType : public GameEntity
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

	void SetPos(Vector3 pos);



protected:

	int type; // 0- Plane, 1- Obstacle, 2- Check Point

	int subType; // Clockwise assignment of planes 0- top plane, 1- right plane, 2- bottom plane, 3- left plane, 4- middle(checkPoint only)

	int state; // 0- Default, 1-

	void SetInitialAttributes(); // Initialise the position of an object depending on its type and subtype

	float length;

	int random;

	int resetDistance; // Reset distance used for check point

	int checkPointTimer;

	bool increaseTimer;

	//float initialPositionZ;

};

