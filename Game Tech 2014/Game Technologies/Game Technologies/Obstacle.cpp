#include "Obstacle.h"


Obstacle::Obstacle(ObjectType* _tile, SceneNode* s, PhysicsNode* p, int _type, int _subType, int _obstacleType) : ObjectType(s, p, _type, _subType)
{
	tile = _tile;
	obstacleType = _obstacleType;

	//if (obstacleType == 0)
		offset.y = 100;
}


Obstacle::~Obstacle()
{
}


void Obstacle::Update(float msec)
{
	if (type == 1)
	{
		if (state == 0)
		{
			if (physicsNode->GetPosition().z < -(length * 14))
			{
				state = 1;
			}
			else
			{
				physicsNode->SetPosition(tile->GetPhysicsNode().GetPosition() + offset);
			}
		}
		
	}
}

void Obstacle::SetLane(int _lane)
{
	lane = _lane;
}