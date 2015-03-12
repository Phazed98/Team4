#include"Vehicle.h"

#define CONTROLLER_ROTATION_TOLERENCE 0.02f

//Altered by Sam - I've just changed this so it can take data in as parameters, 
//in case we want to have a selection of vehicles or something, and for multiplayer
//Also going to pass the obj as a parameter to speed it up. 
Vehicle::Vehicle(Mesh* mesh, float playerScale, float speedTurn, int startingPlaneID, float steeringResponsiveness, SpaceshipSceneNode* ssn) :
Speed_Turn(speedTurn), currentPlaneID(startingPlaneID), steeringResponsiveness(steeringResponsiveness), spaceship_scene_node(ssn), previousPlaneID(startingPlaneID)
{
	//initialise variables;
	rotationOnPlane = 0;
	xLocationOnPlane = 0;
	controllerIsConnected = false;

	BuildPlayerEntity(mesh, playerScale, CalculatePlayerLocation(), CalculateStartingOrientation());

	//set plane switch cooldown period. Add a parameter later if needed
	PlaneSwitchCDTime = 1500;

	soundTemp = 4;
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
	updateCooldowns(msec);

	UpdatePlayerRotationOnPlane();

	GetUserInput();

	if (isSwitchingPlane)
	{
		//update spline progress
		planeSwitchProgress += msec;
		//calculate normalised progress between 0.0 - 1.0
		normalisedPlaneSwitchProgress = planeSwitchProgress / planeSwitchTime;
		//check if transition complete
		if (normalisedPlaneSwitchProgress >= 1.0f)
		{
			normalisedPlaneSwitchProgress = 1.0f;
			PhysNode->SetPosition(Vector3(endPoint.x, endPoint.y, 0));
			UpdatePlayerVelocity();
			isSwitchingPlane = false;
		}
		else
		{
			//I'm applying increased rotational velocity as the rotation was not finishing in time with the transition
			PhysNode->SetAngularVelocity((PhysNode->GetAngularVelocity()) * 2);

			//calculate position on spline
			Vector2 xyPos = BezierMath::calculate2DQuadBezierPoint(normalisedPlaneSwitchProgress, startPoint, Vector2(0, 0), endPoint);
			//set position on spline
			PhysNode->SetPosition(Vector3(xyPos.x, xyPos.y, 0));
		}
	}
	else
	{
		UpdatePlayerVelocity();
	}

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
	float xVel = -rotationOnPlane * 0.05f;



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
	
	if (soundTemp != currentPlaneID)
	{
		if (currentPlaneID == 0)
		{
			SoundSystem::GetSoundSystem()->ChangeSceneSound(SoundManager::GetSound("../../Sounds/wind.wav"));
		}
		if (currentPlaneID == 1)
		{
			SoundSystem::GetSoundSystem()->ChangeSceneSound(SoundManager::GetSound("../../Sounds/void.wav"));
		}
		if (currentPlaneID == 2)
		{
			SoundSystem::GetSoundSystem()->ChangeSceneSound(SoundManager::GetSound("../../Sounds/thunder.wav"));
		}
		if (currentPlaneID == 3)
		{
			SoundSystem::GetSoundSystem()->ChangeSceneSound(SoundManager::GetSound("../../Sounds/water.wav"));
		}
		soundTemp = currentPlaneID;
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
}

void Vehicle::debug()
{
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_B))
	{
		int breakHere = 0;
		breakHere++;
	}
}

void Vehicle::GetSwitchPlaneInputs(float normalisedRX, float normalisedRY)
{
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

	Vector3 currentPos = PhysNode->GetPosition();
	//go to plane 180 degrees from the current
	if ((Window::GetKeyboard()->KeyDown(KEYBOARD_UP) || normalisedRY > 0.5) && !coolingDownPlaneSwitch)
	{
		//set previous plane ID to current ID before change
		previousPlaneID = currentPlaneID;

		Vector3 EulerOrientation;
		//set new position then change the plane of current velocity (maintain in a new direction relative to new plane
		Vector3 currentVelocity = PhysNode->GetLinearVelocity();
		switch (currentPlaneID)
		{
		case 0:
			//set start location
			startPoint = Vector2(PhysNode->GetPosition().x, PhysNode->GetPosition().y);
			//move player to relative position on plane
			endPoint = Vector2(-xLocationOnPlane, -SURFACE_CENTRE_OFFSET);
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
			startPoint = Vector2(PhysNode->GetPosition().x, PhysNode->GetPosition().y);
			//move player to relative position on plane
			endPoint = Vector2(-SURFACE_CENTRE_OFFSET, -xLocationOnPlane);
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
			startPoint = Vector2(PhysNode->GetPosition().x, PhysNode->GetPosition().y);
			//move player to relative position on plane
			endPoint = Vector2(-xLocationOnPlane, SURFACE_CENTRE_OFFSET);
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
			startPoint = Vector2(PhysNode->GetPosition().x, PhysNode->GetPosition().y);
			//move player to relative position on plane
			endPoint = Vector2(SURFACE_CENTRE_OFFSET, -xLocationOnPlane);
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
		SoundSystem::GetSoundSystem()->SwitchBoard(SoundManager::GetSound("../../Sounds/switchsound01.wav"));
	}

	if ((Window::GetKeyboard()->KeyDown(KEYBOARD_RIGHT) || normalisedRX > 0.5) && !coolingDownPlaneSwitch)
	{
		//set previous plane ID to current ID before change
		previousPlaneID = currentPlaneID;

		Vector3 EulerOrientation;
		//set new position then change the plane of current velocity (maintain in a new direction relative to new plane
		Vector3 currentVelocity = PhysNode->GetLinearVelocity();
		switch (currentPlaneID)
		{
		case 0:
			//set start location
			startPoint = Vector2(PhysNode->GetPosition().x, PhysNode->GetPosition().y);
			//move player to relative position on plane
			endPoint = Vector2(-SURFACE_CENTRE_OFFSET, xLocationOnPlane);
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
			startPoint = Vector2(PhysNode->GetPosition().x, PhysNode->GetPosition().y);
			//move player to relative position on plane
			endPoint = Vector2(-xLocationOnPlane, SURFACE_CENTRE_OFFSET);
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
			startPoint = Vector2(PhysNode->GetPosition().x, PhysNode->GetPosition().y);
			//move player to relative position on plane
			endPoint = Vector2(SURFACE_CENTRE_OFFSET, xLocationOnPlane);
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
			startPoint = Vector2(PhysNode->GetPosition().x, PhysNode->GetPosition().y);
			//move player to relative position on plane
			endPoint = Vector2(-xLocationOnPlane, -SURFACE_CENTRE_OFFSET);
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
		SoundSystem::GetSoundSystem()->SwitchBoard(SoundManager::GetSound("../../Sounds/switchsound01.wav"));
	}

	if ((Window::GetKeyboard()->KeyDown(KEYBOARD_LEFT) || normalisedRX < -0.5) && !coolingDownPlaneSwitch)
	{
		//set previous plane ID to current ID before change
		previousPlaneID = currentPlaneID;

		Vector3 EulerOrientation;
		//change the plane of current velocity (maintain in a new direction relative to new plane
		Vector3 currentVelocity = PhysNode->GetLinearVelocity();
		switch (currentPlaneID)
		{
		case 0:
			//set start location
			startPoint = Vector2(PhysNode->GetPosition().x, PhysNode->GetPosition().y);
			//move player to relative position on plane
			endPoint = Vector2(SURFACE_CENTRE_OFFSET, -xLocationOnPlane);
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
			startPoint = Vector2(PhysNode->GetPosition().x, PhysNode->GetPosition().y);
			//move player to relative position on plane
			endPoint = Vector2(xLocationOnPlane, -SURFACE_CENTRE_OFFSET);
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
			startPoint = Vector2(PhysNode->GetPosition().x, PhysNode->GetPosition().y);
			//move player to relative position on plane
			endPoint = Vector2(-SURFACE_CENTRE_OFFSET, -xLocationOnPlane);
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
			startPoint = Vector2(PhysNode->GetPosition().x, PhysNode->GetPosition().y);
			//move player to relative position on plane
			endPoint = Vector2(xLocationOnPlane, SURFACE_CENTRE_OFFSET);
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
		SoundSystem::GetSoundSystem()->SwitchBoard(SoundManager::GetSound("../../Sounds/switchsound01.wav"));
	}

}

void Vehicle::GetUserInput()
{
	//normalised rotation variable needed for advanced controller analogue input
	float normRotOnPlane = abs(rotationOnPlane) / MAX_VEHICLE_X_ROTATION;

	//update controller connected status - saves multiple calls from this class (it would be a lot!!!)
	controllerIsConnected = Window::GetControllerConnected();

	//get controller state
	float LX = 0, normalisedLX = 0;
	float RX = 0, RY = 0, normalisedRX = 0, normalisedRY = 0;
	XINPUT_STATE controllerState = Window::GetController()->GetState();

	//if connected set the controller input variables
	//https://msdn.microsoft.com/en-us/library/windows/desktop/ee417001%28v=vs.85%29.aspx
	if (controllerIsConnected)
	{
		//-------------------get left stick values----------------------
		LX = controllerState.Gamepad.sThumbLX;
		//check if the controller is outside dead zone
		if (abs(LX) > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
		{
			//clip the magnitude at its expected maximum value
			if (LX > 32767)
			{
				LX = 32767;
			}
			if (LX < -32767)
			{
				LX = -32767;
			}

			//adjust magnitude relative to the end of the dead zone
			if (LX > 0)
			{
				LX -= XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
			}
			else //will have to be negative due to previous ifs....so no need for if
			{
				LX += XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
			}

			//normalize the magnitude with respect to its expected range
			//giving a magnitude value of 0.0 to 1.0
			normalisedLX = LX / (32767 - XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
		}
		else //if the controller is in the deadzone zero out the magnitude
		{
			LX = 0.0f;
			normalisedLX = 0.0f;
		}
		//------------------------------------------------------------------

		//------------------get right stick values---------------------------
		RX = controllerState.Gamepad.sThumbRX;
		RY = controllerState.Gamepad.sThumbRY;

		//check if the controller is outside dead zone
		if (abs(RX) > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE || abs(RY) > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
		{
			//clip the magnitude at its expected maximum value
			if (RX > 32767)
			{
				RX = 32767;
			}
			if (RX < -32767)
			{
				RX = -32767;
			}
			if (RY > 32767)
			{
				RY = 32767;
			}
			if (RY < -32767)
			{
				RY = -32767;
			}

			//adjust magnitude relative to the end of the dead zone
			if (RX > 0)
			{
				RX -= XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
			}
			else //will have to be negative due to previous ifs....so no need for if
			{
				RX += XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
			}
			if (RY > 0)
			{
				RY -= XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
			}
			else //will have to be negative due to previous ifs....so no need for if
			{
				RY += XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
			}

			//normalize the magnitude with respect to its expected range
			//giving a magnitude value of 0.0 to 1.0
			normalisedRX = RX / (32767 - XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
			normalisedRY = RY / (32767 - XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
		}
		else //if the controller is in the deadzone in both axes zero out the magnitude
		{
			RX = 0.0f;
			RY = 0.0f;
			normalisedRX = 0.0f;
			normalisedRY = 0.0f;
		}
	}

	//adding some variables for LX with a tolerence level, trying to prevent stuttering on controller input
	float LXHigh = normalisedLX + CONTROLLER_ROTATION_TOLERENCE;
	float LXLow = normalisedLX - CONTROLLER_ROTATION_TOLERENCE;

	if (!isSwitchingPlane)
	{
		//switch plane if keybinds/controller pressed accordingly
		GetSwitchPlaneInputs(normalisedRX, normalisedRY);
	}



	//Statement to return the vehicle to flat rotation after A or D are released. 
	if (!controllerIsConnected && !Window::GetKeyboard()->KeyDown(KEYBOARD_A) && !Window::GetKeyboard()->KeyDown(KEYBOARD_D))
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
	//return statement with controller
	else if (!Window::GetKeyboard()->KeyDown(KEYBOARD_A) && !Window::GetKeyboard()->KeyDown(KEYBOARD_D))
	{
		//if (rotationOnPlane < -0.02 && (normRotOnPlane > normalisedLX))
		if (rotationOnPlane < -0.02 && (normRotOnPlane > LXHigh))
		{
			PhysNode->SetAngularVelocity(Vector3(0, 0, Speed_Turn));
		}
		//else if (rotationOnPlane > 0.02 && (normRotOnPlane > -normalisedLX))
		else if (rotationOnPlane > 0.02 && (normRotOnPlane > -LXHigh))
		{
			PhysNode->SetAngularVelocity(Vector3(0, 0, -Speed_Turn));
		}
		else
		{
			PhysNode->SetAngularVelocity(Vector3(0, 0, 0));
		}
	}



	if (Window::GetKeyboard()->KeyDown(KEYBOARD_D))
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
		SoundSystem::GetSoundSystem()->PlayMove();
	}

	//different method as allowing the controller to do fine adjustments to turn
	if (controllerIsConnected && normalisedLX > 0)
	{
		//rotate vehicle if not at max rotation
		//if ((rotationOnPlane > -MAX_VEHICLE_X_ROTATION) && (normRotOnPlane < normalisedLX))
		if ((rotationOnPlane > -MAX_VEHICLE_X_ROTATION) && (normRotOnPlane < LXLow))
		{
			PhysNode->SetAngularVelocity(Vector3(0, 0, -Speed_Turn));
		}
	}

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_A))
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
		SoundSystem::GetSoundSystem()->PlayMove();
	}

	if (controllerIsConnected && normalisedLX < 0)
	{
		//rotate vehicle if not at max rotation
		//if ((rotationOnPlane < MAX_VEHICLE_X_ROTATION) && (normRotOnPlane < -normalisedLX))
		if ((rotationOnPlane < MAX_VEHICLE_X_ROTATION) && (normRotOnPlane < -LXLow))
		{
			PhysNode->SetAngularVelocity(Vector3(0, 0, Speed_Turn));
		}
	}



	//---------------------------------------------------------------------------------------------------
	//Check for cooldown trigger input

	//slow PowerUp
	if (hasSlowPowerUp == true && slowPowerUpActive == false &&
		(Window::GetKeyboard()->KeyDown(KEYBOARD_Q) || controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_X))
	{
		hasSlowPowerUp = false;
		slowPowerUpActive = true;
		slowDurationRemaining = slowPUDuration;
	}

	//immunity
	if (hasImmunityPowerUp == true && immunityPowerUpActive == false &&
		(Window::GetKeyboard()->KeyDown(KEYBOARD_W) || controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_Y))
	{
		hasImmunityPowerUp = false;
		immunityPowerUpActive = true;
		immunityDurationRemaining = immunityPUDuration;
	}

	//cd reduction
	if (hasCDRedPowerUp == true && cdRedPowerUpActive == false &&
		(Window::GetKeyboard()->KeyDown(KEYBOARD_E) || controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_B))
	{
		hasCDRedPowerUp = false;
		cdRedPowerUpActive = true;
		cdRedDurationRemaining = cdRedPUDurtation;
	}
}

void Vehicle::updateCooldowns(float msec)
{
	//update cooldowns
	if (coolingDownPlaneSwitch)
	{
		//if cooldown powerup active, stop cooldown
		if (cdRedPowerUpActive)
		{
			coolingDownPlaneSwitch = false;
		}
		else
		{
			PlaneSwitchCDRemaining -= msec;
			if (PlaneSwitchCDRemaining <= 0)
			{
				coolingDownPlaneSwitch = false;
				PlaneSwitchCDRemaining = 0.0f;
			}
		}
	}


	if (slowPowerUpActive == true)
	{
		slowDurationRemaining -= msec;
		if (slowDurationRemaining < 0.0f)
		{
			slowDurationRemaining = 0.0f;
			slowPowerUpActive = false;
		}
	}

	if (immunityPowerUpActive == true)
	{
		immunityDurationRemaining -= msec;
		if (immunityDurationRemaining < 0.0f)
		{
			immunityDurationRemaining = 0.0f;
			immunityPowerUpActive = false;
		}
	}

	if (cdRedPowerUpActive == true)
	{
		cdRedDurationRemaining -= msec;
		if (cdRedDurationRemaining < 0.0f)
		{
			cdRedDurationRemaining = 0.0f;
			cdRedPowerUpActive = false;
		}
	}
}