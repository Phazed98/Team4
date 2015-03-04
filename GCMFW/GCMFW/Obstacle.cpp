#include "Obstacle.h"


Obstacle::Obstacle(ObjectType* _tile, SceneNode* s, PhysicsNode* p, int _type, int _subType, int _obstacleType) : ObjectType(s, p, _type, _subType)
{
	tile = _tile;
	obstacleType = _obstacleType;
	offset.setY(150);
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

	movingObsSpeed = 0.05f;

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
		//if (abs(player->GetPosition().getY() - physicsNode->GetPosition().getY()) < 400
		//	&& abs(player->GetPosition().getX() - physicsNode->GetPosition().getX()) < 400
		//	&& physicsNode->GetPosition().getZ() < player->GetPosition().getZ())
		//{
		//	//if (count_time == 80)
		//	if (count_time == 200)//&& bullet->GetPhysicsNode().GetPosition().getZ() > player->GetPosition().getZ())
		//	{
		//		bullet->GetPhysicsNode().SetPosition(physicsNode->GetPosition());
		//		count_time = 0;
		//		Shoot();
		//	}

		//	count_time++;

		//}
		//else
		//{
		//	bullet->GetPhysicsNode().SetPosition(physicsNode->GetPosition());
		//}
	}

	if (type == 1)
	{
		if (state == 0)
		{
			if (physicsNode->GetPosition().getZ() > (5000))
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
					offset.setX(offset.getX() + -movingObsSpeed);
					if (offset.getX() < -180)
					{
						goingLeft = false;
					}
				}
				else
				{
					offset.setX(offset.getX() + movingObsSpeed);
					if (offset.getX() > 180)
					{
						goingLeft = true;
					}
				}

			}
			else
			{
				if (goingLeft)
				{
					offset.setY(offset.getY() + -movingObsSpeed);
					if (offset.getY() < -180)
					{
						goingLeft = false;
					}
				}
				else
				{
					offset.setY(offset.getY() + movingObsSpeed);
					if (offset.getY() > 180)
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
		offset.setY(-150);
		if (lane == 0)
		{
			offset.setX((float)-random);
		}
		else if (lane == 1)
		{
			offset.setX((float)random);
		}
	}
	else if (subType == 1) // Right tile
	{
		offset.setX(-150);
		if (lane == 2)
		{
			offset.setY((float)-random);
		}
		else if (lane == 3)
		{
			offset.setY((float)random);
		}
	}
	else if (subType == 2) // Bottom tile
	{
		offset.setY(150);
		if (lane == 4)
		{
			offset.setX((float)-random);
		}
		else if (lane == 5)
		{
			offset.setX((float)random);
		}
	}
	else if (subType == 3) // Left tile
	{
		offset.setX(150);
		if (lane == 6)
		{
			offset.setY((float)-random);
		}
		else if (lane == 7)
		{
			offset.setY((float)random);
		}
	}
}

void Obstacle::Shoot()
{
	Vector3 direction = player->GetPosition() - physicsNode->GetPosition();
	normalize(direction);
	bullet->GetPhysicsNode().SetLinearVelocity(direction * 2);
}