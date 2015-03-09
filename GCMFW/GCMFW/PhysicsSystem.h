#pragma once

#include "PhysicsNode.h"
#include "Constraint.h"
#include "DebugDrawer.h"
#include "GameClass.h"
//#include "GJKSimplex.h"
#include <vector>
#include "Vehicle.h"
#include "SpaceshipSceneNode.h"

//using std::vector;

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

	PhysicsNode* GetPlayer() { return playerPhysNode; }

	float time;//// = 0;
	int nbFrames;//// = 0;				//Integer for frame count

	static Vehicle* GetVehicle(){ return playerVehicle; }
	static float GetTrackSpeed(){ return track_speed; }
	static void  SetTrackSpeed(float s){ track_speed = s; }

protected:
	PhysicsSystem(void);
	~PhysicsSystem(void);


	//Statics
	static PhysicsSystem* instance;

	std::vector<PhysicsNode*> allNodes;
	std::vector<Constraint*> allSprings;
	std::vector<DebugDrawer*> allDebug;


	//<---------Added by Sam for physics--- DO NOT TRUST IT ---->
	//player physicspointer for collision checks. A list of additional interactive moving objects can be added later
	PhysicsNode* playerPhysNode; //shortcut only DO NOT DELETE IN DESTRUCTOR
	//Player vehicle pointer for updating the player via inputs - this feeds back into the physNode so should be updated first?
	static Vehicle* playerVehicle;


	//<---------Added by Sam for physics--- DO NOT TRUST IT ---->
	//player pointer for collision checks. A list of additional interactive moving objects can be added later
	PhysicsNode* player;

	//add by steven to catch it by render
	SpaceshipSceneNode* spaceship_scene_node;

	//array holds 1 trailing tile, current tile, 8x (adjust later) approaching tile
	//Each vector will then hold the static obstacles on their respective tile
	//Will need another vector if adding moving objects that are NOT on rails, eg projectiles fired at player that are physicsEnabled
	
	//std::vector<PhysicsNode*>* TilePhysicsNodeArray0 = new vector<PhysicsNode*>[10];
	//std::vector<GJKSimplex*>* TileGJKSimplexArray0 = new vector<GJKSimplex*>[10];

	//std::vector<PhysicsNode*>* TilePhysicsNodeArray1 = new vector<PhysicsNode*>[10];
	//std::vector<GJKSimplex*>* TileGJKSimplexArray1 = new vector<GJKSimplex*>[10];

	//std::vector<PhysicsNode*>* TilePhysicsNodeArray2 = new vector<PhysicsNode*>[10];
	//std::vector<GJKSimplex*>* TileGJKSimplexArray2 = new vector<GJKSimplex*>[10];

	//std::vector<PhysicsNode*>* TilePhysicsNodeArray3 = new vector<PhysicsNode*>[10];
	//std::vector<GJKSimplex*>* TileGJKSimplexArray3 = new vector<GJKSimplex*>[10];

	//Basic AABB for broadphase
	bool CheckAABBCollision(PhysicsNode &n0, PhysicsNode &n1);

	//<---------------------------------------------------->
	//steven added for control the track speed everywhere
	static float track_speed;
	int checkPointTimer;
};

