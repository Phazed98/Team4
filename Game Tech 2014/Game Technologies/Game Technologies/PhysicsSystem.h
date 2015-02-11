#pragma once

#include "PhysicsNode.h"
#include "Constraint.h"
#include "DebugDrawer.h"
#include "GameClass.h"
#include "GJKSimplex.h"
#include <vector>

using std::vector;

class PhysicsSystem	{
public:
	friend class GameClass;

	void		Update(float msec);

	void		BroadPhaseCollisions();
	void		NarrowPhaseCollisions();


	static int fps;
	static int getFPS()				{ return fps; }

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

	float time = 0;
	int nbFrames = 0;				//Integer for frame count

protected:
	PhysicsSystem(void);
	~PhysicsSystem(void);


	//Statics
	static PhysicsSystem* instance;

	vector<PhysicsNode*> allNodes;
	vector<Constraint*> allSprings;
	vector<DebugDrawer*> allDebug;



	//<---------Added by Sam for physics--- DO NOT TRUST IT ---->
	//player pointer for collision checks. A list of additional interactive moving objects can be added later
	PhysicsNode* player;

	//array holds 1 trailing tile, current tile, 8x (adjust later) approaching tile
	//Each vector will then hold the static obstacles on their respective tile
	//Will need another vector if adding moving objects that are NOT on rails, eg projectiles fired at player that are physicsEnabled
	vector<PhysicsNode*>* TilePhysicsNodeArray0 = new vector<PhysicsNode*>[10];
	vector<GJKSimplex*>* TileGJKSimplexArray0 = new vector<GJKSimplex*>[10];

	vector<PhysicsNode*>* TilePhysicsNodeArray1 = new vector<PhysicsNode*>[10];
	vector<GJKSimplex*>* TileGJKSimplexArray1 = new vector<GJKSimplex*>[10];

	vector<PhysicsNode*>* TilePhysicsNodeArray2 = new vector<PhysicsNode*>[10];
	vector<GJKSimplex*>* TileGJKSimplexArray2 = new vector<GJKSimplex*>[10];

	vector<PhysicsNode*>* TilePhysicsNodeArray3 = new vector<PhysicsNode*>[10];
	vector<GJKSimplex*>* TileGJKSimplexArray3 = new vector<GJKSimplex*>[10];

	//Basic AABB for broadphase
	bool CheckAABBCollision(PhysicsNode &n0, PhysicsNode &n1);

	//<---------------------------------------------------->

};

