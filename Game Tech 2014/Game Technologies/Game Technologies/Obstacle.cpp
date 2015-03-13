#include "Obstacle.h"
#include "PhysicsSystem.h"

Obstacle::Obstacle(ObjectType* _tile, SceneNode* s, PhysicsNode* p, int _type, int _subType, int _obstacleType) : ObjectType(s, p, _type, _subType)
{
	tile = _tile;
	obstacleType = _obstacleType;
	offset.y = (TILE_HEIGHT + size);
	random = rand() % 175 + 1;
	powerupType = rand() % 3 + 1;
	this->GetRenderNode().SetColour(Vector4(0, 0, 0, 1));

	if (obstacleType == 3) // PowerUp
	{
		if (powerupType == 1) // Slow
		{
			Renderer::GetRenderer().SetYellowformSceneNode(renderNode);
			
		}
		else if (powerupType == 2) // Cooldown
		{
			Renderer::GetRenderer().SetRedformSceneNode(renderNode);
			
		}
		else if (powerupType == 3) // Immunity
		{
			Renderer::GetRenderer().SetBlueformSceneNode(renderNode);
			
		}
	}


	directionRand = rand() % 100 + 1;
	if (directionRand > 50)
	{
		goingLeft = true;
	}
	else
	{
		goingLeft = false;
	}


	bullet = NULL;

	movingObsSpeed = 2.5f;

	count_time = 0;
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
			if (physicsNode->GetPosition().z > (END_POSITION))
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

	HandleShootingObstacles();
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

		/*if (obstacleType == 5)
		{
		offset.x = 0;
		}*/
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

		/*if (obstacleType == 5)
		{
		offset.y = 0;
		}*/
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

		/*if (obstacleType == 5)
		{
		offset.x = 0;
		}*/
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

		/*if (obstacleType == 5)
		{
		offset.y = 0;
		}*/
	}
}

void Obstacle::HandleMovingObstacle()
{
	if (obstacleType == 4)
	{
		if (subType == 0) // Top
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
		else if (subType == 1 || subType == 3)
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
		else if (subType == 2) // Obstacles that belong to Earth/Bottom plane 
		{
			if (goingLeft)
			{
				offset.x += -movingObsSpeed;
				physicsNode->SetAngularVelocity(Vector3(0, 0, 0.005f));
				if (offset.x < -180)
				{
					offset.x = 180;
				}
			}
			else
			{
				offset.x += movingObsSpeed;
				physicsNode->SetAngularVelocity(Vector3(0, 0, -0.005f));
				if (offset.x > 180)
				{
					offset.x = -180;
				}
			}

		}
	}
}

void Obstacle::HandleShootingObstacles()
{
	if (obstacleType == 1 && PhysicsSystem::GetVehicle()->getIsPlaneSwitching() == false)
	{
		if (PhysicsSystem::GetVehicle()->GetCurrentPlaneID() == subType && physicsNode->GetPosition().z < -length)
		{
			if (count_time == 200)
			{
				bullet->GetPhysicsNode().SetPosition(physicsNode->GetPosition());
				count_time = 0;
				Shoot();
			}

			count_time++;
		}

	}
}

void Obstacle::Shoot()
{

	Vector3 direction = player->GetPosition() - physicsNode->GetPosition();
	direction.Normalise();
	bullet->GetPhysicsNode().SetLinearVelocity(direction * 2);
	//bullet->GetPhysicsNode().SetOrientation(Quaternion::LookAt(bullet->GetPhysicsNode().GetPosition(), player->GetPosition()));
	Vector3 rotation = bullet->GetPhysicsNode().GetOrientation().GetEulerAngles();
	rotation.y += 90;
	/*if (rotation.y > 360)
	{
	rotation.y -= 360;
	}*/
	bullet->GetPhysicsNode().SetOrientation(Quaternion::EulerAnglesToQuaternion(rotation.x, rotation.y, rotation.z));

}

void Obstacle::resetObstacle(){
	state = 0;
	physicsNode->SetPosition(Vector3(1000, 1000, 1000));
	directionRand = rand() % 100 + 1;
	//powerupType = rand() % 3 + 1;
	offset.y = (TILE_HEIGHT + size);
	if (directionRand > 30)
	{
		goingLeft = true;
	}
	else
	{
		goingLeft = false;
	}
	//if (obstacleType == 3) // PowerUp
	//{
	//	if (powerupType == 1) // Slow
	//	{
	//		Renderer::GetRenderer().SetYellowformSceneNode(renderNode);
	//		renderNode->SetColour(Vector4(1, 0, 0, 1.0f));
	//	}
	//	else if (powerupType == 2) // Cooldown
	//	{
	//		Renderer::GetRenderer().SetRedformSceneNode(renderNode);
	//		renderNode->SetColour(Vector4(0, 1, 0, 1.0f));
	//	}
	//	else if (powerupType == 3) // Immunity
	//	{
	//		Renderer::GetRenderer().SetBlueformSceneNode(renderNode);
	//		renderNode->SetColour(Vector4(0, 0, 1, 1.0f));
	//	}
	//}
}