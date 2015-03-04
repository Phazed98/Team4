#include <vectormath/cpp/vectormath_aos.h>
#include <vectormath/cpp/quat_aos.h>
#include "common.h"

using namespace Vectormath::Aos;



class MathHelper
{
public:
	inline Vector4 static vec3ToEuler(float pitch, float yaw, float roll)
	{
		float y2 = (float)DegToRad(yaw / 2.0f);
		float p2 = (float)DegToRad(pitch / 2.0f);
		float r2 = (float)DegToRad(roll / 2.0f);


		float cosy = (float)cos(y2);
		float cosp = (float)cos(p2);
		float cosr = (float)cos(r2);

		float siny = (float)sin(y2);
		float sinp = (float)sin(p2);
		float sinr = (float)sin(r2);

		Vector4 q;

		q.setX(cosr * sinp * cosy + sinr * cosp * siny);
		q.setY(cosr * cosp * siny - sinr * sinp * cosy);
		q.setZ(sinr * cosp * cosy - cosr * sinp * siny);
		q.setW(cosr * cosp * cosy + sinr * sinp * siny);

		return q;
	}
};