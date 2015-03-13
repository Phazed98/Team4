#pragma once

#include "PhysicsNode.h"
#include "Constraint.h"
#include "DebugDrawer.h"
#include "GameClass.h"
//#include "GJKSimplex.h"
#include <vector>
#include "Vehicle.h"
#include "SpaceshipSceneNode.h"
#include "Obstacle.h"

//added by Sam for collision bouncing. Scales the forward track speed to the amount the track reverses
#define COLLISION_BOUNCE_FACTOR 20.0f

//added by Sam for updating the speed of the game over time. Larger value gives SLOWER speed increase
#define DIFFICULTY_SPEED_INCREASE 7000.0f
//added by Sam for ship acceleration LERP after collision. Higher = slower acceleration
#define COLLISION_RECOVERY_SPEED 0.2f

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

	void setImmunity()
	{
		immunityTime = 5000;
		canDie = false;
	}

	PhysicsNode* GetPlayer() { return playerPhysNode; }

	float time;//// = 0;
	int nbFrames;//// = 0;				//Integer for frame count

	static Vehicle* GetVehicle(){ return playerVehicle; }
	static float GetTrackSpeed(){ return track_speed; }
	static void  SetTrackSpeed(float s){ track_speed = s; }


	int GetCheckPointTimer(){ return checkPointTimer; }
	void  SetCheckPointTimer(int s){ checkPointTimer = s; }


	//---------------------Score--------------------------------------------
	void CalculateScore(float);
	int GetScoreMultiplier() { return scoreMultiplier; }
	int GetNumberOfCoins() { return numberOfCoins; }
	int GetScore() { return actualScore; }

	//---------------------------------------------------------------------------------

	//---------------------------------------------------------------------------------
	//getters for storage vectors - to be filled at tile/obstacle creation
	std::vector<PhysicsNode*>* GetPlane0Tiles() { return &Plane0Tiles; }
	std::vector<Obstacle*>*	GetPlane0Obstacles() { return &Plane0Obstacles; }

	std::vector<PhysicsNode*>* GetPlane1Tiles() { return &Plane1Tiles; }
	std::vector<Obstacle*>*	GetPlane1Obstacles() { return &Plane1Obstacles; }

	std::vector<PhysicsNode*>* GetPlane2Tiles() { return &Plane2Tiles; }
	std::vector<Obstacle*>*	GetPlane2Obstacles() { return &Plane2Obstacles; }

	std::vector<PhysicsNode*>* GetPlane3Tiles() { return &Plane3Tiles; }
	std::vector<Obstacle*>*	GetPlane3Obstacles() { return &Plane3Obstacles; }

	std::vector<Obstacle*>*	GetObstacles() { return &Obstacles; }
	std::vector<PhysicsNode*>* GetMissiles() { return &Missiles; }
	//---------------------------------------------------------------------------------




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

	//Basic AABB for broadphase
	bool CheckAABBCollision(PhysicsNode &n0, PhysicsNode &n1);

	//Plane 0 (top) vectors
	std::vector<PhysicsNode*> Plane0Tiles;
	std::vector<Obstacle*> Plane0Obstacles;

	//Plane 1 (right)
	std::vector<PhysicsNode*> Plane1Tiles;
	std::vector<Obstacle*> Plane1Obstacles;

	//Plane 2 (bottom)
	std::vector<PhysicsNode*> Plane2Tiles;
	std::vector<Obstacle*> Plane2Obstacles;

	//Plane 3 (left)
	std::vector<PhysicsNode*> Plane3Tiles;
	std::vector<Obstacle*> Plane3Obstacles;

	//Obstacles all planes - having issues with broadphasing via planes due to the way objects can be reassigned. 
	std::vector<Obstacle*> Obstacles;
	//missiles all planes
	std::vector<PhysicsNode*> Missiles;

	//----------------------------added by Sam--------------------------------------------------
	//added by Sam for physics - used to 'bounce' the player off of collided objects
	bool playerIsReversing;
	float zAmountToReverse;
	bool playerIsAccelerating;
	float gameSpeedFromTime;
	float distanceReversed;
	float initialTrackSpeed;

	void reverseTrackDeccel();
	void accelTrackToCurrentSpeed();
	//-------------------------------------------------------------------------------------------
	//<---------------------------------------------------->
	//steven added for control the track speed everywhere
	static float track_speed;
	int checkPointTimer;

	//------------------------added by Sam---------------------------------------------------------
	//added to allow max track speed to keep increasing with time even when reversing due to collision.
	//this is important for multiplayer so crashed players can accelerate to be the same speed after a collision.
	float maxTrackSpeed;
	//---------------------------------------------------------------------------------------------


	void		ObstacleCollisions();
	void		MissileCollisions();
	bool		CheckOnATile();
	int		immunityTime;

	bool canDie;


	//---------------------Score--------------------------------------------
	float		score;
	int			scoreMultiplier;
	int			numberOfCoins;
	int			actualScore;
	bool		increaseScore;
	//----------------------------------------------------------------------

};

