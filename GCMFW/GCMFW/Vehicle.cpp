#include"Vehicle.h"
#include "MathHelper.h"
#include "SpaceshipSceneNode.h"


#define CONTROLLER_ROTATION_TOLERENCE 0.02f

//Altered by Sam - I've just changed this so it can take data in as parameters, 
//in case we want to have a selection of vehicles or something, and for multiplayer
//Also going to pass the obj as a parameter to speed it up. 
Vehicle::Vehicle(Mesh* mesh, float playerScale, float speedTurn, int startingPlaneID, float steeringResponsiveness, SpaceshipSceneNode* ssn) :
Speed_Turn(speedTurn), currentPlaneID(startingPlaneID), steeringResponsiveness(steeringResponsiveness), spaceship_scene_node(ssn)
{
	//initialise variables;
	rotationOnPlane = 0;
	xLocationOnPlane = 0;
	controllerIsConnected = false;
	coolingDownPlaneSwitch = false;
	isSwitchingPlane = false;

	isSwitchingPlane;
	planeSwitchTime = 1000;
	planeSwitchProgress = 0;

	BuildPlayerEntity(mesh, playerScale, CalculatePlayerLocation(), CalculateStartingOrientation());

	//set plane switch cooldown period. Add a parameter later if needed
	PlaneSwitchCDTime = 1500;
	temp = 4;
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
	Vector4 rotVec = MathHelper::vec3ToEuler(orient.getX(), orient.getY(), orient.getZ());
	PhysNode->SetOrientation(Quat(rotVec.getX(), rotVec.getY(), rotVec.getZ(), rotVec.getW()));

	this->shipWidth = PhysNode->GetAABBHalfLength();
	this->tileShipWidthDelta = TILE_WIDTH/2 - shipWidth;
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

	UpdatePlayerRotationOnPlane();

	GetUserInput();

	if (isSwitchingPlane)
	{
		//update spline progress
		planeSwitchProgress += msec;
		//calculate normalised progress between 0.0 - 1.0
		float normProgress = planeSwitchProgress / planeSwitchTime;
		//check if transition complete
		if (normProgress >= 1.0f)
		{
			PhysNode->SetPosition(Vector3(endPoint.getX(), endPoint.getY(), 0));
			UpdatePlayerVelocity();
			isSwitchingPlane = false;
		}
		else
		{
			//I'm applying increased rotational velocity as the rotation was not finishing in time with the transition
			PhysNode->SetAngularVelocity((PhysNode->GetAngularVelocity()) * 2);

			//calculate position on spline
			Vector3 xyPos = BezierMath::calculate2DQuadBezierPoint(normProgress, startPoint, Vector3(0, 0,0), endPoint);
			//set position on spline
			PhysNode->SetPosition(Vector3(xyPos.getX(), xyPos.getY(), 0));
		}
	}
	else
	{
		UpdatePlayerVelocity();
	}

	//if (temp != currentPlaneID)
	//{
	//	if (currentPlaneID == 0){
	//		SoundManager::PlaySound(0,0);
	//	}
	//	if (currentPlaneID == 1){
	//		SoundManager::PlaySound(0,0);
	//	}
	//	if (currentPlaneID == 2){
	//		SoundManager::PlaySound(0,0);
	//	}
	//	if (currentPlaneID == 3){
	//		SoundManager::PlaySound(0,0);
	//	}
	//	temp = currentPlaneID;
	//}

}

//Added by Sam to calculate the vehicle location on the current plane it is occupying
Vector3 Vehicle::CalculatePlayerLocation()
{
	Vector3 position = Vector3(0, 0, 0);

	switch (currentPlaneID)
	{
		//top plane
	case 0:
		position.setX(xLocationOnPlane);
		position.setY(SURFACE_CENTRE_OFFSET);
		break;

		//right plane
	case 1:
		position.setX(SURFACE_CENTRE_OFFSET);
		position.setY(xLocationOnPlane);
		break;

		//bottom plane
	case 2:
		position.setX(xLocationOnPlane);
		position.setY(-SURFACE_CENTRE_OFFSET);
		break;

		//left plane
	case 3:
		position.setX(-SURFACE_CENTRE_OFFSET);
		position.setY(-xLocationOnPlane);
		break;
	}
	return position;
}

void Vehicle::UpdatePlayerVelocity()
{
	//calculate this frames linear velocity derived from the current rotation
	float xVel = -rotationOnPlane * 0.05f;



	//get current velocity
	Vector3 vel = PhysNode->GetLinearVelocity();

	//using the planeID work out weighted new velocity
	switch (currentPlaneID)
	{
		//top plane
	case 0:
		//work out weighted new velocity
		vel.setX((vel.getX() * (1 - steeringResponsiveness)) + (-xVel * steeringResponsiveness));
		PhysNode->SetLinearVelocity(vel);
		break;

		//right plane
	case 1:
		//work out weighted new velocity
		vel.setY((vel.getY() * (1 - steeringResponsiveness)) + (xVel * steeringResponsiveness));
		PhysNode->SetLinearVelocity(vel);
		break;

		//bottom plane
	case 2:
		//work out weighted new velocity
		vel.setX((vel.getX() * (1 - steeringResponsiveness)) + (xVel * steeringResponsiveness));
		PhysNode->SetLinearVelocity(vel);
		break;

		//left plane
	case 3:
		//work out weighted new velocity
		vel.setY((vel.getY() * (1 - steeringResponsiveness)) + (-xVel * steeringResponsiveness));
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
		if (vel.getX() < 0)
		{
			//if edge of plane
			if (PhysNode->GetPosition().getX() <= -tileShipWidthDelta)
			{
				//stop movement
				vel.setX(0);
				//clamp position just in case 
				PhysNode->SetPosition(Vector3(-tileShipWidthDelta, currentPos.getY(), currentPos.getZ()));
			}
		}
		//if going right
		if (vel.getX() > 0)
		{
			//if edge of plane
			if (PhysNode->GetPosition().getX() >= tileShipWidthDelta)
			{
				//stop movement
				vel.setX(0);
				//clamp position just in case 
				PhysNode->SetPosition(Vector3(tileShipWidthDelta, currentPos.getY(), currentPos.getZ()));
			}
		}
		break;

		//left or right plane
	case 1:
	case 3:
		//if going down
		if (vel.getY() < 0)
		{
			//if edge of plane
			if (PhysNode->GetPosition().getY() <= -tileShipWidthDelta)
			{
				//stop movement
				vel.setX(0);
				//clamp position just in case 
				PhysNode->SetPosition(Vector3(currentPos.getX(), -tileShipWidthDelta, currentPos.getZ()));
			}
		}
		//if going right
		if (vel.getY() > 0)
		{
			//if edge of plane
			if (PhysNode->GetPosition().getY() >= tileShipWidthDelta)
			{
				//stop movement
				vel.setX(0);
				//clamp position just in case 
				PhysNode->SetPosition(Vector3(currentPos.getX(), tileShipWidthDelta, currentPos.getZ()));
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
		rotation.setZ(rotation.getZ() + 180);
		break;

		//right plane
	case 1:
		//rotate additional 270 degrees
		rotation.setZ(rotation.getZ() + 90);
		break;

		//bottom plane
	case 2:
		//rotation is fine as is
		break;

		//left plane
	case 3:
		//rotate 90 degrees
		rotation.setZ(rotation.getZ()+270);
		break;
	}

	return rotation;
}

void Vehicle::UpdatePlayerRotationOnPlane()
{
	//Matrix4 rotMat = PhysNode->GetOrientation().ToMatrix();
	Matrix4 rotMat= Matrix4::rotation(PhysNode->GetOrientation());

	Vector3 upVec = Vector3(0, 1, 0);

	Vector3 rotVec = MathHelper::applyMat3RotToVec3(rotMat, upVec);

	float zAngle = MathHelper::get360AngleBetweenTwoVecs(upVec, rotVec);

	Vector3 rotation = Vector3(0, 0, zAngle);

	switch (currentPlaneID)
	{
		//top plane
	case 0:
		rotation.setZ(rotation.getZ() + 180);
		if (rotation.getZ() > 180)
		{
			rotation.setZ(rotation.getZ() - 360);
		}

		break;

		//right plane
	case 1:
		rotation.setZ(rotation.getZ() + 90);
		break;

		//bottom plane
	case 2:
		break;

		//left plane
	case 3:
		//rotation.z -= 180;
		rotation.setZ(rotation.getZ() - 90);
		break;

	}


	rotationOnPlane = -rotation.getZ();


	//correct for going >360 or <-360 degrees
	if (rotationOnPlane <= -360)
	{
		rotationOnPlane += 360.0;
	}
	else if (!(rotationOnPlane <= 360))
	{
		rotationOnPlane -= 360.0f;
	}
}

void Vehicle::debug()
{
	/*if (Window::GetKeyboard()->KeyDown(KEYBOARD_B))
	{
		int breakHere = 0;
		breakHere++;
	}*/
}

void Vehicle::GetSwitchPlaneInputs(float normalisedRX, float normalisedRY)
{

//	Input::UpdateJoypad();
	//update xLocationOnPlane
	switch (currentPlaneID)
	{
	case 0:
	case 2:
		xLocationOnPlane = PhysNode->GetPosition().getX();
		break;
	case 1:
	case 3:
		xLocationOnPlane = PhysNode->GetPosition().getY();
		break;
	}

	Vector3 currentPos = PhysNode->GetPosition();
	//go to plane 180 degrees from the current
	if ((Input::ButtonDown(INPUT_UP) || normalisedRY > 0.5) && !coolingDownPlaneSwitch)
	{
		Vector3 EulerOrientation;
		//set new position then change the plane of current velocity (maintain in a new direction relative to new plane
		Vector3 currentVelocity = PhysNode->GetLinearVelocity();
		switch (currentPlaneID)
		{
		case 0:
			//set start location
			startPoint = Vector3(PhysNode->GetPosition().getX(), PhysNode->GetPosition().getY(),0);
			//move player to relative position on plane
			endPoint = Vector3(-xLocationOnPlane, -SURFACE_CENTRE_OFFSET,0);
			//set relevant variables;
			isSwitchingPlane = true;
			planeSwitchProgress = 0;
			//set velocity to 0 as will follow the spline
			PhysNode->SetLinearVelocity(Vector3(0, 0, 0));
			//change currentPlaneID
			currentPlaneID = 2;
			break;

		case 1:
			//set start location
			startPoint = Vector3(PhysNode->GetPosition().getX(), PhysNode->GetPosition().getY(),0);
			//move player to relative position on plane
			endPoint = Vector3(-SURFACE_CENTRE_OFFSET, -xLocationOnPlane,0);
			//set relevant variables;
			isSwitchingPlane = true;
			planeSwitchProgress = 0;
			//set velocity to 0 as will follow the spline
			PhysNode->SetLinearVelocity(Vector3(0, 0, 0));
			//change currentPlaneID
			currentPlaneID = 3;
			break;

		case 2:
			//set start location
			startPoint = Vector3(PhysNode->GetPosition().getX(), PhysNode->GetPosition().getY(),0);
			//move player to relative position on plane
			endPoint = Vector3(-xLocationOnPlane, SURFACE_CENTRE_OFFSET,0);
			//set relevant variables;
			isSwitchingPlane = true;
			planeSwitchProgress = 0;
			//set velocity to 0 as will follow the spline
			PhysNode->SetLinearVelocity(Vector3(0, 0, 0));
			//change currentPlaneID
			currentPlaneID = 0;
			break;

		case 3:
			//set start location
			startPoint = Vector3(PhysNode->GetPosition().getX(), PhysNode->GetPosition().getY(),0);
			//move player to relative position on plane
			endPoint = Vector3(SURFACE_CENTRE_OFFSET, -xLocationOnPlane,0);
			//set relevant variables;
			isSwitchingPlane = true;
			planeSwitchProgress = 0;
			//set velocity to 0 as will follow the spline
			PhysNode->SetLinearVelocity(Vector3(0, 0, 0));
			//change currentPlaneID
			currentPlaneID = 1;
			break;

		}
		//set cooldown
		coolingDownPlaneSwitch = true;
		PlaneSwitchCDRemaining = PlaneSwitchCDTime;
	}

	if ((Input::ButtonDown(INPUT_RIGHT) || normalisedRX > 0.5) && !coolingDownPlaneSwitch)
	{
		Vector3 EulerOrientation;
		//set new position then change the plane of current velocity (maintain in a new direction relative to new plane
		Vector3 currentVelocity = PhysNode->GetLinearVelocity();
		switch (currentPlaneID)
		{
		case 0:
			//set start location
			startPoint = Vector3(PhysNode->GetPosition().getX(), PhysNode->GetPosition().getY(),0);
			//move player to relative position on plane
			endPoint = Vector3(-SURFACE_CENTRE_OFFSET, xLocationOnPlane,0);
			//set relevant variables;
			isSwitchingPlane = true;
			planeSwitchProgress = 0;
			//set velocity to 0 as will follow the spline
			PhysNode->SetLinearVelocity(Vector3(0, 0, 0));
			//change currentPlaneID
			currentPlaneID = 3;
			break;

		case 1:
			//set start location
			startPoint = Vector3(PhysNode->GetPosition().getX(), PhysNode->GetPosition().getY(),0);
			//move player to relative position on plane
			endPoint = Vector3(-xLocationOnPlane, SURFACE_CENTRE_OFFSET,0);
			//set relevant variables;
			isSwitchingPlane = true;
			planeSwitchProgress = 0;
			//set velocity to 0 as will follow the spline
			PhysNode->SetLinearVelocity(Vector3(0, 0, 0));
			//change currentPlaneID
			currentPlaneID = 0;
			break;

		case 2:
			//set start location
			startPoint = Vector3(PhysNode->GetPosition().getX(), PhysNode->GetPosition().getY(),0);
			//move player to relative position on plane
			endPoint = Vector3(SURFACE_CENTRE_OFFSET, xLocationOnPlane,0);
			//set relevant variables;
			isSwitchingPlane = true;
			planeSwitchProgress = 0;
			//set velocity to 0 as will follow the spline
			PhysNode->SetLinearVelocity(Vector3(0, 0, 0));
			//change currentPlaneID
			currentPlaneID = 1;
			break;

		case 3:
			//set start location
			startPoint = Vector3(PhysNode->GetPosition().getX(), PhysNode->GetPosition().getY(),0);
			//move player to relative position on plane
			endPoint = Vector3(-xLocationOnPlane, -SURFACE_CENTRE_OFFSET,0);
			//set relevant variables;
			isSwitchingPlane = true;
			planeSwitchProgress = 0;
			//set velocity to 0 as will follow the spline
			PhysNode->SetLinearVelocity(Vector3(0, 0, 0));
			//change currentPlaneID
			currentPlaneID = 2;
			break;
		}

		//set cooldown
		coolingDownPlaneSwitch = true;
		PlaneSwitchCDRemaining = PlaneSwitchCDTime;
	}

	if ((Input::ButtonDown(INPUT_LEFT) || normalisedRX < -0.5) && !coolingDownPlaneSwitch)
	{
		Vector3 EulerOrientation;
		//change the plane of current velocity (maintain in a new direction relative to new plane
		Vector3 currentVelocity = PhysNode->GetLinearVelocity();
		switch (currentPlaneID)
		{
		case 0:
			//set start location
			startPoint = Vector3(PhysNode->GetPosition().getX(), PhysNode->GetPosition().getY(),0);
			//move player to relative position on plane
			endPoint = Vector3(SURFACE_CENTRE_OFFSET, -xLocationOnPlane,0);
			//set relevant variables;
			isSwitchingPlane = true;
			planeSwitchProgress = 0;
			//set velocity to 0 as will follow the spline
			PhysNode->SetLinearVelocity(Vector3(0, 0, 0));
			//change currentPlaneID
			currentPlaneID = 1;
			break;

		case 1:
			//set start location
			startPoint = Vector3(PhysNode->GetPosition().getX(), PhysNode->GetPosition().getY(),0);
			//move player to relative position on plane
			endPoint = Vector3(xLocationOnPlane, -SURFACE_CENTRE_OFFSET,0);
			//set relevant variables;
			isSwitchingPlane = true;
			planeSwitchProgress = 0;
			//set velocity to 0 as will follow the spline
			PhysNode->SetLinearVelocity(Vector3(0, 0, 0));
			//change currentPlaneID
			currentPlaneID = 2;
			break;

		case 2:
			//set start location
			startPoint = Vector3(PhysNode->GetPosition().getX(), PhysNode->GetPosition().getY(),0);
			//move player to relative position on plane
			endPoint = Vector3(-SURFACE_CENTRE_OFFSET, -xLocationOnPlane,0);
			//set relevant variables;
			isSwitchingPlane = true;
			planeSwitchProgress = 0;
			//set velocity to 0 as will follow the spline
			PhysNode->SetLinearVelocity(Vector3(0, 0, 0));
			//change currentPlaneID
			currentPlaneID = 3;
			break;

		case 3:
			//set start location
			startPoint = Vector3(PhysNode->GetPosition().getX(), PhysNode->GetPosition().getY(),0);
			//move player to relative position on plane
			endPoint = Vector3(xLocationOnPlane, SURFACE_CENTRE_OFFSET,0);
			//set relevant variables;
			isSwitchingPlane = true;
			planeSwitchProgress = 0;
			//set velocity to 0 as will follow the spline
			PhysNode->SetLinearVelocity(Vector3(0, 0, 0));
			//change currentPlaneID
			currentPlaneID = 0;
			break;
		}

		//set cooldown
		coolingDownPlaneSwitch = true;
		PlaneSwitchCDRemaining = PlaneSwitchCDTime;
	}

}

void Vehicle::GetUserInput()
{
	//normalised rotation variable needed for advanced controller analogue input
	float normRotOnPlane = abs(rotationOnPlane) / MAX_VEHICLE_X_ROTATION;

	//update controller connected status - saves multiple calls from this class (it would be a lot!!!)
	//controllerIsConnected = Window::GetControllerConnected();

	//get controller state
	float LX = 0, normalisedLX = 0;
	float RX = 0, RY = 0, normalisedRX = 0, normalisedRY = 0;
	//XINPUT_STATE controllerState = Window::GetController()->GetState();

	//adding some variables for LX with a tolerence level, trying to prevent stuttering on controller input
	float LXHigh = normalisedLX + CONTROLLER_ROTATION_TOLERENCE;
	float LXLow = normalisedLX - CONTROLLER_ROTATION_TOLERENCE;

	if (!isSwitchingPlane)
	{
		//switch plane if keybinds/controller pressed accordingly
		GetSwitchPlaneInputs(normalisedRX, normalisedRY);
	}


	float x, y;
	Input::GetJoypadMovement(x, y);
	//Statement to return the vehicle to flat rotation after A or D are released. 
	if (abs(x) < 0.25f)
//	if (!(Input::ButtonDown(INPUT_SQUARE)) && !(Input::ButtonDown(INPUT_CIRCLE)))
	//if (!(Input::ButtonDown(INPUT_SQUARE)) && !(Input::ButtonDown(INPUT_CIRCLE)))
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
	if (x>0)
//	if (Input::ButtonDown(INPUT_CIRCLE))
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

	if (x<0)	
//	if (Input::ButtonDown(INPUT_SQUARE))
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
}