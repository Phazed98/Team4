#include "ChaseCamera.h"

ChaseCamera::ChaseCamera(Vehicle* player, float chaseDistance, float verticalOffset, float chaseLagFactorLinear) :
chaseDistance(chaseDistance), verticalOffset(verticalOffset), chaseLagFactorLinear(chaseLagFactorLinear)
{
	playerPhysNode = player->GetPhysicsNode();
	cameraRotation = Quaternion();

	chaseLagFactorRotation = (chaseLagFactorLinear / 1000) * 1.5;
}

ChaseCamera::~ChaseCamera()
{
	//dont delete stuff....its done elsewhere
}

void ChaseCamera::UpdateCamera(float msec)
{
	//get player position
	Vector3 playerPos = playerPhysNode->GetPosition();
	//get chase position
	Vector3 chasePos = playerPos;
	chasePos.z += chaseDistance; //plus as opengl
	//LERP position
	cameraPosition.getWeightedAverage(chasePos, chaseLagFactorLinear);

	//get orientation
	Quaternion playerQuat = playerPhysNode->GetOrientation();

	playerQuat.invert();

	//SLERP quats
	cameraRotation = Quaternion::SLERP(cameraRotation, playerQuat, chaseLagFactorRotation);
}

//copied from camera class
Vector3 ChaseCamera::GetFacingVector()
{
	Matrix4 viewMatrix = BuildViewMatrix();

	Vector3 faceVector(viewMatrix.values[2], viewMatrix.values[6], viewMatrix.values[10]);

	return faceVector;
}

Matrix4 ChaseCamera::BuildViewMatrix()
{
	//return rotationMatrix * Matrix4::Translation(-cameraPosition); //negated because just becuase.
	Matrix4 rotationMatrix = cameraRotation.ToMatrix();

	return rotationMatrix * Matrix4::Translation(-cameraPosition);
}

