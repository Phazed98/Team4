#include "PhysicsSystem.h"
#include "CollisionHelper.h"
#include "OBJMesh.h"
//#include "GJK.h"

PhysicsSystem* PhysicsSystem::instance = 0;
int PhysicsSystem::fps = 0;
Vehicle* PhysicsSystem::playerVehicle = NULL;
float PhysicsSystem::track_speed = 15.0f;

PhysicsSystem::PhysicsSystem(void)
{
	time = 0;
	nbFrames = 0;
	maxTrackSpeed = 15.0f;

	//Sam - moving here for scoping reasons
	OBJMesh* PlayerMesh = new OBJMesh(MESHDIR"SR-71_Blackbird.obj");
	PlayerMesh->SetDefaultTexture(*GCMRenderer::LoadGTF("/Textures/lava_texture.gtf"));

	//add by steven because it should use render instance
	spaceship_scene_node = new SpaceshipSceneNode(PlayerMesh);
	//store the ssn in render to render the particle system at the end of everything
	Renderer::GetRenderer().SetSpaceshipSceneNode(spaceship_scene_node);

	this->playerVehicle = new Vehicle(PlayerMesh, 3.0f, 0.0023f, 2, 0.02f, spaceship_scene_node);
	this->playerPhysNode = playerVehicle->GetPhysicsNode();
	playerPhysNode->SetLinearVelocity(Vector3(0, 0, 0));

	playerIsReversing = false;
	playerIsAccelerating = false;
	gameSpeedFromTime = 0.0f;
	immunityTime = 5000;
	checkPointTimer = 50000;
	canDie = false;


	score = 0;
	scoreMultiplier = 1;
	numberOfCoins = 0;
	actualScore = 0;
	increaseScore = true;
}

PhysicsSystem::~PhysicsSystem(void)
{
	for (int i = 0; i < allSprings.size(); i++)
	{
		delete allSprings[i];
	}
}

void	PhysicsSystem::Update(float msec)
{
	//Update FPS
	nbFrames++;
	time += msec;
	checkPointTimer -= msec;


	//Calculate Score
	CalculateScore(msec);

	//--------added by sam-----------------------
	//Update max track speed
	maxTrackSpeed += (msec / DIFFICULTY_SPEED_INCREASE);

	//if not bouncing due to collision, set track speed to max speed
	if (!playerIsReversing && !playerIsAccelerating)
	{
		track_speed = maxTrackSpeed;
	}
	//--------------------------------------------

	

	if (time >= 1000)
	{
		fps = nbFrames;
		nbFrames = 0;
		time = 0;
		canDie = true;
	}

	if (immunityTime > 0)
	{
		immunityTime--;
	}

	if (immunityTime < 1)
	{
		canDie = true;
	}


	ObstacleCollisions();

	if (canDie)
	{
		if (!CheckOnATile()&& !(playerVehicle->getIsSwitchingPlane()))
		{
			std::cout << " For those of you who dont know boobs loook like ( . Y . )" << std::endl;
			GameClass::GetGameClass().setCurrentState(GAME_OVER);
		}
	}

	for (std::vector<PhysicsNode*>::iterator i = allNodes.begin(); i != allNodes.end(); ++i)
	{
		(*i)->Update(msec);
	}


	playerVehicle->UpdatePlayer(msec);
	playerPhysNode->Update(msec);
}




//Created by Sam 
void	PhysicsSystem::BroadPhaseCollisions()
{
	////get current plane
	////int currentPlaneIndex = GameClass::GetGameClass().GetCurrentPlaneIndex();
	//int currentPlaneIndex = 0; //TEMP ##################################################################

	////only check collisions on the active player plane
	////NOTE this will need changing a little if we add other physics objects in. 
	//switch (currentPlaneIndex)
	//{
	//case 0:
	//	for (PhysicsNode* &node : *TilePhysicsNodeArray0)
	//	{
	//		//Check if POTENTIAL collision with player
	//		if (CheckAABBCollision(*player, *node))
	//		{
	//			//Perform collision check between player and object
	//			CollisionData* collisionData = new CollisionData();
	//			GJK* gjkObj = new GJK();
	//			if (gjkObj->CollisionDetection(*player, *node, collisionData))
	//			{
	//				//resolve collision
	//				AddCollisionImpulse(*player, *node, collisionData);
	//			}
	//			delete gjkObj;
	//			delete collisionData;
	//		}
	//	}
	//	break;
	//case 1:
	//	for (PhysicsNode* &node : *TilePhysicsNodeArray1)
	//	{
	//		//Check if POTENTIAL collision with player
	//		if (CheckAABBCollision(*player, *node))
	//		{
	//			//Perform collision check between player and object
	//			CollisionData* collisionData = new CollisionData();
	//			GJK* gjkObj = new GJK();
	//			if (gjkObj->CollisionDetection(*player, *node, collisionData))
	//			{
	//				//resolve collision
	//				AddCollisionImpulse(*player, *node, collisionData);
	//			}

	//		}
	//	}
	//	break;
	//case 2:
	//	for (PhysicsNode* &node : *TilePhysicsNodeArray2)
	//	{
	//		//Check if POTENTIAL collision with player
	//		if (CheckAABBCollision(*player, *node))
	//		{
	//			//Perform collision check between player and object
	//			CollisionData* collisionData = new CollisionData();
	//			GJK* gjkObj = new GJK();
	//			if (gjkObj->CollisionDetection(*player, *node, collisionData))
	//			{
	//				//resolve collision
	//				AddCollisionImpulse(*player, *node, collisionData);
	//			}

	//		}
	//	}
	//	break;
	//case 3:
	//	for (PhysicsNode* &node : *TilePhysicsNodeArray3)
	//	{
	//		//Check if POTENTIAL collision with player
	//		if (CheckAABBCollision(*player, *node))
	//		{
	//			//Perform collision check between player and object
	//			CollisionData* collisionData = new CollisionData();
	//			GJK* gjkObj = new GJK();
	//			if (gjkObj->CollisionDetection(*player, *node, collisionData))
	//			{
	//				//resolve collision
	//				AddCollisionImpulse(*player, *node, collisionData);
	//			}

	//		}
	//	}
	//	break;
	//}
}

//Added by Sam - basic AABB for broadphase
bool PhysicsSystem::CheckAABBCollision(PhysicsNode &n0, PhysicsNode &n1)
{
	float AABBSum = n0.GetAABBHalfLength() + n1.GetAABBHalfLength();
	float distance = abs(n0.GetPosition().getZ() - n1.GetPosition().getZ());
	//check Z axis for early out
	if (distance < AABBSum)
	{
		float distance = abs(n0.GetPosition().getX() - n1.GetPosition().getX());
		//next is X axis
		if (distance < AABBSum)
		{
			float distance = abs(n0.GetPosition().getY() - n1.GetPosition().getY());
			//Y axis last
			if (distance < AABBSum)
			{
				return true;
			}
		}
	}
	return false;
}

void	PhysicsSystem::reverseTrackDeccel()
{
	float normalisedProgress = (zAmountToReverse - distanceReversed) / zAmountToReverse;
	track_speed = -(normalisedProgress * initialTrackSpeed);
}


void	PhysicsSystem::accelTrackToCurrentSpeed()
{
	//zero can be considered the 'initial' speed when we started accelerating. 
	//LERP function will be sdjusted to give a result that speeds the player up QUICKER as it approaches the final speed (LERP normally slows towards goal)

	//try reciprocal LERP
	track_speed = track_speed + (1 / ((maxTrackSpeed - track_speed) * COLLISION_RECOVERY_SPEED));

	//clamp (maybe not needed?) and end accel phase
	if (track_speed > maxTrackSpeed || (maxTrackSpeed - track_speed) < 1.0f)
	{
		track_speed = maxTrackSpeed;
		playerIsAccelerating = false;
	}

}

void	PhysicsSystem::AddNode(PhysicsNode* n)
{
	allNodes.push_back(n);
}

void	PhysicsSystem::RemoveNode(PhysicsNode* n)
{
	for (std::vector<PhysicsNode*>::iterator i = allNodes.begin(); i != allNodes.end(); ++i)
	{
		if ((*i) == n)
		{
			allNodes.erase(i);
			return;
		}
	}
}


//Created by Sam 
void	PhysicsSystem::ObstacleCollisions()
{

	if (!playerIsReversing)
	{
		for (int i = 0; i < Obstacles.size(); i++)
		{
			if (Obstacles[i]->getState() == 0)
			{
				//get other phys node (saves multiple get calls
				PhysicsNode* otherObj = &(Obstacles.at(i)->GetPhysicsNode());
				//check objects are close enough to collide

				if (otherObj->GetPosition().getZ() > 0)
				{

				}
				else if (CheckAABBCollision(*playerPhysNode, *otherObj) == true)
				{
					//create the collision objects to test for an actual collision
					CollisionData* collisionData = new CollisionData();
					
					//check for collision
					
					//link the amount reversed to both track speed
					zAmountToReverse = COLLISION_BOUNCE_FACTOR * track_speed;
					//check reverse amount doesnt go off the back of the current tile (in case no previous tile)
					if (zAmountToReverse > (TILE_DEPTH - playerPhysNode->GetAABBHalfLength() - 10.0f)) //the 10.0 is just to prevent the player going right to the edge of the tile
					{
						zAmountToReverse = TILE_DEPTH - playerPhysNode->GetAABBHalfLength() - 10.0f;
					}
					//store forward track speed at moment of impact
					initialTrackSpeed = track_speed;
					//set track to be reversing
					playerIsReversing = true;
					track_speed = -track_speed;
					//zero reverse distance so far
					distanceReversed = 0.0f;
					//set track accelerating to false (collision still possible if accelerating after collision
					playerIsAccelerating = false;

				}
			}
		}

	}

	else if (playerIsReversing)
	{
		//update distance reversed
		distanceReversed += abs(track_speed);
		//check if player has reversed (0.5f tolerance in z axis as player will never reach end point with LERP
		//if (zAmountToReverse - distanceReversed < 0.5f)
		float value = 0.5 * zAmountToReverse / 100;
		if (((zAmountToReverse - distanceReversed) < value) || (zAmountToReverse - distanceReversed) < 0.5f || track_speed > -0.05f)
			//
		{
			//stop reverse
			playerIsReversing = false;
			//start accelerating back to track speed
			playerIsAccelerating = true;
			//zero the track speed
			track_speed = 0.0f;
		}
		else
		{
			//reverse track using LERP weighted average
			reverseTrackDeccel();
		}
	}
	if (playerIsAccelerating)
	{
		accelTrackToCurrentSpeed();
	}

}

void	PhysicsSystem::MissileCollisions()
{
	//having to limit the missile collisions to when not reversing
	//missile collisions will simple stop the player rather than bouncing them back
	//this is because we cannot garauntee a distance behind the player to the back edge of a tile
	//therefore a bounce could push the player off the tile. This game is already too bloody hard......

	if (!playerIsReversing)
	{
		for (int i = 0; i < Missiles.size(); i++)
		{
			//check objects are close enough to collide
			if (CheckAABBCollision(*playerPhysNode, *Missiles[i]) == true)
			{
				//create the collision objects to test for an actual collision
				
				track_speed = 0.0f;
				playerIsAccelerating = true;

				//also 'despawn' missile from track
				Missiles[i]->SetPosition(Vector3(1000.0f, 1000.0f, 1000.0f));

			}

		}

	}
}

bool	PhysicsSystem::CheckOnATile()
{
	//get current plane
	int currentPlaneIndex = playerVehicle->GetCurrentPlaneID();

	switch (currentPlaneIndex)
	{
	case 0:
		//for all tiles on that plane
		for (int i = 0; i < Plane0Tiles.size(); i++)
		{
			//check if front of vehicle is on the tile
			if ((playerPhysNode->GetPosition().getZ() > Plane0Tiles[i]->GetPosition().getZ()) &&
				(playerPhysNode->GetPosition().getZ() - playerPhysNode->GetAABBHalfLength()) < (Plane0Tiles[i]->GetPosition().getZ() + TILE_DEPTH))
			{
				//on a tile so return true as early out
				return true;
			}
			//check if back of vehicle is on the tile
			if ((playerPhysNode->GetPosition().getZ() < Plane0Tiles[i]->GetPosition().getZ()) &&
				(playerPhysNode->GetPosition().getZ() + playerPhysNode->GetAABBHalfLength()) >(Plane0Tiles[i]->GetPosition().getZ() - TILE_DEPTH))
			{
				//on a tile so return true as early out
				return true;
			}
		}
		break;

	case 1:
		//for all tiles on that plane
		for (int i = 0; i < Plane1Tiles.size(); i++)
		{
			//check if front of vehicle is on the tile
			if ((playerPhysNode->GetPosition().getZ() > Plane1Tiles[i]->GetPosition().getZ()) &&
				(playerPhysNode->GetPosition().getZ() - playerPhysNode->GetAABBHalfLength()) < (Plane1Tiles[i]->GetPosition().getZ() + TILE_DEPTH))
			{
				//on a tile so return true as early out
				return true;
			}
			//check if back of vehicle is on the tile
			if ((playerPhysNode->GetPosition().getZ() < Plane1Tiles[i]->GetPosition().getZ()) &&
				(playerPhysNode->GetPosition().getZ() + playerPhysNode->GetAABBHalfLength()) >(Plane1Tiles[i]->GetPosition().getZ() - TILE_DEPTH))
			{
				//on a tile so return true as early out
				return true;
			}
		}
		break;

	case 2:
		//for all tiles on that plane
		for (int i = 0; i < Plane2Tiles.size(); i++)
		{
			//check if front of vehicle is on the tile
			if ((playerPhysNode->GetPosition().getZ()> Plane2Tiles[i]->GetPosition().getZ()) &&
				(playerPhysNode->GetPosition().getZ() - playerPhysNode->GetAABBHalfLength()) < (Plane2Tiles[i]->GetPosition().getZ() + TILE_DEPTH))
			{
				//on a tile so return true as early out
				return true;
			}
			//check if back of vehicle is on the tile
			if ((playerPhysNode->GetPosition().getZ() < Plane2Tiles[i]->GetPosition().getZ()) &&
				(playerPhysNode->GetPosition().getZ() + playerPhysNode->GetAABBHalfLength()) >(Plane2Tiles[i]->GetPosition().getZ() - TILE_DEPTH))
			{
				//on a tile so return true as early out
				return true;
			}
		}
		break;

	case 3:
		//for all tiles on that plane
		for (int i = 0; i < Plane3Tiles.size(); i++)
		{
			//check if front of vehicle is on the tile
			if ((playerPhysNode->GetPosition().getZ() > Plane3Tiles[i]->GetPosition().getZ()) &&
				(playerPhysNode->GetPosition().getZ() - playerPhysNode->GetAABBHalfLength()) < (Plane3Tiles[i]->GetPosition().getZ() + TILE_DEPTH))
			{
				//on a tile so return true as early out
				return true;
			}
			//check if back of vehicle is on the tile
			if ((playerPhysNode->GetPosition().getZ() < Plane3Tiles[i]->GetPosition().getZ()) &&
				(playerPhysNode->GetPosition().getZ() + playerPhysNode->GetAABBHalfLength()) >(Plane3Tiles[i]->GetPosition().getZ() - TILE_DEPTH))
			{
				//on a tile so return true as early out
				return true;
			}
		}
		break;
	}

	//not on any tiles on its plane.....return false.
	return false;
}



void	PhysicsSystem::AddConstraint(Constraint* s)
{
	allSprings.push_back(s);
}

void	PhysicsSystem::RemoveConstraint(Constraint* c)
{
	for (std::vector<Constraint*>::iterator i = allSprings.begin(); i != allSprings.end(); ++i)
	{
		if ((*i) == c)
		{
			allSprings.erase(i);
			return;
		}
	}
}

void	PhysicsSystem::AddDebugDraw(DebugDrawer* d)
{
	allDebug.push_back(d);
}

void	PhysicsSystem::RemoveDebugDraw(DebugDrawer* d)
{
	for (std::vector<DebugDrawer*>::iterator i = allDebug.begin(); i != allDebug.end(); ++i)
	{
		if ((*i) == d)
		{
			allDebug.erase(i);
			return;
		}
	}
}

void    PhysicsSystem::DrawDebug()
{
	for (std::vector<DebugDrawer*>::iterator i = allDebug.begin(); i != allDebug.end(); ++i)
	{
		(*i)->DebugDraw();
	}
}

void PhysicsSystem::AddCollisionImpulse(PhysicsNode &s0, PhysicsNode &s1, CollisionData* data)
{
	if (s0.GetInverseMass() + s1.GetInverseMass() == 0.0f)
		return;

	float invMass0 = s0.GetInverseMass();
	float invMass1 = s1.GetInverseMass();

	Vector3 r0 = data->m_point - s0.GetPosition();
	Vector3 r1 = data->m_point - s1.GetPosition();

	Vector3 v0 = s0.GetLinearVelocity() + cross(s0.GetAngularVelocity(), r0);
	Vector3 v1 = s1.GetLinearVelocity() + cross(s1.GetAngularVelocity(), r1);

	Vector3 relativeVel = v0 - v1;

	float relMovement = -dot(relativeVel, data->m_normal);
	if (relMovement < 0.0001f) //value may need adjusting with scale
		return;


	//resolve linear component of collision
	{
		float colElasticity = (s0.GetElasticity() + s1.GetElasticity()) / 2;
		float normalDiv = invMass0 + invMass1 +
			dot(data->m_normal,
			cross(s1.GetInverseInertiaMatrix().getUpper3x3() * cross(r0, data->m_normal),r0) + cross(s1.GetInverseInertiaMatrix().getUpper3x3() *cross(r1, data->m_normal), r1));
		float normalImpulse = -1 * (1 + colElasticity) * dot(relativeVel, data->m_normal) / normalDiv;

		normalImpulse = normalImpulse + (data->m_penetration * 0.01f);

		Vector3 linear0 = s0.GetLinearVelocity() + data->m_normal * (normalImpulse * invMass0);
		s0.SetLinearVelocity(linear0);
		Vector3 angular0 = s0.GetAngularVelocity() + s0.GetInverseInertiaMatrix().getUpper3x3() *cross(r0, data->m_normal * normalImpulse);
		s0.SetAngularVelocity(angular0);

		Vector3 linear1 = s1.GetLinearVelocity() - data->m_normal * (normalImpulse * invMass1);
		s1.SetLinearVelocity(linear1);
		Vector3 angular1 = s1.GetAngularVelocity() - s1.GetInverseInertiaMatrix().getUpper3x3() * cross(r1, data->m_normal * normalImpulse);
		s1.SetAngularVelocity(angular1);
	}

	//resolve angular component of collision
	{
		Vector3 tangent = relativeVel - data->m_normal * dot(relativeVel, data->m_normal);
		normalize(tangent);
		float tangentDiv = (invMass0 + invMass1 +
			dot(tangent,
			cross(s0.GetInverseInertiaMatrix().getUpper3x3() * cross(r0, tangent), r0) +
			cross(s1.GetInverseInertiaMatrix().getUpper3x3() * cross(r1, tangent), r1)));

		float angularImpulse = -1 * dot(relativeVel, tangent) / tangentDiv;

		Vector3 linear0 = s0.GetLinearVelocity() + tangent * (angularImpulse * invMass0);
		s0.SetLinearVelocity(linear0);
		Vector3 angular0 = s0.GetAngularVelocity() + s0.GetInverseInertiaMatrix().getUpper3x3() * cross(r0, tangent * angularImpulse);
		s0.SetAngularVelocity(angular0);

		Vector3 linear1 = s1.GetLinearVelocity() - tangent * (angularImpulse * invMass1);
		s1.SetLinearVelocity(linear1);
		Vector3 angular1 = s1.GetAngularVelocity() - s1.GetInverseInertiaMatrix().getUpper3x3() * cross(r1, tangent * angularImpulse);
		s1.SetAngularVelocity(angular1);
	}

}

void PhysicsSystem::CalculateScore(float msec)
{

	if (!increaseScore && track_speed > 0)
	{
		distanceReversed -= track_speed;
		if (distanceReversed < 0)
		{
			score += (abs(distanceReversed) * msec) / 10000;

			increaseScore = true;
			actualScore = score * scoreMultiplier;
		}
	}
	else
	{
		if (numberOfCoins > scoreMultiplier)
		{
			scoreMultiplier++;
			numberOfCoins = 0;
		}

		score += (track_speed * msec) / 10000;

		actualScore = score * scoreMultiplier;
	}


}