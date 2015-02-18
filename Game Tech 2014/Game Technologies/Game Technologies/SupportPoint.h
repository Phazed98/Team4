#pragma once
#include "../../nclgl/Vector3.h"

class SupportPoint
{
public:

	SupportPoint();
	SupportPoint(Vector3 &point);
	SupportPoint(Vector3 &point, Vector3 &pointInA, Vector3 &pointInB);

	Vector3 getPoint() { return point; }
	Vector3 getpointInA() { return pointInA; }
	Vector3 getpointInB() { return pointInB; }

	Vector3 setPoint(Vector3 v) { point = v; }
	Vector3 setpointInA(Vector3 v) { pointInA = v; }
	Vector3 setpointInB(Vector3 v) { pointInB = v; }

	bool operator==(const SupportPoint &r) const { return point == r.point; }

private:
	Vector3 point;
	Vector3 pointInA;
	Vector3 pointInB;

};

SupportPoint::SupportPoint()
{
	point = Vector3(0, 0, 0);
	pointInA = Vector3(0, 0, 0);
	pointInB = Vector3(0, 0, 0);
}

SupportPoint::SupportPoint(Vector3 &point) :
point(point)
{
	pointInA = Vector3(0, 0, 0);
	pointInB = Vector3(0, 0, 0);
}

SupportPoint::SupportPoint(Vector3 &point, Vector3 &pointInA, Vector3 &pointInB) :
point(point), pointInA(pointInA), pointInB(pointInB)
{

}

