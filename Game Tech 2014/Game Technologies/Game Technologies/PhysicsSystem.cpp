#include "PhysicsSystem.h"
#include "CollisionHelper.h"
#include "GJK.h"
#include "../../nclgl/OBJMesh.h"

PhysicsSystem* PhysicsSystem::instance = 0;
int PhysicsSystem::fps = 0;
Vehicle* PhysicsSystem::playerVehicle = NULL;
float PhysicsSystem::track_speed = 15.f;


PhysicsSystem::PhysicsSystem(void)
{
	//Sam - moving here for scoping reasons
	OBJMesh* PlayerMesh = new OBJMesh(MESHDIR"SR-71_Blackbird.obj");

	//------------------------------------------------------------------------------------
	//add by steven because it should use render instance
	spaceship_scene_node = new SpaceshipSceneNode(PlayerMesh);
	
	spaceship_scene_node->SetRenderType(PLAYER_RENDER);
	//store the ssn in render to render the particle system at the end of everything
	Renderer::GetRenderer().SetSpaceshipSceneNode(spaceship_scene_node);
	//-----------------------------------------------------------------------------------------

	this->playerVehicle = new Vehicle(PlayerMesh, 3.0f, 0.0023f, 2, 0.02f, spaceship_scene_node);
	this->playerPhysNode = playerVehicle->GetPhysicsNode();
	playerPhysNode->SetLinearVelocity(Vector3(0, 0, 0));

	//Added by Sam
	maxTrackSpeed = track_speed;
	canDie = false;
	offset = 0.0f;
}

PhysicsSystem::~PhysicsSystem(void)
{

}

void	PhysicsSystem::Update(float msec)
{
	//Update FPS
	nbFrames++;
	time += msec;

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

	//if slow powerup active, half track speed
	if (playerVehicle->getSlowPowerUpActive())
	{
		track_speed *= 0.5;
	}

	if (time >= 1000)
	{
		fps = nbFrames;
		nbFrames = 0;
		time = 0;
		//canDie = true;
	}

	ObstacleCollisions();
	MissileCollisions();
	if (canDie)
	{
		if (!CheckOnATile())
		{
			SoundSystem::GetSoundSystem()->PlaySoundA(SoundManager::GetSound("../../Sounds/bomb.wav"),Vector3());
			GameClass::GetGameClass().setCurrentState(GAME_OVER);
		}
	}

	for (vector<PhysicsNode*>::iterator i = allNodes.begin(); i != allNodes.end(); ++i) 
	{
		(*i)->Update(msec);
	}

	playerVehicle->UpdatePlayer(msec);
	playerPhysNode->Update(msec);
}


void	PhysicsSystem::AddNode(PhysicsNode* n) 
{
	allNodes.push_back(n);
}

void	PhysicsSystem::RemoveNode(PhysicsNode* n) 
{
	for (vector<PhysicsNode*>::iterator i = allNodes.begin(); i != allNodes.end(); ++i) 
	{
		if ((*i) == n) 
		{
			allNodes.erase(i);
			return;
		}
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

	Vector3 v0 = s0.GetLinearVelocity() + Vector3::Cross(s0.GetAngularVelocity(), r0);
	Vector3 v1 = s1.GetLinearVelocity() + Vector3::Cross(s1.GetAngularVelocity(), r1);

	Vector3 relativeVel = v0 - v1;

	float relMovement = -Vector3::Dot(relativeVel, data->m_normal);
	if (relMovement < 0.0001f) //value may need adjusting with scale
		return;


	//resolve linear component of collision
	{
		float colElasticity = (s0.GetElasticity() + s1.GetElasticity()) / 2;
		float normalDiv = invMass0 + invMass1 +
			Vector3::Dot(data->m_normal,
			Vector3::Cross(s1.GetInverseInertiaMatrix() *
			Vector3::Cross(r0, data->m_normal), r0) +
			Vector3::Cross(s1.GetInverseInertiaMatrix() *
			Vector3::Cross(r1, data->m_normal), r1));
		float normalImpulse = -1 * (1 + colElasticity) * Vector3::Dot(relativeVel, data->m_normal) / normalDiv;

		normalImpulse = normalImpulse + (data->m_penetration * 0.01f);

		Vector3 linear0 = s0.GetLinearVelocity() + data->m_normal * (normalImpulse * invMass0);
		s0.SetLinearVelocity(linear0);
		Vector3 angular0 = s0.GetAngularVelocity() + s0.GetInverseInertiaMatrix() * Vector3::Cross(r0, data->m_normal * normalImpulse);
		s0.SetAngularVelocity(angular0);

		Vector3 linear1 = s1.GetLinearVelocity() - data->m_normal * (normalImpulse * invMass1);
		s1.SetLinearVelocity(linear1);
		Vector3 angular1 = s1.GetAngularVelocity() - s1.GetInverseInertiaMatrix() * Vector3::Cross(r1, data->m_normal * normalImpulse);
		s1.SetAngularVelocity(angular1);
	}

	//resolve angular component of collision
	{
		Vector3 tangent = relativeVel - data->m_normal * Vector3::Dot(relativeVel, data->m_normal);
		tangent.Normalise();
		float tangentDiv = (invMass0 + invMass1 +
			Vector3::Dot(tangent,
			Vector3::Cross(s0.GetInverseInertiaMatrix() * Vector3::Cross(r0, tangent), r0) +
			Vector3::Cross(s1.GetInverseInertiaMatrix() * Vector3::Cross(r1, tangent), r1)));

		float angularImpulse = -1 * Vector3::Dot(relativeVel, tangent) / tangentDiv;

		Vector3 linear0 = s0.GetLinearVelocity() + tangent * (angularImpulse * invMass0);
		s0.SetLinearVelocity(linear0);
		Vector3 angular0 = s0.GetAngularVelocity() + s0.GetInverseInertiaMatrix() * Vector3::Cross(r0, tangent * angularImpulse);
		s0.SetAngularVelocity(angular0);

		Vector3 linear1 = s1.GetLinearVelocity() - tangent * (angularImpulse * invMass1);
		s1.SetLinearVelocity(linear1);
		Vector3 angular1 = s1.GetAngularVelocity() - s1.GetInverseInertiaMatrix() * Vector3::Cross(r1, tangent * angularImpulse);
		s1.SetAngularVelocity(angular1);
	}

}


//Added by Sam - basic AABB for broadphase
bool	PhysicsSystem::CheckAABBCollision(PhysicsNode &n0, PhysicsNode &n1)
{
	float AABBSum = n0.GetAABBHalfLength() + n1.GetAABBHalfLength();
	float distance = abs(n0.GetPosition().z - n1.GetPosition().z);
	//check Z axis for early out
	if (distance < AABBSum)
	{
		float distance = abs(n0.GetPosition().x - n1.GetPosition().x);
		//next is X axis
		if (distance < AABBSum)
		{
			float distance = abs(n0.GetPosition().y - n1.GetPosition().y);
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


//Created by Sam 
void	PhysicsSystem::ObstacleCollisions()
{

	if (!playerIsReversing && !(playerVehicle->getImmunityPowerUpActive()))
	{
		for (int i = 0; i < Obstacles.size(); i++)
		{
			if (Obstacles[i]->getState() == 0)
			{
				//get other phys node (saves multiple get calls
				PhysicsNode* otherObj = &(Obstacles.at(i)->GetPhysicsNode());
				//check objects are close enough to collide
				if (CheckAABBCollision(*playerPhysNode, *otherObj) == true)
				{
					//create the collision objects to test for an actual collision
					CollisionData* collisionData = new CollisionData();
					GJK* gjkObj = new GJK();
					//check for collision
					if (gjkObj->CollisionDetection(*playerPhysNode, *otherObj, collisionData))
					{
						//bounce object if collision normal is positive in z axis
						if (collisionData->m_normal.z > 0.0f)
						{

							//check if object is a powerup
							if (Obstacles[i]->getObstacleType() == 3)
							{
								switch (Obstacles[i]->GetPowerupType())
								{
								case 1:
									if (playerVehicle->getHasSlowPowerUp() == false)
									{
										playerVehicle->setHasSlowPowerUp(true);
										Obstacles[i]->GetRidOfObstacle();
									}
									break;

								case 2:
									if (playerVehicle->getHasCDRedPowerUp() == false)
									{
										playerVehicle->setHasCDRedPowerUp(true);
										Obstacles[i]->GetRidOfObstacle();
									}
									break;

								case 3:
									if (playerVehicle->getHasImmunityPowerUp() == false)
									{
										playerVehicle->setHasImmunityPowerUp(true);
										Obstacles[i]->GetRidOfObstacle();
									}
									break;
								}
							}
							//check if a special obstacle; tornado, flamethrower, water spout
							else if (Obstacles[i]->getObstacleType() == 4)
							{
								switch (Obstacles[i]->getSubType())
								{
									//tornado
									
								case 0:
									//flames
									SoundSystem::GetSoundSystem()->PlaySoundA(SoundManager::GetSound("../../Sounds/thundercross.wav"), Vector3());

								case 1:
									//water spout 
									SoundSystem::GetSoundSystem()->PlaySoundA(SoundManager::GetSound("../../Sounds/firecross.wav"), Vector3());

								case 3:
									SoundSystem::GetSoundSystem()->PlaySoundA(SoundManager::GetSound("../../Sounds/watercross.wav"), Vector3());
									break;

								case 2:
									//do nothing
									break;
								}
							}//-------------------------------------Kostas made it----------------------
							else if (Obstacles[i]->getObstacleType() == 2 && Obstacles[i]->GetCoinCollected() == false)
							{
								numberOfCoins++;
								Obstacles[i]->SetCoinCollected(true);
								Obstacles[i]->GetPhysicsNode().SetPosition(Vector3(1000, 1000, 1000));
								Obstacles[i]->GetRidOfObstacle();

							}
							//-------------------------------------------------------------------------
							else
							{
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
								SoundSystem::GetSoundSystem()->PlaySoundA(SoundManager::GetSound("../../Sounds/collision01.wav"), Vector3());
							}
						}
					}
					delete collisionData;
					delete gjkObj;
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

	if (!playerIsReversing  && !(playerVehicle->getImmunityPowerUpActive()))
	{
		for (int i = 0; i < Missiles.size(); i++)
		{
			//check objects are close enough to collide
			if (CheckAABBCollision(*playerPhysNode, *Missiles[i]) == true)
			{
				//create the collision objects to test for an actual collision
				CollisionData* collisionData = new CollisionData();
				GJK* gjkObj = new GJK();
				//check for collision
				if (gjkObj->CollisionDetection(*playerPhysNode, *Missiles[i], collisionData))
				{
					SoundSystem::GetSoundSystem()->PlaySoundA(SoundManager::GetSound("../../Sounds/bullets.wav"), Vector3());
					//stop player if collision normal is positive in z axis
					if (collisionData->m_normal.z > 0.0f)
					{
						track_speed = 0.0f;
						playerIsAccelerating = true;

						//also 'despawn' missile from track
						Missiles[i]->SetPosition(Vector3(1000.0f, 1000.0f, 1000.0f));
					}
				}
				delete collisionData;
				delete gjkObj;
			}

		}

	}
}

bool	PhysicsSystem::CheckOnATile()
{
	//if immune, return true
	if (playerVehicle->getImmunityPowerUpActive())
	{
		return true;
	}
	//if switching plane, return true
	if (playerVehicle->getIsPlaneSwitching())
	{
		return true;
	}

	//get current plane
	int currentPlaneIndex = playerVehicle->GetCurrentPlaneID();

	switch (currentPlaneIndex)
	{
	case 0:
		//for all tiles on that plane
		for (int i = 0; i < Plane0Tiles.size(); i++)
		{
			//check if front of vehicle is on the tile
			if ((playerPhysNode->GetPosition().z > Plane0Tiles[i]->GetPosition().z) &&
				(playerPhysNode->GetPosition().z - playerPhysNode->GetAABBHalfLength()) < (Plane0Tiles[i]->GetPosition().z + TILE_DEPTH))
			{
				//on a tile so return true as early out
				return true;
			}
			//check if back of vehicle is on the tile
			if ((playerPhysNode->GetPosition().z < Plane0Tiles[i]->GetPosition().z) &&
				(playerPhysNode->GetPosition().z + playerPhysNode->GetAABBHalfLength()) >(Plane0Tiles[i]->GetPosition().z - TILE_DEPTH))
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
			if ((playerPhysNode->GetPosition().z > Plane1Tiles[i]->GetPosition().z) &&
				(playerPhysNode->GetPosition().z - playerPhysNode->GetAABBHalfLength()) < (Plane1Tiles[i]->GetPosition().z + TILE_DEPTH))
			{
				//on a tile so return true as early out
				return true;
			}
			//check if back of vehicle is on the tile
			if ((playerPhysNode->GetPosition().z < Plane1Tiles[i]->GetPosition().z) &&
				(playerPhysNode->GetPosition().z + playerPhysNode->GetAABBHalfLength()) >(Plane1Tiles[i]->GetPosition().z - TILE_DEPTH))
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
			if ((playerPhysNode->GetPosition().z > Plane2Tiles[i]->GetPosition().z) &&
				(playerPhysNode->GetPosition().z - playerPhysNode->GetAABBHalfLength()) < (Plane2Tiles[i]->GetPosition().z + TILE_DEPTH))
			{
				//on a tile so return true as early out
				return true;
			}
			//check if back of vehicle is on the tile
			if ((playerPhysNode->GetPosition().z < Plane2Tiles[i]->GetPosition().z) &&
				(playerPhysNode->GetPosition().z + playerPhysNode->GetAABBHalfLength()) >(Plane2Tiles[i]->GetPosition().z - TILE_DEPTH))
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
			if ((playerPhysNode->GetPosition().z > Plane3Tiles[i]->GetPosition().z) &&
				(playerPhysNode->GetPosition().z - playerPhysNode->GetAABBHalfLength()) < (Plane3Tiles[i]->GetPosition().z + TILE_DEPTH))
			{
				//on a tile so return true as early out
				return true;
			}
			//check if back of vehicle is on the tile
			if ((playerPhysNode->GetPosition().z < Plane3Tiles[i]->GetPosition().z) &&
				(playerPhysNode->GetPosition().z + playerPhysNode->GetAABBHalfLength()) >(Plane3Tiles[i]->GetPosition().z - TILE_DEPTH))
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
			offset -= abs(distanceReversed);
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

		offset -= track_speed;
	}


}