#include"Vehicle.h"

//Altered by Sam - I've just changed this so it can take data in as parameters, 
//in case we want to have a selection of vehicles or something, and for multiplayer
//Also going to pass the obj as a parameter to speed it up. 
Vehicle::Vehicle(Mesh* mesh, float playerScale, float speedTurn, int startingPlaneID, float steeringResponsiveness, SpaceshipSceneNode* ssn) :
Speed_Turn(speedTurn), currentPlaneID(startingPlaneID), steeringResponsiveness(steeringResponsiveness), spaceship_scene_node(ssn)
{
	//initialise variables;
	rotationOnPlane = 0;
	xLocationOnPlane = 0;


	BuildPlayerEntity(mesh, playerScale, CalculatePlayerLocation(), CalculateStartingOrientation());

	//set plane switch cooldown period. Add a parameter later if needed
	PlaneSwitchCDTime = 500;
}

Vehicle::~Vehicle(void)
{
	delete PhysNode;
}

//Sam - made some changes in here to bring in line with the rest of the game objects
void Vehicle::BuildPlayerEntity(Mesh* mesh, float size, Vector3 pos, Vector3 orient)
{
	spaceship_scene_node->SetModelScale(Vector3(size, size, size));

	spaceship_scene_node->SetColour(Vector4(1.0f, 1.0f, 0.0f, 1.0f));

	//Connect just the renderNode to systems as phys node will be called seperately
	GameEntity*g = new GameEntity(spaceship_scene_node);
	g->ConnectToSystems();


	//create the physics node locally, do not connect to systems
	PhysNode = new PhysicsNode();
	PhysNode->SetUseGravity(false);
	PhysNode->SetPosition(pos);
	PhysNode->SetTarget(spaceship_scene_node);
	PhysNode->ConfigureAABBHalfLength();
	PhysNode->SetMovable(true);
	PhysNode->SetOrientation(Quaternion::EulerAnglesToQuaternion(orient.x, orient.y, orient.z));

	this->shipWidth = PhysNode->GetAABBHalfLength();
	this->tileShipWidthDelta = TILE_WIDTH - shipWidth;
}

//Modified by Sam to fit into game
void Vehicle::UpdatePlayer(float msec)
{
	//update cooldowns
	if (coolingDownPlaneSwitch)
	{
		PlaneSwitchCDRemaining -= msec;
		if (PlaneSwitchCDRemaining <= 0)
		{
			coolingDownPlaneSwitch = false;
		}
	}

	//update xLocationOnPlane
	switch (currentPlaneID)
	{
	case 0:
	case 2:
		xLocationOnPlane = PhysNode->GetPosition().x;
		break;
	case 1:
	case 3:
		xLocationOnPlane = PhysNode->GetPosition().y;
		break;
	}

	SwitchPlane();

	UpdatePlayerRotationOnPlane();

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_L))
	{
		//rotate vehicle if not at max rotation
		if (rotationOnPlane > -MAX_VEHICLE_X_ROTATION)
		{
			PhysNode->SetAngularVelocity(Vector3(0, 0, -Speed_Turn));
		}
		//else clamp
		else
		{
			PhysNode->SetAngularVelocity(Vector3(0, 0, 0));
		}
	}

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_J))
	{
		//rotate vehicle if not at max rotation
		if (rotationOnPlane < MAX_VEHICLE_X_ROTATION)
		{
			PhysNode->SetAngularVelocity(Vector3(0, 0, Speed_Turn));
		}
		//else clamp
		else
		{
			PhysNode->SetAngularVelocity(Vector3(0, 0, 0));
		}
	}

	//Statement to return the vehicle to flat rotation after A or D are released. 
	if (!Window::GetKeyboard()->KeyDown(KEYBOARD_J) && !Window::GetKeyboard()->KeyDown(KEYBOARD_L))
	{
		if (rotationOnPlane < -0.02)
		{
			PhysNode->SetAngularVelocity(Vector3(0, 0, Speed_Turn));
		}
		else if (rotationOnPlane > 0.02)
		{
			PhysNode->SetAngularVelocity(Vector3(0, 0, -Speed_Turn));
		}
		else
		{
			PhysNode->SetAngularVelocity(Vector3(0, 0, 0));
		}
	}




	UpdatePlayerVelocity();
}




//Added by Sam to calculate the vehicle location on the current plane it is occupying
Vector3 Vehicle::CalculatePlayerLocation()
{
	Vector3 position = Vector3(0, 0, 0);

	switch (currentPlaneID)
	{
		//top plane
	case 0:
		position.x = xLocationOnPlane;
		position.y = SURFACE_CENTRE_OFFSET;
		break;

		//right plane
	case 1:
		position.x = SURFACE_CENTRE_OFFSET;
		position.y = xLocationOnPlane;
		break;

		//bottom plane
	case 2:
		position.x = xLocationOnPlane;
		position.y = -SURFACE_CENTRE_OFFSET;
		break;

		//left plane
	case 3:
		position.x = -SURFACE_CENTRE_OFFSET;
		position.y = -xLocationOnPlane;
		break;
	}
	return position;
}

void Vehicle::UpdatePlayerVelocity()
{
	//calculate this frames linear velocity derived from the current rotation
	float xVel = -rotationOnPlane * 0.05;



	//get current velocity
	Vector3 vel = PhysNode->GetLinearVelocity();

	//using the planeID work out weighted new velocity
	switch (currentPlaneID)
	{
		//top plane
	case 0:
		//work out weighted new velocity
		vel.x = (vel.x * (1 - steeringResponsiveness)) + (-xVel * steeringResponsiveness);
		PhysNode->SetLinearVelocity(vel);
		break;

		//right plane
	case 1:
		//work out weighted new velocity
		vel.y = (vel.y * (1 - steeringResponsiveness)) + (xVel * steeringResponsiveness);
		PhysNode->SetLinearVelocity(vel);
		break;

		//bottom plane
	case 2:
		//work out weighted new velocity
		vel.x = (vel.x * (1 - steeringResponsiveness)) + (xVel * steeringResponsiveness);
		PhysNode->SetLinearVelocity(vel);
		break;

		//left plane
	case 3:
		//work out weighted new velocity
		vel.y = (vel.y * (1 - steeringResponsiveness)) + (-xVel * steeringResponsiveness);
		PhysNode->SetLinearVelocity(vel);
		break;
	}

	//store position locally
	Vector3 currentPos = PhysNode->GetPosition();

	//account for walls at the edge of the planes
	switch (currentPlaneID)
	{
		//top and bottom plane
	case 0:
	case 2:
		//if going left
		if (vel.x < 0)
		{
			//if edge of plane
			if (PhysNode->GetPosition().x <= -tileShipWidthDelta)
			{
				//stop movement
				vel.x = 0;
				//clamp position just in case 
				PhysNode->SetPosition(Vector3(-tileShipWidthDelta, currentPos.y, currentPos.z));
			}
		}
		//if going right
		if (vel.x > 0)
		{
			//if edge of plane
			if (PhysNode->GetPosition().x >= tileShipWidthDelta)
			{
				//stop movement
				vel.x = 0;
				//clamp position just in case 
				PhysNode->SetPosition(Vector3(tileShipWidthDelta, currentPos.y, currentPos.z));
			}
		}
		break;

		//left or right plane
	case 1:
	case 3:
		//if going down
		if (vel.y < 0)
		{
			//if edge of plane
			if (PhysNode->GetPosition().y <= -tileShipWidthDelta)
			{
				//stop movement
				vel.x = 0;
				//clamp position just in case 
				PhysNode->SetPosition(Vector3(currentPos.x, -tileShipWidthDelta, currentPos.z));
			}
		}
		//if going right
		if (vel.y > 0)
		{
			//if edge of plane
			if (PhysNode->GetPosition().y >= tileShipWidthDelta)
			{
				//stop movement
				vel.x = 0;
				//clamp position just in case 
				PhysNode->SetPosition(Vector3(currentPos.x, tileShipWidthDelta, currentPos.z));
			}
		}

		break;
	}
}

Vector3 Vehicle::CalculateStartingOrientation()
{
	Vector3 rotation = Vector3(0, 0, 0);
	//using the planeID update the z rotation
	switch (currentPlaneID)
	{
		//top plane
	case 0:
		//rotate additional 180 degrees
		rotation.z += 180;
		break;

		//right plane
	case 1:
		//rotate additional 270 degrees
		rotation.z += 90;
		break;

		//bottom plane
	case 2:
		//rotation is fine as is
		break;

		//left plane
	case 3:
		//rotate 90 degrees
		rotation.z += 270;
		break;
	}

	return rotation;
}

void Vehicle::UpdatePlayerRotationOnPlane()
{

	Matrix4 rotMat = PhysNode->GetOrientation().ToMatrix();

	Vector3 upVec = Vector3(0, 1, 0);

	Vector3 rotVec = rotMat.applyMat3RotToVec3(upVec);

	float zAngle = Vector3::get360AngleBetweenTwoVecs(upVec, rotVec);

	Vector3 rotation = Vector3(0, 0, zAngle);

	switch (currentPlaneID)
	{
		//top plane
	case 0:
		rotation.z += 180;
		if (rotation.z > 180)
		{
			rotation.z -= 360;
		}

		break;

		//right plane
	case 1:
		rotation.z -= 90;
		break;

		//bottom plane
	case 2:
		break;

		//left plane
	case 3:
		//rotation.z -= 180;
		rotation.z += 90;
		break;

	}

	rotationOnPlane = rotation.z;


	//correct for going >360 or <-360 degrees
	if (rotationOnPlane <= -360)
	{
		rotationOnPlane += 360.0;
	}
	else if (!(rotationOnPlane <= 360))
	{
		rotationOnPlane -= 360.0f;
	}
	cout << rotationOnPlane << endl;
}

void Vehicle::debug()
{
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_P))
	{
		int breakHere = 0;
		breakHere++;
	}
}

void Vehicle::SwitchPlane()
{
	//Vector3 currentPos = PhysNode->GetPosition();
	//if (Window::GetKeyboard()->KeyDown(KEYBOARD_UP) && currentPlaneID != 0)
	//{
	//	//set new position then change the plane of current velocity (maintain in a new direction relative to new plane
	//	Vector3 currentVelocity = PhysNode->GetLinearVelocity();
	//	switch (currentPlaneID)
	//	{
	//	case 1:
	//		//move player to relative position on plane
	//		PhysNode->SetPosition(Vector3(-xLocationOnPlane, SURFACE_CENTRE_OFFSET, 0));
	//		//flip both plane and direction of movement
	//		PhysNode->SetLinearVelocity(Vector3(-currentVelocity.y, 0, 0));
	//		break;

	//	case 2:
	//		//move player to relative position on plane
	//		PhysNode->SetPosition(Vector3(-xLocationOnPlane, SURFACE_CENTRE_OFFSET, 0));
	//		//flip only direction of movement
	//		PhysNode->SetLinearVelocity(Vector3(-currentVelocity.x, 0, 0));
	//		break;

	//	case 3:
	//		//move player to relative position on plane
	//		PhysNode->SetPosition(Vector3(xLocationOnPlane, SURFACE_CENTRE_OFFSET, 0));
	//		//flip only plane of movement
	//		PhysNode->SetLinearVelocity(Vector3(currentVelocity.y, 0, 0));
	//		break;
	//	}

	//	//correct rotation of ship
	//	Vector3 EulerOrientation = Vector3(0, 0, 180);
	//	EulerOrientation.z += rotationOnPlane;
	//	PhysNode->SetOrientation(
	//		Quaternion::EulerAnglesToQuaternion(EulerOrientation.x, EulerOrientation.y, EulerOrientation.z));

	//	//change currentPlaneID
	//	currentPlaneID = 0;
	//}

	//if (Window::GetKeyboard()->KeyDown(KEYBOARD_RIGHT) && currentPlaneID != 1)
	//{
	//	//set new position then change the plane of current velocity (maintain in a new direction relative to new plane
	//	Vector3 currentVelocity = PhysNode->GetLinearVelocity();
	//	switch (currentPlaneID)
	//	{
	//	case 0:
	//		//move player to relative position on plane
	//		PhysNode->SetPosition(Vector3(SURFACE_CENTRE_OFFSET, -xLocationOnPlane, 0));
	//		//flip both plane and direction of movement
	//		PhysNode->SetLinearVelocity(Vector3(0, -currentVelocity.x, 0));
	//		break;

	//	case 2:
	//		//move player to relative position on plane
	//		PhysNode->SetPosition(Vector3(SURFACE_CENTRE_OFFSET, xLocationOnPlane, 0));
	//		//flip only plane of movement
	//		PhysNode->SetLinearVelocity(Vector3(0, currentVelocity.x, 0));
	//		break;

	//	case 3:
	//		//move player to relative position on plane
	//		PhysNode->SetPosition(Vector3(SURFACE_CENTRE_OFFSET, -xLocationOnPlane, 0));
	//		//flip only direction of movement
	//		PhysNode->SetLinearVelocity(Vector3(0, -currentVelocity.y, 0));
	//		break;
	//	}

	//	//correct rotation of ship
	//	Vector3 EulerOrientation = Vector3(0, 0, 90);
	//	EulerOrientation.z += rotationOnPlane;
	//	PhysNode->SetOrientation(
	//		Quaternion::EulerAnglesToQuaternion(EulerOrientation.x, EulerOrientation.y, EulerOrientation.z));

	//	//change currentPlaneID
	//	currentPlaneID = 1;
	//}

	//if (Window::GetKeyboard()->KeyDown(KEYBOARD_DOWN) && currentPlaneID != 2)
	//{
	//	//change the plane of current velocity (maintain in a new direction relative to new plane
	//	Vector3 currentVelocity = PhysNode->GetLinearVelocity();
	//	switch (currentPlaneID)
	//	{
	//	case 0:
	//		//move player to relative position on plane
	//		PhysNode->SetPosition(Vector3(-xLocationOnPlane, -SURFACE_CENTRE_OFFSET, 0));
	//		//flip only direction of movement
	//		PhysNode->SetLinearVelocity(Vector3(-currentVelocity.x, 0, 0));
	//		break;

	//	case 1:
	//		//move player to relative position on plane
	//		PhysNode->SetPosition(Vector3(xLocationOnPlane, -SURFACE_CENTRE_OFFSET, 0));
	//		//flip only plane of movement
	//		PhysNode->SetLinearVelocity(Vector3(currentVelocity.y, 0, 0));
	//		break;

	//	case 3:
	//		//move player to relative position on plane
	//		PhysNode->SetPosition(Vector3(-xLocationOnPlane, -SURFACE_CENTRE_OFFSET, 0));
	//		//flip plane and direction of movement
	//		PhysNode->SetLinearVelocity(Vector3(-currentVelocity.y, 0, 0));
	//		break;
	//	}

	//	//correct rotation of ship
	//	Vector3 EulerOrientation = Vector3(0, 0, 0);
	//	EulerOrientation.z += rotationOnPlane;
	//	PhysNode->SetOrientation(
	//		Quaternion::EulerAnglesToQuaternion(EulerOrientation.x, EulerOrientation.y, EulerOrientation.z));

	//	//change currentPlaneID
	//	currentPlaneID = 2;
	//}

	//if (Window::GetKeyboard()->KeyDown(KEYBOARD_LEFT) && currentPlaneID != 3)
	//{
	//	//change the plane of current velocity (maintain in a new direction relative to new plane
	//	Vector3 currentVelocity = PhysNode->GetLinearVelocity();
	//	switch (currentPlaneID)
	//	{
	//	case 0:
	//		//move player to relative position on plane
	//		PhysNode->SetPosition(Vector3(-SURFACE_CENTRE_OFFSET, xLocationOnPlane, 0));
	//		//flip only plane of movement
	//		PhysNode->SetLinearVelocity(Vector3(0, currentVelocity.x, 0));
	//		break;

	//	case 1:
	//		//move player to relative position on plane
	//		PhysNode->SetPosition(Vector3(-SURFACE_CENTRE_OFFSET, -xLocationOnPlane, 0));
	//		//flip only direction of movement
	//		PhysNode->SetLinearVelocity(Vector3(0, -currentVelocity.y, 0));
	//		break;

	//	case 2:
	//		//move player to relative position on plane
	//		PhysNode->SetPosition(Vector3(-SURFACE_CENTRE_OFFSET, -xLocationOnPlane, 0));
	//		//flip plane and direction of movement
	//		PhysNode->SetLinearVelocity(Vector3(0, -currentVelocity.x, 0));
	//		break;
	//	}

	//	//correct rotation of ship
	//	Vector3 EulerOrientation = Vector3(0, 0, 270);
	//	EulerOrientation.z += rotationOnPlane;
	//	PhysNode->SetOrientation(
	//		Quaternion::EulerAnglesToQuaternion(EulerOrientation.x, EulerOrientation.y, EulerOrientation.z));

	//	//change currentPlaneID
	//	currentPlaneID = 3;
	//}


	Vector3 currentPos = PhysNode->GetPosition();

	//go to plane 180 degrees from the current
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_UP) && !coolingDownPlaneSwitch)
	{
		Vector3 EulerOrientation;
		//set new position then change the plane of current velocity (maintain in a new direction relative to new plane
		Vector3 currentVelocity = PhysNode->GetLinearVelocity();
		switch (currentPlaneID)
		{
		case 0:
			//move player to relative position on plane
			PhysNode->SetPosition(Vector3(-xLocationOnPlane, -SURFACE_CENTRE_OFFSET, 0));
			//flip only direction of movement
			PhysNode->SetLinearVelocity(Vector3(-currentVelocity.x, 0, 0));

			//correct rotation of ship
			EulerOrientation = Vector3(0, 0, 0);
			EulerOrientation.z += rotationOnPlane;
			PhysNode->SetOrientation(Quaternion::EulerAnglesToQuaternion(EulerOrientation.x, EulerOrientation.y, EulerOrientation.z));

			//change currentPlaneID
			currentPlaneID = 2;
			break;

		case 1:
			//move player to relative position on plane
			PhysNode->SetPosition(Vector3(-SURFACE_CENTRE_OFFSET, -xLocationOnPlane, 0));
			//flip only direction of movement
			PhysNode->SetLinearVelocity(Vector3(0, -currentVelocity.y, 0));

			//correct rotation of ship
			EulerOrientation = Vector3(0, 0, 270);
			EulerOrientation.z += rotationOnPlane;
			PhysNode->SetOrientation(Quaternion::EulerAnglesToQuaternion(EulerOrientation.x, EulerOrientation.y, EulerOrientation.z));

			//change currentPlaneID
			currentPlaneID = 3;
			break;

		case 2:
			//move player to relative position on plane
			PhysNode->SetPosition(Vector3(-xLocationOnPlane, SURFACE_CENTRE_OFFSET, 0));
			//flip only direction of movement
			PhysNode->SetLinearVelocity(Vector3(-currentVelocity.x, 0, 0));

			//correct rotation of ship
			EulerOrientation = Vector3(0, 0, 180);
			EulerOrientation.z += rotationOnPlane;
			PhysNode->SetOrientation(Quaternion::EulerAnglesToQuaternion(EulerOrientation.x, EulerOrientation.y, EulerOrientation.z));

			//change currentPlaneID
			currentPlaneID = 0;
			break;

		case 3:
			//move player to relative position on plane
			PhysNode->SetPosition(Vector3(SURFACE_CENTRE_OFFSET, -xLocationOnPlane, 0));
			//flip only direction of movement
			PhysNode->SetLinearVelocity(Vector3(0, -currentVelocity.y, 0));

			//correct rotation of ship
			Vector3 EulerOrientation = Vector3(0, 0, 90);
			EulerOrientation.z += rotationOnPlane;
			PhysNode->SetOrientation(Quaternion::EulerAnglesToQuaternion(EulerOrientation.x, EulerOrientation.y, EulerOrientation.z));

			//change currentPlaneID
			currentPlaneID = 1;
			break;
		}

		//set cooldown
		coolingDownPlaneSwitch = true;
		PlaneSwitchCDRemaining = PlaneSwitchCDTime;

	}

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_RIGHT) && !coolingDownPlaneSwitch)
	{
		Vector3 EulerOrientation;
		//set new position then change the plane of current velocity (maintain in a new direction relative to new plane
		Vector3 currentVelocity = PhysNode->GetLinearVelocity();
		switch (currentPlaneID)
		{
		case 0:
			//move player to relative position on plane
			PhysNode->SetPosition(Vector3(-SURFACE_CENTRE_OFFSET, xLocationOnPlane, 0));
			//flip only plane of movement
			PhysNode->SetLinearVelocity(Vector3(0, currentVelocity.x, 0));

			//correct rotation of ship
			EulerOrientation = Vector3(0, 0, 270);
			EulerOrientation.z += rotationOnPlane;
			PhysNode->SetOrientation(Quaternion::EulerAnglesToQuaternion(EulerOrientation.x, EulerOrientation.y, EulerOrientation.z));

			//change currentPlaneID
			currentPlaneID = 3;
			break;

		case 1:
			//move player to relative position on plane
			PhysNode->SetPosition(Vector3(-xLocationOnPlane, SURFACE_CENTRE_OFFSET, 0));
			//flip both plane and direction of movement
			PhysNode->SetLinearVelocity(Vector3(-currentVelocity.y, 0, 0));

			//correct rotation of ship
			EulerOrientation = Vector3(0, 0, 180);
			EulerOrientation.z += rotationOnPlane;
			PhysNode->SetOrientation(Quaternion::EulerAnglesToQuaternion(EulerOrientation.x, EulerOrientation.y, EulerOrientation.z));

			//change currentPlaneID
			currentPlaneID = 0;
			break;

		case 2:
			//move player to relative position on plane
			PhysNode->SetPosition(Vector3(SURFACE_CENTRE_OFFSET, xLocationOnPlane, 0));
			//flip only plane of movement
			PhysNode->SetLinearVelocity(Vector3(0, currentVelocity.x, 0));

			//correct rotation of ship
			EulerOrientation = Vector3(0, 0, 90);
			EulerOrientation.z += rotationOnPlane;
			PhysNode->SetOrientation(Quaternion::EulerAnglesToQuaternion(EulerOrientation.x, EulerOrientation.y, EulerOrientation.z));

			//change currentPlaneID
			currentPlaneID = 1;
			break;

		case 3:
			//move player to relative position on plane
			PhysNode->SetPosition(Vector3(-xLocationOnPlane, -SURFACE_CENTRE_OFFSET, 0));
			//flip plane and direction of movement
			PhysNode->SetLinearVelocity(Vector3(-currentVelocity.y, 0, 0));

			//correct rotation of ship
			EulerOrientation = Vector3(0, 0, 0);
			EulerOrientation.z += rotationOnPlane;
			PhysNode->SetOrientation(Quaternion::EulerAnglesToQuaternion(EulerOrientation.x, EulerOrientation.y, EulerOrientation.z));

			//change currentPlaneID
			currentPlaneID = 2;
			break;
		}

		//set cooldown
		coolingDownPlaneSwitch = true;
		PlaneSwitchCDRemaining = PlaneSwitchCDTime;
	}

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_LEFT) && !coolingDownPlaneSwitch)
	{
		Vector3 EulerOrientation;
		//change the plane of current velocity (maintain in a new direction relative to new plane
		Vector3 currentVelocity = PhysNode->GetLinearVelocity();
		switch (currentPlaneID)
		{
		case 0:
			//move player to relative position on plane
			PhysNode->SetPosition(Vector3(SURFACE_CENTRE_OFFSET, -xLocationOnPlane, 0));
			//flip both plane and direction of movement
			PhysNode->SetLinearVelocity(Vector3(0, -currentVelocity.x, 0));


			//correct rotation of ship
			EulerOrientation = Vector3(0, 0, 90);
			EulerOrientation.z += rotationOnPlane;
			PhysNode->SetOrientation(Quaternion::EulerAnglesToQuaternion(EulerOrientation.x, EulerOrientation.y, EulerOrientation.z));

			//change currentPlaneID
			currentPlaneID = 1;
			break;

		case 1:
			//move player to relative position on plane
			PhysNode->SetPosition(Vector3(xLocationOnPlane, -SURFACE_CENTRE_OFFSET, 0));
			//flip only plane of movement
			PhysNode->SetLinearVelocity(Vector3(currentVelocity.y, 0, 0));

			//correct rotation of ship
			EulerOrientation = Vector3(0, 0, 0);
			EulerOrientation.z += rotationOnPlane;
			PhysNode->SetOrientation(Quaternion::EulerAnglesToQuaternion(EulerOrientation.x, EulerOrientation.y, EulerOrientation.z));

			//change currentPlaneID
			currentPlaneID = 2;
			break;

		case 2:
			//move player to relative position on plane
			PhysNode->SetPosition(Vector3(-SURFACE_CENTRE_OFFSET, -xLocationOnPlane, 0));
			//flip plane and direction of movement
			PhysNode->SetLinearVelocity(Vector3(0, -currentVelocity.x, 0));

			//correct rotation of ship
			EulerOrientation = Vector3(0, 0, 270);
			EulerOrientation.z += rotationOnPlane;
			PhysNode->SetOrientation(Quaternion::EulerAnglesToQuaternion(EulerOrientation.x, EulerOrientation.y, EulerOrientation.z));

			//change currentPlaneID
			currentPlaneID = 3;
			break;

		case 3:
			//move player to relative position on plane
			PhysNode->SetPosition(Vector3(xLocationOnPlane, SURFACE_CENTRE_OFFSET, 0));
			//flip only plane of movement
			PhysNode->SetLinearVelocity(Vector3(currentVelocity.y, 0, 0));

			//correct rotation of ship
			EulerOrientation = Vector3(0, 0, 180);
			EulerOrientation.z += rotationOnPlane;
			PhysNode->SetOrientation(Quaternion::EulerAnglesToQuaternion(EulerOrientation.x, EulerOrientation.y, EulerOrientation.z));

			//change currentPlaneID
			currentPlaneID = 0;
			break;
		}

		//set cooldown
		coolingDownPlaneSwitch = true;
		PlaneSwitchCDRemaining = PlaneSwitchCDTime;
	}
}