#include "PhysicsSystem.h"
#include "CollisionHelper.h"
//#include "GJK.h"

PhysicsSystem* PhysicsSystem::instance = 0;
int PhysicsSystem::fps = 0;

PhysicsSystem::PhysicsSystem(void)
{
	time = 0;
	nbFrames = 0;
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

	if (time >= 1000)
	{
		fps = nbFrames;
		nbFrames = 0;
		time = 0;
	}


	//BroadPhaseCollisions();
	//NarrowPhaseCollisions();

	for (std::vector<Constraint*>::iterator i = allSprings.begin(); i != allSprings.end(); ++i)
	{
		(*i)->Update(msec);
	}

	for (std::vector<PhysicsNode*>::iterator i = allNodes.begin(); i != allNodes.end(); ++i)
	{
		(*i)->Update(msec);
	}
}




//Original rubbish. Delete later
//void	PhysicsSystem::NarrowPhaseCollisions()
//{
//	for (int i = 0; i < allNodes.size(); i++)
//	{
//		PhysicsNode& first = *allNodes[i];
//		for (int j = i + 1; j < allNodes.size(); j++)
//		{
//			PhysicsNode& second = *allNodes[j];
//
//
//			CollisionData* collisionData = new CollisionData();
//			GJK* gjkObj = new GJK();
//
//
//			if (gjkObj->CollisionDetection(first, second, collisionData))
//			{
//
//			}
//
//			delete gjkObj; 
//			delete collisionData;
//		}
//	}
//}


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
	float distance = n0.GetPosition().getZ() - n1.GetPosition().getZ();
	//check Z axis for early out
	if (distance < AABBSum)
	{
		float distance = n0.GetPosition().getX() - n1.GetPosition().getX();
		//next is X axis
		if (distance < AABBSum)
		{
			float distance = n0.GetPosition().getY() - n1.GetPosition().getY();
			//Y axis last
			if (distance < AABBSum)
			{
				return true;
			}
		}
	}
	return false;
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