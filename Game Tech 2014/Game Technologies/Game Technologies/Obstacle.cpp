#include "Obstacle.h"


Obstacle::Obstacle(ObjectType* _tile, SceneNode* s, PhysicsNode* p, int _type, int _subType, int _obstacleType) : ObjectType(s, p, _type, _subType)
{
	tile = _tile;
	obstacleType = _obstacleType;
	offset.y = 150;
	random = rand() % 175 + 1;
	this->GetRenderNode().SetColour(Vector4(0, 0, 0, 1));

	
	
		
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

	if (subType == 0) // Top tile
	{
		offset.y = -150;
		if (lane == 0)
		{
			offset.x = -random;
		}
		else if (lane == 1)
		{
			offset.x = random;
		}
	}
	else if (subType == 1) // Right tile
	{
		offset.x = -offset.y;
		if (lane == 2)
		{
			offset.y = -random;
		}
		else if (lane == 3)
		{
			offset.y = random;
		}
	}
	else if (subType == 2) // Bottom tile
	{
		offset.y = offset.y;
		if (lane == 4)
		{
			offset.x = -random;
		}
		else if (lane == 5)
		{
			offset.x = random;
		}
	}
	else if (subType == 3) // Left tile
	{
		offset.x = offset.y;
		if (lane == 6)
		{
			offset.y = -random;
		}
		else if (lane == 7)
		{
			offset.y = random;
		}
	}
}