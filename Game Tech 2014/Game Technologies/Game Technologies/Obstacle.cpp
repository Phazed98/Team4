#include "Obstacle.h"


Obstacle::Obstacle(ObjectType* _tile, SceneNode* s, PhysicsNode* p, int _type, int _subType, int _obstacleType) : ObjectType(s, p, _type, _subType)
{
	tile = _tile;
	obstacleType = _obstacleType;
	offset.y = 150;
	random = rand() % 175 + 1;
	this->GetRenderNode().SetColour(Vector4(0, 0, 0, 1));
	
	if (obstacleType == 1)
	{
		renderNode->SetColour(Vector4(0.835, 0.262, 0.043, 1));
	}
	else if (obstacleType == 2)
	{
		renderNode->SetColour(Vector4(0.827, 0.835, 0.043, 1));
	}
	else if (obstacleType == 3)
	{
		renderNode->SetColour(Vector4(0.564, 0.043, 0.835, 1));
	}
	goingLeft = true;

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

		if (obstacleType == 4)
		{
			if (subType == 0 || subType == 2) // Top and bottom tile
			{
				if (goingLeft)
				{
					offset.x += -10;
					if (offset.x < -180)
					{
						goingLeft = false;
					}
				}
				else
				{
					 offset.x += 10;
					 if (offset.x > 180)
					{
						goingLeft = true;
					}
				}
				
			}
			else
			{
				if (goingLeft)
				{
					offset.y += -10;
					if (offset.y < -180)
					{
						goingLeft = false;
					}
				}
				else
				{
					offset.y += 10;
					if (offset.y > 180)
					{
						goingLeft = true;
					}
				}
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
		offset.x = -150;
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
		offset.y = 150;
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
		offset.x = 150;
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