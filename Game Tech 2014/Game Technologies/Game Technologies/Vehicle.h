#pragma once
#include"GameEntity.h"

//Written by Sam, adapted from Daixi's vehicle code to allow the vehicle to retain its behaviour over all planes, as well as adding improved hover mechanics and plane switching


//Sam - added a min/max rotation define (negative for min)
#define MAX_VEHICLE_X_ROTATION 55.0f

//Sam - added planeInfo for positioning in game
#include "PlaneInfo.h"
#include "SpaceshipSceneNode.h"

class Vehicle
{
public:
	//Sam - made some alterations to the constructor to bring it more in line with other objects for consistency
	Vehicle(Mesh* mesh, float playerScale, float speedTurn, int startingPlaneID, float steeringResponsiveness, SpaceshipSceneNode* ssn);
	~Vehicle(void);

	void UpdatePlayer(float msec);

	Vector3 tempPosition;

	//Sam - added a way to get the physics node for the physics system
	PhysicsNode* GetPhysicsNode() { return PhysNode; }

	int GetCurrentPlaneID() { return currentPlaneID; }

protected:


	void BuildPlayerEntity(Mesh* mesh, float size, Vector3 pos, Vector3 orient);

	void UpdatePlayerVelocity();

	void SwitchPlane();

	void UpdatePlayerRotationOnPlane();

	Vector3 CalculateStartingOrientation();



	PhysicsNode* PhysNode;

	float Speed_Turn;

	//Added by Sam for physics management and player placement
	int currentPlaneID;
	Vector3 CalculatePlayerLocation();
	float rotationOnPlane; //a rotation relative to the current plane for the if comparisons 
	float xLocationOnPlane; //player's x location on the current plane

	//weighting value of how quickly the hover vehicle will react to direction changes (lower = more inertia)
	//MUST BE <=1.0
	float steeringResponsiveness;


	float shipWidth;
	float tileShipWidthDelta;

	bool coolingDownPlaneSwitch = false;
	float PlaneSwitchCDTime;
	float PlaneSwitchCDRemaining;


	//Debug method, doesnt do anything, only for triggering breaks when needed.
	void debug();

	SpaceshipSceneNode* spaceship_scene_node;

};