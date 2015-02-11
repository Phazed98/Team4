#pragma once
#include "../../nclgl/Vector3.h"

//Added by Sam to allow the support point to be stored outside the GJK class
class SupportPoint
{
public:

	SupportPoint();
	SupportPoint(Vector3 &point);
	SupportPoint(Vector3 &point, Vector3 &pointInA, Vector3 &pointInB);
	//added for optimised GJK simplex storage
	SupportPoint(Vector3 &point, Vector3 &pointInA, Vector3 &pointInB, int indexA, int indexB);

	Vector3 getPoint() { return point; }
	Vector3 getpointInA() { return pointInA; }
	Vector3 getpointInB() { return pointInB; }
	int getVertIndexInA() { return vertIndexInA; }
	int getVertIndexInB() { return vertIndexInB; }

	void setPoint(Vector3 v) { point = v; }
	void setpointInA(Vector3 v) { pointInA = v; }
	void setpointInB(Vector3 v) { pointInB = v; }
	void setVertIndexInA(int index){ vertIndexInA = index; }
	void setVertIndexInB(int index){ vertIndexInB = index; }


	bool operator==(const SupportPoint &r) const { return point == r.point; }

private:
	Vector3 point;
	Vector3 pointInA;
	Vector3 pointInB;

	//added so the simplex storage can work
	int vertIndexInA;
	int vertIndexInB;

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

SupportPoint::SupportPoint(Vector3 &point, Vector3 &pointInA, Vector3 &pointInB, int indexA, int indexB) :
point(point), pointInA(pointInA), pointInB(pointInB), vertIndexInA(indexA), vertIndexInB(indexB)
{

}