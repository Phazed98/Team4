//#pragma once
//#include "PhysicsNode.h"
//#include "SupportPoint.h"
//#include "GJKSimplex.h"
//#include "Mesh.h"
//
//struct Triangle
//{
//	SupportPoint a, b, c;
//	Vector3 triNormal;
//
//	Triangle(SupportPoint &aa, SupportPoint &bb, SupportPoint &cc)
//	{
//		a = aa;
//		b = bb;
//		c = cc;
//		triNormal = cross((bb.getPoint() - aa.getPoint()), (cc.getPoint() - aa.getPoint()));
//		normalize(triNormal);
//	}
//};
//
//struct Edge
//{
//	SupportPoint a, b;
//
//	Edge(SupportPoint &aa, SupportPoint &bb)
//	{
//		a = aa;
//		b = bb;
//	}
//};
//
//class GJK
//{
//public:
//	GJK();
//	~GJK();
//
//	//Keeping nomral GJK method...might be needed somewhere (Sam)
//	bool CollisionDetection(PhysicsNode& s0, PhysicsNode& s1, CollisionData* collisionData = NULL);
//
//	//Added by Sam - optimised GJK using GJKSimplex history object to speed up collision detection in successive frames.
//	bool CollisionDetection(PhysicsNode& s0, PhysicsNode& s1, GJKSimplex simplexStore, CollisionData* collisionData = NULL);
//
//
//
//private:
//
//	bool ContainsOrigin(Vector3&);
//	bool triangle(Vector3&);
//	bool tetrahedron(Vector3&);
//	bool checkTetrahedron(const Vector3&, const Vector3&, const Vector3&, const Vector3&, Vector3& dir);
//
//	void addEdge(SupportPoint &a, SupportPoint &b);
//	void addTriangle();
//
//	int findTriangle();
//	float findTriangleLength();
//	int findEdge();
//
//	SupportPoint Support(Vector3 pt0[], int point0Count, Vector3 pt1[], int point1Count, Vector3& dir);
//	int DirectionPoint(Vector3 pt[], int pointNum, Vector3& dir);
//	Vector3 ClosestPtPointTriangle(Vector3 p, Vector3 a, Vector3 b, Vector3 c);
//	void collisionDataGeneration();
//	void barycentric(const Vector3 &p, const Vector3 &a, const Vector3 &b, const Vector3 &c, float *u, float *v, float *w);
//
//	SupportPoint a, b, c, d;
//	vector <Triangle> triVector;
//	vector <Edge> edgeVector;
//	int nrPointsSimplex;
//
//
//
//	//Added by Sam for the optimised GJK
//	SupportPoint SupportWithStore(Vector3 pt0[], int point0Count, Vector3 pt1[], int point1Count, Vector3& dir);
//	bool checkTetraBaseStillValid();
//	void restoreSupportPoint(SupportPoint &point, Vector3 pt0[], int &pointIndex0, Vector3 pt1[], int &pointIndex1);
//
//};