#include"GJK.h"

GJK::GJK()
{
	nrPointsSimplex = 0;

}
GJK::~GJK()
{

}


bool GJK::CollisionDetection(PhysicsNode& n0, PhysicsNode& n1, CollisionData* cd)
{
	//Default Direction
	Vector3 dir = Vector3(1, 1, 1);

	//Reset Any points
	a = b = c = d = SupportPoint();
	simplexVector.clear();
	nrPointsSimplex = 0;

	Mesh* mesh0 = n0.getTarget()->GetMesh();
	Mesh* mesh1 = n1.getTarget()->GetMesh();

	Vector3* Object0Vertices = mesh0->getVertices();
	Vector3* Object1Vertices = mesh1->getVertices();

	int Object0VertexCount = n0.getTarget()->GetMesh()->getNumVertices();
	int Object1VertexCount = n1.getTarget()->GetMesh()->getNumVertices();

	Matrix4 translation0 = Matrix4::Translation(n0.GetPosition());
	Matrix4 translation1 = Matrix4::Translation(n1.GetPosition());

	Matrix4 orientation0 = n0.GetOrientation().ToMatrix();
	Matrix4 orientation1 = n1.GetOrientation().ToMatrix();


	Matrix4 scale0 = Matrix4::Scale(n0.getTarget()->GetModelScale());
	Matrix4 scale1 = Matrix4::Scale(n1.getTarget()->GetModelScale());

	Matrix4 Object0Matrix = translation0 * orientation0 * scale0;
	Matrix4 Object1Matrix = translation1 * orientation1 * scale1;

	Vector3* Object0FixedVertices = new Vector3[Object0VertexCount];
	Vector3* Object1FixedVertices = new Vector3[Object1VertexCount];

	//Get Objects in 3D space with proper Scaling rotation and translation
	for (int x = 0; x < Object0VertexCount; x++)
		Object0FixedVertices[x] = Object0Matrix *  Object0Vertices[x];

	for (int x = 0; x < Object1VertexCount; x++)
		Object1FixedVertices[x] = Object1Matrix *  Object1Vertices[x];

	//Get first Point of TetraHedron
	c = Support(Object0FixedVertices, Object0VertexCount, Object1FixedVertices, Object1VertexCount, dir);

	dir = -(c.getPoint());

	//Get Point Opposite point C's normal
	b = Support(Object0FixedVertices, Object0VertexCount, Object1FixedVertices, Object1VertexCount, dir);

	//Early Out because is invalid
	if (Vector3::Dot(b.getPoint(), dir) < 0)
	{
		delete[] Object0FixedVertices;
		delete[] Object1FixedVertices;
		return false;
	}

	dir = Vector3::DoubleCross(c.getPoint() - b.getPoint(), -b.getPoint());

	nrPointsSimplex = 2;

	int steps = 0;

	while (steps < 20) //This is a bounded while true to avoid "Going down the Rabbit Hole" if no out is possible
	{
		//Build A from the Updated Vector
		a = Support(Object0FixedVertices, Object0VertexCount, Object1FixedVertices, Object1VertexCount, dir);

		//A is invalid so early out, YAY!!!
		if (Vector3::Dot(a.getPoint(), dir) < 0)
		{
			delete[] Object0FixedVertices;
			delete[] Object1FixedVertices;
			return false;
		}

		else //Check if we have a valid solution or need to continue to iterate
		{
			if (ContainsOrigin(dir))
			{

				/* This is the basic algorithm for EPA :

				1 Take over the simplex from GJK when GJK terminated, and “blow up” the simplex to a tetrahedron if it contains less than 4 vertices.

				2 Use the 4 faces(triangles) of the tetrahedron to construct an initial polytope.

				3 Pick the closest face of the polytope to the origin.

				4 If the closest face is no closer(by a certain threshold) to the origin than the previously picked one, go to 8.

				5 Remove the closest face, use the face normal(outward pointing) as the search direction to find a support point on the CSO.

				6 Remove all faces from the polytope that can be “seen” by this new support point, and add new faces to cover up the “hole” on the polytope, where all new faces share the new support
				point as a common vertex(this is the expanding part of the algorithm).

				7 Go to 3.

				8 Project the origin onto the closest triangle.This is our closest point to the origin on the CSO’s boundary.Compute the barycentric coordinates of this closest point with respect to
				the vertices from the closest triangle.The barycentric coordinates are coefficients of linear combination of vertices from the closest triangle.Linearly combining the individual
				support points(original results from individual colliders) corresponding to the vertices from the closest triangle, with the same barycentric coordinates as coefficients, gives us
				contact points on both colliders in their own model space.We can then convert these contact points to world space.

				9 End EPA.
				*/

				if (a == Vector3(0, 0, 0) || b == Vector3(0, 0, 0) || c == Vector3(0, 0, 0) || d == Vector3(0, 0, 0))
				{
					return false;
				}

				/*1 Take over the simplex from GJK when GJK terminated, and “blow up” the simplex to a tetrahedron if it contains less than 4 vertices.Assuming Now that we dont need this 
				*/

				/*
				2 Use the 4 faces(triangles) of the tetrahedron to construct an initial polytope.
				*/
				triVector.push_back(Triangle(a, b, c));
				triVector.push_back(Triangle(a, c, d));
				triVector.push_back(Triangle(a, d, b));
				triVector.push_back(Triangle(b, d, c));

				float distToOrigin = -FLT_MAX;

				int count = 0;
				while (count < 100) //Should never matter, but if EPA gets stuck, have an out
				{
					count++;
					/*
					3 Pick the closest face of the polytope to the origin.
					*/
					Triangle tempT = triVector.at(findTriangle());
					SupportPoint entry_cur_support = Support(Object0FixedVertices, Object0VertexCount, Object1FixedVertices, Object1VertexCount, tempT.triNormal);
					float vecLength = Vector3::Dot(entry_cur_support.getPoint(), tempT.triNormal);

					/*4 If the closest face is no closer(by a certain threshold) to the origin than the previously picked one, go to 8. (8 = collisionData generation)
					*/
					if (vecLength - distToOrigin < 0.001f)
					{
						/*8 Project the origin onto the closest triangle.This is our closest point to the origin on the CSO’s boundary.Compute the barycentric coordinates of this closest point with respect to the vertices from the closest triangle.The barycentric coordinates are coefficients of linear combination of vertices from the closest triangle.Linearly combining the individual support points(original results from individual colliders) corresponding to the vertices from the closest triangle, with the same barycentric coordinates as coefficients, gives us contact points on both colliders in their own model space.We can then convert these contact points to world space.
						*/

						float bary_u, bary_v, bary_w;
						barycentric(tempT.triNormal * vecLength,
							tempT.a.getPoint(),
							tempT.b.getPoint(),
							tempT.c.getPoint(),
							&bary_u,
							&bary_v,
							&bary_w);

						// collision point on object a in world space
						cd->m_point = (tempT.a.getpointInA() * bary_u) +
							(tempT.b.getpointInB() * bary_v) +
							(tempT.c.getpointInA() * bary_w);

						// collision normal

						cd->m_normal = -tempT.triNormal;

						// penetration depth
						cd->m_penetration = distToOrigin * 0.1;

						if (cd->m_penetration > 6 || cd->m_penetration < 0)
							return false;

						if (cd->m_normal.Length() > 1.1 || cd->m_normal.Length() < 0.9)
							return false;


						triVector.clear();
						//break;

						delete[] Object0FixedVertices;
						delete[] Object1FixedVertices;
						return true;
					}

					distToOrigin = vecLength;

					//Determine to remove = Iterate though triangles 
					// If dot product of (triangles normal, SupportPoint - currentTriangleFirstpoint)
				
					/*
					5 Remove the closest face, use the face normal(outward pointing) as the search direction to find a support point on the CSO. Remove Triangle and Add its edges to the edgeVector
					*/
			

					// If dot product of (triangles normal, SupportPoint - currentTriangleFirstpoint)
					for (auto i = triVector.begin(); i != triVector.end();)
					{
						Triangle& t = *i;
						if (Vector3::Dot(t.triNormal, (entry_cur_support.getPoint() - t.a.getPoint())) > 0)
						{
							addEdge(t.a, t.b);
							addEdge(t.b, t.c);
							addEdge(t.c, t.a);
							i = triVector.erase(i);
							continue;
						}
						++i;
					}

					//if currentEdge == any other edge

					//Create Triangles from EdgeList
					for (Edge e : edgeVector)
					{
						triVector.push_back(Triangle(entry_cur_support, e.a, e.b));
					}

					/*
					6 Remove all faces from the polytope that can be “seen” by this new support point, and add new faces to cover up the “hole” on the polytope, where all new faces share the new support point as a common vertex(this is the expanding part of the algorithm).
					*/
					edgeVector.clear();
	
					

					/*
					7 Go to 3. and Try Again
					*/
				} 
				

				/*
				9 End EPA.
				*/
				delete[] Object0FixedVertices;
				delete[] Object1FixedVertices;
				return false;
			}
		}
		steps++;
	}

	delete[] Object0FixedVertices;
	delete[] Object1FixedVertices;
	return false;
}

bool GJK::ContainsOrigin(Vector3& dir)
{
	if (nrPointsSimplex == 2) // We currently have a line, build a triangle
	{
		return triangle(dir);
	}
	else if (nrPointsSimplex == 3) //We Have a triangle built a Tetrahedron
	{
		return tetrahedron(dir);
	}

	return false;
}


bool GJK::triangle(Vector3& dir)
{
	Vector3 ao = Vector3(-a.getPoint().x, -a.getPoint().y, -a.getPoint().z);
	Vector3 ab = b.getPoint() - a.getPoint();
	Vector3 ac = c.getPoint() - a.getPoint();
	Vector3 abc = Vector3::Cross(ab, ac);

	Vector3 ab_abc = Vector3::Cross(ab, abc);

	if (Vector3::Dot(ab_abc, ao) > 0)
	{
		//Swap the two points
		c = b;
		b = a;

		//ab_abc doesnt poitn at origin, so change the direction
		dir = Vector3::DoubleCross(ab, ao);
		return false;
	}


	Vector3 abc_ac = Vector3::Cross(abc, ac);

	if (Vector3::Dot(abc_ac, ao) > 0)
	{
		//Swap B and A
		b = a;

		//abc_ac doesnt point at origin
		dir = Vector3::DoubleCross(ac, ao);
		return false;
	}


	if (Vector3::Dot(abc, ao) > 0)
	{
		//Swap around all points
		d = c;
		c = b;
		b = a;

		//update direction
		dir = abc;
	}

	else
	{
		//This means the tetrahedron is upside down????
		d = b;
		b = a;
		dir = -abc;
	}

	nrPointsSimplex = 3;

	return false;

}
bool GJK::tetrahedron(Vector3& dir)
{
	//Create the existing points
	Vector3 ao = -a.getPoint();
	Vector3 ab = b.getPoint() - a.getPoint();
	Vector3 ac = c.getPoint() - a.getPoint();

	//Build the abc triangle
	Vector3 abc = Vector3::Cross(ab, ac);


	if (Vector3::Dot(abc, ao) > 0)
	{
		//Origin is in front of first triangle and not inside tetrahedron, update points and return false;
		return checkTetrahedron(ao, ab, ac, abc, dir);
	}


	// First Triangle didnt see the origin, check second triangle
	Vector3 ad = d.getPoint() - a.getPoint();

	//Build the acd triangle
	Vector3 acd = Vector3::Cross(ac, ad);

	if (Vector3::Dot(acd, ao) > 0)
	{
		//Origin is in front of second triangle and not inside tetrahedron, update points and return false;
		b = c;
		c = d;
		ab = ac;
		ac = ad;
		abc = acd;
		return checkTetrahedron(ao, ab, ac, abc, dir);
	}

	//build adb triangle
	Vector3 adb = Vector3::Cross(ad, ab);

	if (Vector3::Dot(adb, ao) > 0)
	{
		//Origin is in front of third triangle and not inside tetrahedron, update points and return false;

		c = b;
		b = d;

		ac = ab;
		ab = ad;

		abc = adb;

		return checkTetrahedron(ao, ab, ac, abc, dir);

	}


	//Origin is inside tetrahedron !!!! Success in 
	return true;

}

bool GJK::checkTetrahedron(const Vector3& ao, const Vector3& ab, const Vector3& ac, const Vector3& abc, Vector3& dir)
{
	//almost the same like triangle checks
	Vector3 ab_abc = Vector3::Cross(ab, abc);

	if (Vector3::Dot(ab_abc, ao) > 0)
	{
		c = b;
		b = a;

		//dir is not ab_abc because it's not point towards the origin;
		//ABxA0xAB direction we are looking for
		dir = Vector3::DoubleCross(ab, ao);

		//build new triangle
		// d will be lost
		nrPointsSimplex = 2;

		return false;
	}

	Vector3 acp = Vector3::Cross(abc, ac);

	if (Vector3::Dot(acp, ao) > 0)
	{
		b = a;

		//dir is not abc_ac because it's not point towards the origin;
		//ACxA0xAC direction we are looking for
		dir = Vector3::DoubleCross(ac, ao);

		//build new triangle
		// d will be lost
		nrPointsSimplex = 2;

		return false;
	}

	//build new tetrahedron with new base
	d = c;
	c = b;
	b = a;

	dir = abc;

	nrPointsSimplex = 3;

	return false;
}

//Support function Gets the minkoswki points farthest points in the provided vector in the specified direction (Reallllly usefull for future note)
SupportPoint GJK::Support(Vector3 pt0[], int point0Count, Vector3 pt1[], int point1Count, Vector3& dir)
{
	Vector3 dirCopy = dir;
	dirCopy.Normalise();

	Vector3 p1 = pt0[DirectionPoint(pt0, point0Count, dirCopy)];
	Vector3 p2 = pt1[DirectionPoint(pt1, point1Count, -dirCopy)];

	Vector3 p3 = p1 - p2;
	SupportPoint point(p3, p1, p2);
	return  point;
}

// Method From Real-Time Collision Detection
// Returns indices imin and imax into pt[] array of the least and
// most, respectively, distant points along the direction dir
int GJK::DirectionPoint(Vector3 pt[], int pointNum, Vector3& dir)
{
	float maxproj = -FLT_MAX;
	int indexPosition;

	for (int i = 0; i <pointNum; i++)
	{
		// Project vector from origin to point onto direction vector
		float proj = Vector3::Dot(pt[i], dir);

		// Keep track of most distant point along direction vector
		if (proj > maxproj)
		{
			maxproj = proj;
			indexPosition = i;
		}
	}
	return indexPosition;
}


//Finds the triangle closest to origin on minkowski sum
int GJK::findTriangle()
{
	int index = 0;
	float value = FLT_MAX;
	int triIndex = 0;

	for (Triangle t : triVector)
	{
		float triValue = Vector3::Dot(t.triNormal, t.a.getPoint());

		if (triValue < value)
		{
			value = triValue;
			triIndex = index;
		}

		index++;
	}

	return triIndex;
}


//Adds edge to edge list, checks to make sure opposite edge doesnt already exist first, if it does remove them both (Creates the required hole)
void GJK::addEdge(SupportPoint &a, SupportPoint &b)
{
	int count = 0;
	for (Edge e : edgeVector)
	{
		if (e.a == b && e.b == a) //If opposite exists remove both from the equation
		{
			edgeVector.erase(edgeVector.begin() + count);
			return;
		}
		count++;
	}

	edgeVector.push_back(Edge(a, b));
}


// Code from Crister Erickson's Real-Time Collision Detection
// Takes Coordinates and converts the mto localspace coordinates for the collision
void GJK::barycentric(const Vector3 &p, const Vector3 &a, const Vector3 &b, const Vector3 &c, float *u, float *v, float *w)
{

	Vector3 v0 = b - a, v1 = c - a, v2 = p - a;

	float d00 = Vector3::Dot(v0, v0);
	float d01 = Vector3::Dot(v0, v1);
	float d11 = Vector3::Dot(v1, v1);
	float d20 = Vector3::Dot(v2, v0);
	float d21 = Vector3::Dot(v2, v1);

	float denom = d00 * d11 - d01 * d01;
	*v = (d11 * d20 - d01 * d21) / denom;
	*w = (d00 * d21 - d01 * d20) / denom;
	*u = 1.0f - *v - *w;
}