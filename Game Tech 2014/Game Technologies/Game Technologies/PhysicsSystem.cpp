#include "PhysicsSystem.h"
#include "CollisionHelper.h"
#include "GJK.h"

PhysicsSystem* PhysicsSystem::instance = 0;

PhysicsSystem::PhysicsSystem(void)
{

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
	BroadPhaseCollisions();
	//NarrowPhaseCollisions();

	for (vector<Constraint*>::iterator i = allSprings.begin(); i != allSprings.end(); ++i) 
	{
		(*i)->Update(msec);
	}

	for (vector<PhysicsNode*>::iterator i = allNodes.begin(); i != allNodes.end(); ++i) 
	{
		(*i)->Update(msec);
	}
}

void	PhysicsSystem::BroadPhaseCollisions() 
{

}

void	PhysicsSystem::NarrowPhaseCollisions()
{
	for (int i = 0; i < allNodes.size(); i++)
	{
		PhysicsNode& first = *allNodes[i];
		for (int j = i + 1; j < allNodes.size(); j++)
		{
			PhysicsNode& second = *allNodes[j];


			CollisionData* collisionData = new CollisionData();
			GJK* gjkObj = new GJK();


			if (gjkObj->CollisionDetection(first, second, collisionData))
			{

			}

			delete gjkObj; 
			delete collisionData;
		}
	}
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

void	PhysicsSystem::AddConstraint(Constraint* s) 
{
	allSprings.push_back(s);
}

void	PhysicsSystem::RemoveConstraint(Constraint* c)
{
	for (vector<Constraint*>::iterator i = allSprings.begin(); i != allSprings.end(); ++i) 
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
	for (vector<DebugDrawer*>::iterator i = allDebug.begin(); i != allDebug.end(); ++i) 
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
	for (vector<DebugDrawer*>::iterator i = allDebug.begin(); i != allDebug.end(); ++i) 
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