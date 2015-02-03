#include "Camera.h"

void Camera::UpdateCamera(float msec)
{
	pitch += (Window::GetMouse()->GetRelativePosition().y);
	yaw += (Window::GetMouse()->GetRelativePosition().x);

	pitch = min(pitch, 90.0f);
	pitch = max(pitch, -90.0f);

	if (yaw < 0)
		yaw += 360.0f;

	if (yaw > 360.0f)
		yaw -= 360.0f;

	float speed = 2.0f;

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_W))
		position += Matrix4::Rotation(yaw, Vector3(0.0f, -1.0f, 0.0f)) * Matrix4::Rotation(pitch, Vector3(-1.0f, 0.0, 0.0f)) * Vector3(0.0f, 0.0f, -1.0f) * msec / speed;

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_S))
		position -= Matrix4::Rotation(yaw, Vector3(0.0f, -1.0f, 0.0f)) * Matrix4::Rotation(pitch, Vector3(-1.0f, 0.0, 0.0f)) * Vector3(0.0f, 0.0f, -1.0f) * msec / speed;

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_A))
		position += Matrix4::Rotation(yaw, Vector3(0.0f, -1.0f, 0.0f)) * Vector3(-1.0f, 0.0f, 0.0f) * msec / speed;

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_D))
		position -= Matrix4::Rotation(yaw, Vector3(0.0f, -1.0f, 0.0f)) * Vector3(-1.0f, 0.0f, 0.0f) * msec / speed;

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_SPACE))
		position.y += msec / speed;

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_SHIFT))
		position.y -= msec / speed;


}


Vector3 Camera::getFacingVector()
{
	Matrix4 viewMatrix = BuildViewMatrix();

	Vector3 faceVector(viewMatrix.values[2], viewMatrix.values[6], viewMatrix.values[10]);

	return faceVector;
}

Matrix4 Camera::BuildViewMatrix()
{
	return	Matrix4::Rotation(-pitch, Vector3(-1, 0, 0)) *
		Matrix4::Rotation(-yaw, Vector3(0, -1, 0)) *

		Matrix4::Translation(-position);
}