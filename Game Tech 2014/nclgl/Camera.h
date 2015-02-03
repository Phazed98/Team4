#pragma once

#include "Window.h"
#include "Matrix4.h"
#include "Vector3.h"
#include "Quaternion.h"

class Camera
{
public:
	Camera(void)
	{
		yaw = 0.0f;
		pitch = 0.0f;
	};
	Camera(float pitch, float yaw, Vector3 position)
	{
		this->pitch = pitch;
		this->yaw = yaw;
		this->position = position;
	}
	~Camera(void)
	{

	}

	void UpdateCamera(float msec = 10.0f);
	Vector3 getFacingVector();
	Matrix4 BuildViewMatrix();
	Vector3 GetPosition() const		{ return position; }
	void SetPosition(Vector3 val)	{ position = val; }

	float GetYaw() const			{ return yaw; }
	void SetYaw(float y)			{ yaw = y; }

	float GetPitch() const			{ return pitch; }
	void SetPitch(float p)			{ pitch = p; }


protected:

	float yaw;
	float pitch;
	bool boost;
	Vector3 position;
};