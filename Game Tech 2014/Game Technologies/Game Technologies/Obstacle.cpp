#include "Obstacle.h"


Obstacle::Obstacle(ObjectType* _tile, SceneNode* s, PhysicsNode* p, int _type, int _subType, int _obstacleType) : ObjectType(s, p, _type, _subType)
{
	tile = _tile;
	obstacleType = _obstacleType;
	offset.y = (TILE_HEIGHT + size);
	random = rand() % 175 + 1;
	this->GetRenderNode().SetColour(Vector4(0, 0, 0, 1));

	if (obstacleType == 1)
	{
		renderNode->SetColour(Vector4(0.835f, 0.262f, 0.043f, 1.0f));
	}
	else if (obstacleType == 2)
	{
		renderNode->SetColour(Vector4(0.827f, 0.835f, 0.043f, 1.0f));
	}
	else if (obstacleType == 3)
	{
		renderNode->SetColour(Vector4(0.564f, 0.043f, 0.835f, 1.0f));
	}
	
	goingLeft = true;

	bullet = NULL;

	movingObsSpeed = 5.0f;

	count_time = 0;
}


Obstacle::~Obstacle()
{
}


void Obstacle::Update(float msec)
{
	//Controls shooting obstacles
	if (obstacleType == 1)
	{
		if (abs(player->GetPosition().y - physicsNode->GetPosition().y) < 400
			&& abs(player->GetPosition().x - physicsNode->GetPosition().x) < 400
			&& physicsNode->GetPosition().z < player->GetPosition().z)
		{
			//if (count_time == 80)
			if (count_time == 200)//&& bullet->GetPhysicsNode().GetPosition().z > player->GetPosition().z)
			{
				bullet->GetPhysicsNode().SetPosition(physicsNode->GetPosition());
				count_time = 0;
				Shoot();
			}

			count_time++;

		}
		else
		{
			bullet->GetPhysicsNode().SetPosition(physicsNode->GetPosition());
		}
	}

	if (type == 1)
	{
		if (state == 0)
		{
			if (physicsNode->GetPosition().z > (5000))
			{
				state = 1;
			}
			else
			{
				physicsNode->SetPosition(tile->GetPhysicsNode().GetPosition() + offset);
			}
		}

		HandleMovingObstacle();

	}


}

void Obstacle::SetLane(int _lane)
{
	lane = _lane;


	if (subType == 0) // Top tile
	{
		offset.y = -(TILE_HEIGHT + size);
		if (lane == 0)
		{
			offset.x = (float)-random;
		}
		else if (lane == 1)
		{
			offset.x = (float)random;
		}

		if (obstacleType == 5)
		{
			offset.x = 0;
		}
	}
	else if (subType == 1) // Right tile
	{
		offset.x = -(TILE_HEIGHT + size);
		if (lane == 2)
		{
			offset.y = (float)-random;
		}
		else if (lane == 3)
		{
			offset.y = (float)random;
		}
		
		if (obstacleType == 5)
		{
			offset.y = 0;
		}
	}
	else if (subType == 2) // Bottom tile
	{
		offset.y = (TILE_HEIGHT + size);
		if (lane == 4)
		{
			offset.x = (float)-random;
		}
		else if (lane == 5)
		{
			offset.x = (float)random;
		}
		 
			if (obstacleType == 5)
		{
			offset.x = 0;
		}
	}
	else if (subType == 3) // Left tile
	{
		offset.x = (TILE_HEIGHT + size);
		if (lane == 6)
		{
			offset.y = (float)-random;
		}
		else if (lane == 7)
		{
			offset.y = (float)random;
		}
		
		if (obstacleType == 5)
		{
			offset.y = 0;
		}
	}
}

void Obstacle::HandleMovingObstacle()
{
	if (obstacleType == 4)
	{
		if (subType == 0 || subType == 2) // Top and bottom tile
		{
			if (goingLeft)
			{
				offset.x += -movingObsSpeed;
				if (offset.x < -180)
				{
					goingLeft = false;
				}
			}
			else
			{
				offset.x += movingObsSpeed;
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
				offset.y += -movingObsSpeed;
				if (offset.y < -180)
				{
					goingLeft = false;
				}
			}
			else
			{
				offset.y += movingObsSpeed;
				if (offset.y > 180)
				{
					goingLeft = true;
				}
			}
		}
	}
}

void Obstacle::Shoot()
{
	Vector3 direction = player->GetPosition() - physicsNode->GetPosition();
	direction.Normalise();
	bullet->GetPhysicsNode().SetLinearVelocity(direction * 2);
}