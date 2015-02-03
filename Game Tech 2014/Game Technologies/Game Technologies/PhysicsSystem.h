/******************************************************************************
Class:PhysicsSystem
Implements:
Author:Rich Davison	<richard.davison4@newcastle.ac.uk>, Callum Rhodes <c.g.rhodes@ncl.ac.uk> and YOU!
Description: A very simple physics engine class, within which to implement the
material introduced in the Game Technologies module. This is just a rough
skeleton of how the material could be integrated into the existing codebase -
it is still entirely up to you how the specifics should work. Now C++ and
graphics are out of the way, you should be starting to get a feel for class
structures, and how to communicate data between systems.

It is worth poinitng out that the PhysicsSystem is constructed and destructed
manually using static functions. Why? Well, we probably only want a single
physics system to control the entire state of our game world, so why allow
multiple systems to be made? So instead, the constructor / destructor are
hidden, and we 'get' a single instance of a physics system with a getter.
This is known as a 'singleton' design pattern, and some developers don't like
it - but for systems that it doesn't really make sense to have multiples of,
it is fine!

-_-_-_-_-_-_-_,------,
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""

*//////////////////////////////////////////////////////////////////////////////


#pragma once

#include "PhysicsNode.h"
#include "Constraint.h"
#include "DebugDrawer.h"
#include <vector>

using std::vector;

//struct GJKStorage
//{
//	PhysicsSystem otherObj;
//
//	GJKStorage()
//	{
//
//	}
//};

class PhysicsSystem	{
public:
	friend class GameClass;

	void		Update(float msec);

	void		BroadPhaseCollisions();
	void		NarrowPhaseCollisions();

	static void AddCollisionImpulse(PhysicsNode &s0, PhysicsNode &s1, CollisionData* data);

	//Statics
	static void Initialise() {
		instance = new PhysicsSystem();
	}

	static void Destroy() {
		delete instance;
	}

	static PhysicsSystem& GetPhysicsSystem() {
		return *instance;
	}

	void	AddNode(PhysicsNode* n);

	void	RemoveNode(PhysicsNode* n);

	void	AddConstraint(Constraint* c);

	void	RemoveConstraint(Constraint* c);

	void	AddDebugDraw(DebugDrawer* d);

	void	RemoveDebugDraw(DebugDrawer* d);

	void    DrawDebug();

protected:
	PhysicsSystem(void);
	~PhysicsSystem(void);


	//Statics
	static PhysicsSystem* instance;

	vector<PhysicsNode*> allNodes;
	vector<Constraint*> allSprings;
	vector<DebugDrawer*> allDebug;
};

