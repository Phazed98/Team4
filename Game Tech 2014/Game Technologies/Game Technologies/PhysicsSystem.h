#pragma once

#include "PhysicsNode.h"
#include "Constraint.h"
#include "DebugDrawer.h"
#include "GameClass.h"
#include "GJKSimplex.h"
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

	PhysicsNode* GetPlayer() { return playerPhysNode; }

	float time = 0;
	int nbFrames = 0;				//Integer for frame count

	static Vehicle* GetVehicle(){ return playerVehicle; }
	static float GetTrackSpeed(){ return track_speed; }
	static void  SetTrackSpeed(float s){ track_speed = s; }

	//---------------------CheckPoint Timer--------------------------------------------
	int GetCheckPointTimer(){ return checkPointTimer; }
	void  SetCheckPointTimer(int s){ checkPointTimer = s; }

	//---------------------Score--------------------------------------------
	void CalculateScore(float);
	int GetScoreMultiplier() { return scoreMultiplier; }
	int GetNumberOfCoins() { return numberOfCoins; }
	int GetScore() { return score; }

	//---------------------------------------------------------------------------------


	//---------------------------------------------------------------------------------
	//getters for storage vectors - to be filled at tile/obstacle creation
	vector<PhysicsNode*>* GetPlane0Tiles() { return &Plane0Tiles; }
	vector<Obstacle*>*	GetPlane0Obstacles() { return &Plane0Obstacles; }

	vector<PhysicsNode*>* GetPlane1Tiles() { return &Plane1Tiles; }
	vector<Obstacle*>*	GetPlane1Obstacles() { return &Plane1Obstacles; }

	vector<PhysicsNode*>* GetPlane2Tiles() { return &Plane2Tiles; }
	vector<Obstacle*>*	GetPlane2Obstacles() { return &Plane2Obstacles; }

	vector<PhysicsNode*>* GetPlane3Tiles() { return &Plane3Tiles; }
	vector<Obstacle*>*	GetPlane3Obstacles() { return &Plane3Obstacles; }

	vector<Obstacle*>*	GetObstacles() { return &Obstacles; }
	vector<PhysicsNode*>* GetMissiles() { return &Missiles; }
	//---------------------------------------------------------------------------------

protected:
	PhysicsSystem(void);
	~PhysicsSystem(void);


	//Statics
	static PhysicsSystem* instance;

	vector<PhysicsNode*> allNodes;
	//vector<DebugDrawer*> allDebug;



	//<---------Added by Sam for physics--- DO NOT TRUST IT ---->
	//player physicspointer for collision checks. A list of additional interactive moving objects can be added later
	PhysicsNode* playerPhysNode; //shortcut only DO NOT DELETE IN DESTRUCTOR
	//Player vehicle pointer for updating the player via inputs - this feeds back into the physNode so should be updated first?
	static Vehicle* playerVehicle;

	////--------------------------------------------------------------------------------------------------------------
	///*each tile only has one obstacle, so the 'broadphase' sorted lists can simply repressent each plane's tile list
	//then another list for the same plane will hold obstacles on those planes.
	//Any 'gaps' in the list can simply be repressented by a NULL pointer which the physics algorithm can then check for.
	//*/

	////Plane 0 (top) arrays
	//static PhysicsNode*		Plane0Tiles[MAX_NUM_TILES_PER_PLANE];
	//static Obstacle*			Plane0Obstacles[MAX_NUM_TILES_PER_PLANE];
	//static GJKSimplex*		Plane0ObstacleGJKs[MAX_NUM_TILES_PER_PLANE];

	////Plane 1 (right) arrays
	//static PhysicsNode*		Plane1Tiles[MAX_NUM_TILES_PER_PLANE];
	//static Obstacle*			Plane1Obstacles[MAX_NUM_TILES_PER_PLANE];
	//static GJKSimplex*		Plane1ObstacleGJKs[MAX_NUM_TILES_PER_PLANE];

	////Plane 2 (bottom) arrays
	//static PhysicsNode*		Plane2Tiles[MAX_NUM_TILES_PER_PLANE];
	//static Obstacle*			Plane2Obstacles[MAX_NUM_TILES_PER_PLANE];
	//static GJKSimplex*		Plane2ObstacleGJKs[MAX_NUM_TILES_PER_PLANE];

	////Plane 3 (left) arrays
	//static PhysicsNode*		Plane3Tiles[MAX_NUM_TILES_PER_PLANE];
	//static Obstacle*			Plane3Obstacles[MAX_NUM_TILES_PER_PLANE];
	//static GJKSimplex*		Plane3ObstacleGJKs[MAX_NUM_TILES_PER_PLANE];

	//static int currentTileIndex; //repressents the current tile index under the front of the vehicle
	////--------------------------------------------------------------------------------------------------------------



	//--------------------------------------------------------------------------------------------------------------------------------------------
	/*the original plan was to use a form of sorted list (arrays) for the physics which acted as a broadphase, unfortunately
	due to the way objects are created this is not possible. When we have a 'gap' in the track a tile is created but put into a holding state.
	In order for the list system to work I would need a 'NULL' pointer assigned to the array at the index whenever no track is generated, whilst there
	is probably a way of processing this with the current track generation algorithm, it is not a straightforward solution. Given time contraints
	(physics has had to be left until the end of the project so we can ensure track generation and networking work together), I am opting for a simple
	iterater through a non-sorted list (std::vector). This will still be quick as it is a worst case O(n) complexity of number of tiles per plane which is
	very small, but bear in mind the solution could be even quicker if we had time to alter the track generation
	*/

	//Plane 0 (top) vectors
	vector<PhysicsNode*> Plane0Tiles;
	vector<Obstacle*> Plane0Obstacles;

	//Plane 1 (right)
	vector<PhysicsNode*> Plane1Tiles;
	vector<Obstacle*> Plane1Obstacles;

	//Plane 2 (bottom)
	vector<PhysicsNode*> Plane2Tiles;
	vector<Obstacle*> Plane2Obstacles;

	//Plane 3 (left)
	vector<PhysicsNode*> Plane3Tiles;
	vector<Obstacle*> Plane3Obstacles;

	//Obstacles all planes - having issues with broadphasing via planes due to the way objects can be reassigned. 
	vector<Obstacle*> Obstacles;
	//missiles all planes
	vector<PhysicsNode*> Missiles;

	//----------------------------------------------------------------------------------------------------------------------------------------------


	//Basic AABB for broadphase
	bool CheckAABBCollision(PhysicsNode &n0, PhysicsNode &n1);





	//<-----------------added by steven--------------------------->
	//add by steven to catch it by render
	SpaceshipSceneNode* spaceship_scene_node;

	//steven added for control the track speed everywhere
	static float track_speed;

	int checkPointTimer;
	//<------------------------------------------------------------>



	//----------------------------added by Sam--------------------------------------------------
	//added by Sam for physics - used to 'bounce' the player off of collided objects
	bool playerIsReversing = false;
	float zAmountToReverse;
	bool playerIsAccelerating = false;
	float gameSpeedFromTime = 0.0f;
	float distanceReversed;
	float initialTrackSpeed;

	void reverseTrackDeccel();
	void accelTrackToCurrentSpeed();
	//-------------------------------------------------------------------------------------------



	//------------------------added by Sam---------------------------------------------------------
	//added to allow max track speed to keep increasing with time even when reversing due to collision.
	//this is important for multiplayer so crashed players can accelerate to be the same speed after a collision.
	float maxTrackSpeed;
	//---------------------------------------------------------------------------------------------


	void		ObstacleCollisions();
	void		MissileCollisions();
	bool		CheckOnATile();

	bool canDie;

	//---------------------Score--------------------------------------------
	int			score = 0;
	int			scoreMultiplier = 1;
	int			numberOfCoins = 0;
	int			scoreTimer = 0;
};

