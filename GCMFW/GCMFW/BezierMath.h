//added by Sam for Quadratic Bezier mathmatics 
//(needed for vehicle plane transition animation)
//taken from http://devmag.org.za/2011/04/05/bzier-curves-a-tutorial/

#pragma once

#include <vectormath/cpp/vectormath_aos.h>
#include <vectormath/cpp/quat_aos.h>

using namespace Vectormath::Aos;
class BezierMath
{
public:

	static Vector3 calculate2DQuadBezierPoint(float t, Vector3 p0, Vector3 p1, Vector3 p2)
	{
		float u = 1 - t;
		float tsq = t * t;
		float usq = u * u;

		//create return Vec2
		Vector3 ret;
		ret.setX((usq * p0.getX()) + (2 * u * t * p1.getX()) + tsq * p2.getX());
		ret.setY((usq * p0.getY()) + (2 * u * t * p1.getY()) + tsq * p2.getY());

		return ret;
	}
};