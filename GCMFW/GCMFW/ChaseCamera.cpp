#include "ChaseCamera.h"
#include "MathHelper.h"

ChaseCamera::ChaseCamera(Vehicle* player, float chaseDistance, float verticalOffset, float chaseLagFactorLinear) :
chaseDistance(chaseDistance), verticalOffset(verticalOffset), chaseLagFactorLinear(chaseLagFactorLinear)
{
	playerPhysNode = player->GetPhysicsNode();
	cameraRotation = Quat();

	chaseLagFactorRotation = (chaseLagFactorLinear / 1000) * 1.5f;
}

ChaseCamera::~ChaseCamera()
{
	//dont delete stuff....its done elsewhere
}

void ChaseCamera::Update(float msec)
{
	//get player position
	Vector3 playerPos = playerPhysNode->GetPosition();
	
	//get chase position
	Vector3 chasePos = playerPos;
	chasePos.setZ(chasePos.getZ() + chaseDistance);
	
	//LERP position
	cameraPosition = MathHelper::getWeightedAverage(chasePos, cameraPosition, chaseLagFactorLinear);

	//get orientation
	Quat playerQuat = playerPhysNode->GetOrientation();

	playerQuat = MathHelper::invert(playerQuat);
	
	//SLERP quats
	cameraRotation = slerp(chaseLagFactorRotation, cameraRotation, playerQuat);

}



//copied from camera class
Vector3 ChaseCamera::GetFacingVector()
{
	Matrix4 viewMatrix = BuildViewMatrix();

	//Vector3 faceVector(viewMatrix.values[2], viewMatrix.values[6], viewMatrix.values[10]);


	return viewMatrix.getUpper3x3().getCol2();
}

Matrix4 ChaseCamera::BuildViewMatrix()
{
	Matrix4 rotationMatrix = Matrix4::rotation(cameraRotation);
	return rotationMatrix * Matrix4::translation(-cameraPosition);
}

