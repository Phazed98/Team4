#include "Camera.h"

/*
Set useful default values!
*/
Camera::Camera(void)	
{
	yaw = 0.0f;
	pitch = 0.0f;
	pad = JOYPAD_A;
	position = Vector3(0, 0, 0);
}

Camera::~Camera(void)	
{
}

/*
This should all be very familiar to you, if you've actually looked
at the tutorial code for the previous module! Only changes are to
use Sony's Matrix4 class instead of mine. See how instead of having
an arbitrary 'rotation' function, that takes a direction vector to
rotate around, Sony provide functions to specifically rotate around
the X, Y, and Z axis
*/
void Camera::Update(float msec) 
{
	float y, p;

	//If this camera has no joypad controlling it, it can't have moved!
	if (pad == JOYPAD_NONE) 
	{
		return;
	}

	Input::GetJoypadMovement(y, p, pad);

	yaw += y/1000;
	pitch -= p/1000;

	pitch = min(pitch, 90.0f);
	pitch = max(pitch, -90.0f);

	if (yaw <0) 
	{
		yaw += 360.0f;
	}
	if (yaw > 360.0f) 
	{
		yaw -= 360.0f;
	}


	//if (Window::GetKeyboard()->KeyDown(KEYBOARD_W))
	//	position += Matrix4::Rotation(yaw, Vector3(0.0f, -1.0f, 0.0f)) * Matrix4::Rotation(pitch, Vector3(-1.0f, 0.0, 0.0f)) * Vector3(0.0f, 0.0f, -1.0f) * msec / speed;

	//if (Window::GetKeyboard()->KeyDown(KEYBOARD_S))
	//	position -= Matrix4::Rotation(yaw, Vector3(0.0f, -1.0f, 0.0f)) * Matrix4::Rotation(pitch, Vector3(-1.0f, 0.0, 0.0f)) * Vector3(0.0f, 0.0f, -1.0f) * msec / speed;

	//if (Window::GetKeyboard()->KeyDown(KEYBOARD_A))
	//	position += Matrix4::Rotation(yaw, Vector3(0.0f, -1.0f, 0.0f)) * Vector3(-1.0f, 0.0f, 0.0f) * msec / speed;

	//if (Window::GetKeyboard()->KeyDown(KEYBOARD_D))
	//	position -= Matrix4::Rotation(yaw, Vector3(0.0f, -1.0f, 0.0f)) * Vector3(-1.0f, 0.0f, 0.0f) * msec / speed;


	if (Input::ButtonDown(INPUT_UP, pad))
	{
		position += (Matrix4::rotationY(DegToRad(-yaw)) * Matrix4::rotationX(DegToRad(pitch)) * Vector3(0, 0, -1)  * msec).getXYZ();
	}
	if (Input::ButtonDown(INPUT_DOWN, pad)) 
	{
		position -= (Matrix4::rotationY(DegToRad(-yaw)) * Matrix4::rotationX(DegToRad(pitch)) * Vector3(0, 0, -1)  * msec).getXYZ();
	}

	if (Input::ButtonDown(INPUT_LEFT, pad)) 
	{
		position += (Matrix4::rotationY(DegToRad(-yaw)) * Vector3(-1, 0, 0) * msec).getXYZ();
	}
	if (Input::ButtonDown(INPUT_RIGHT, pad)) 
	{
		position -= (Matrix4::rotationY(DegToRad(-yaw)) * Vector3(-1, 0, 0) * msec).getXYZ();
	}

	//Go up and down using the shoulder buttons!
	if (Input::ButtonDown(INPUT_L1, pad)) 
	{
		position[1] -= msec/10;
	}
	if (Input::ButtonDown(INPUT_R1, pad)) 
	{
		position[1] += msec/10;
	}

	//std::cout << "Yaw: " << yaw << " Pitch: " << pitch << std::endl;
}

Matrix4 Camera::BuildViewMatrix() 
{
	return	Matrix4::rotationX(DegToRad(-pitch)) * Matrix4::rotationY(DegToRad(yaw)) * Matrix4::translation(-position);
}
