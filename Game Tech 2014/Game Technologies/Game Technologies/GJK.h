#pragma once
#include "../../nclgl/Vector3.h"
#include "PhysicsNode.h"

struct SupportPoint
{
	Vector3 vector; // the minkowski difference point

	// the individual support points
	Vector3 sup_a;
	Vector3 sup_b; // not actually necessary

	SupportPoint(Vector3 &vv, Vector3 &sup_aa, Vector3 &sup_bb)
	{
		vector = vv;
		sup_a = sup_aa;
		sup_b = sup_bb;
	}

	SupportPoint()
	{
		vector = Vector3(0, 0, 0);
		sup_a = Vector3(0, 0, 0);
		sup_b = Vector3(0, 0, 0);
	}


	SupportPoint(Vector3 &vv)
	{
		vector = vv;
		sup_a = Vector3(0, 0, 0);
		sup_b = Vector3(0, 0, 0);
	}

	BOOL operator==(const SupportPoint &r) const { return vector == r.vector; }
};

struct Triangle
{
	SupportPoint a, b, c;
	Vector3 triNormal;

	Triangle(SupportPoint &aa, SupportPoint &bb, SupportPoint &cc)
	{
		a = aa;
		b = bb;
		c = cc;
		triNormal = Vector3::Cross((bb.vector - aa.vector), (cc.vector - aa.vector));
		triNormal.Normalise();
	}
};

struct Edge
{
	SupportPoint a, b;

	Edge(SupportPoint &aa, SupportPoint &bb)
	{
		a = aa;
		b = bb;
	}
};

class GJK
{
public:
	GJK();
	~GJK();
	bool CollisionDetection(PhysicsNode& s0, PhysicsNode& s1, CollisionData* collisionData = NULL);




private:

	bool ContainsOrigin(Vector3&);
	bool triangle(Vector3&);
	bool tetrahedron(Vector3&);
	bool checkTetrahedron(const Vector3&, const Vector3&, const Vector3&, const Vector3&, Vector3& dir);

	void addEdge(SupportPoint &a, SupportPoint &b);
	void addTriangle();

	int findTriangle();
	float findTriangleLength();
	int findEdge();

	SupportPoint Support(Vector3 pt0[], int point0Count, Vector3 pt1[], int point1Count, Vector3& dir);
	int DirectionPoint(Vector3 pt[], int pointNum, Vector3& dir);
	Vector3 ClosestPtPointTriangle(Vector3 p, Vector3 a, Vector3 b, Vector3 c);
	void collisionDataGeneration();
	void barycentric(const Vector3 &p, const Vector3 &a, const Vector3 &b, const Vector3 &c, float *u, float *v, float *w);

	SupportPoint a, b, c, d;
	vector<Vector3> simplexVector;
	vector <Triangle> triVector;
	vector <Edge> edgeVector;
	int nrPointsSimplex;


};