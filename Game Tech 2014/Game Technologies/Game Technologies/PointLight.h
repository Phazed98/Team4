# pragma once

#include "../../nclgl/Vector4.h"
#include "../../nclgl/Vector3.h"

class PointLight {
public:
	PointLight(void){};
	PointLight(Vector3 position, Vector4 colour, float radius) {
		this->position = position;
		this->colour = colour;
		this->radius = radius;

	}

	~PointLight(void){};

	Vector3 GetPosition() const { return position; }
	void SetPosition(Vector3 val) { position = val; }

	float GetRadius() const { return radius; }
	void SetRadius(float val) { radius = val; }

	Vector4 GetColour() const { return colour; }
	void SetColour(Vector4 val) { colour = val; }

protected:
	Vector3 position;
	Vector4 colour;
	float radius;

};