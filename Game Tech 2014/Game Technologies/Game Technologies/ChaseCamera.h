//class created by Sam, not extending the camera class as camera's functionality and variables are irrelevent here.
//If we have issues with integration we can make it a subclass for polymorphism.....

#pragma once

#include "../../nclgl/Matrix4.h"
#include "../../nclgl/Vector3.h"
#include "../../nclgl/Quaternion.h"
#include "Vehicle.h"
#include "PhysicsNode.h"


class ChaseCamera
{
public:
	ChaseCamera(Vehicle* player, float chaseDistance, float verticalOffset, float chaseLagFactorLinear);
	~ChaseCamera();

	void UpdateCamera(float msec);

	Vector3 GetFacingVector();

	Matrix4 BuildViewMatrix();

	Vector3 GetPosition() const { return cameraPosition; }

	void SetPosition(Vector3 val)	{ cameraPosition = val; }

private:

	float chaseDistance;
	float verticalOffset;
	//need two of these as the SLERP weightings need to be much smaller than LERP weightings
	float chaseLagFactorLinear;
	float chaseLagFactorRotation;

	Vector3 cameraPosition;
	Quaternion cameraRotation;


	PhysicsNode* playerPhysNode;
};