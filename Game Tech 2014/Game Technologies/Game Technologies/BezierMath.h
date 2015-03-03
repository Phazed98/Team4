//added by Sam for Quadratic Bezier mathmatics 
//(needed for vehicle plane transition animation)
//taken from http://devmag.org.za/2011/04/05/bzier-curves-a-tutorial/

#pragma once

#include "../../nclgl/Vector2.h"

class BezierMath
{
public:

	static Vector2 calculate2DQuadBezierPoint(float t, Vector2 p0, Vector2 p1, Vector2 p2)
	{
		float u = 1 - t;
		float tsq = t * t;
		float usq = u * u;

		//create return Vec2
		Vector2 ret;
		ret.x = (usq * p0.x) + (2 * u * t * p1.x) + tsq * p2.x;
		ret.y = (usq * p0.y) + (2 * u * t * p1.y) + tsq * p2.y;

		return ret;
	}
};