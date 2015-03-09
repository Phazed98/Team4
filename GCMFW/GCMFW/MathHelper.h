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

	//Added by Sam for chase camera position LERP
	inline Vector3 static getWeightedAverage(Vector3 rhs, Vector3 lhs, float weight)
	{
		Vector3 result;
		result.setX(((lhs.getX() * (weight - 1)) + rhs.getX()) / weight);
		result.setY(((lhs.getY() * (weight - 1)) + rhs.getY()) / weight);
		result.setZ(((lhs.getZ() * (weight - 1)) + rhs.getZ()) / weight);

		return result;
	}

	inline float static lengthSquared(Quat quatToSquare) 
	{
		float xsq = quatToSquare.getX() * quatToSquare.getX();
		float ysq = quatToSquare.getY() * quatToSquare.getY();
		float zsq = quatToSquare.getZ() * quatToSquare.getZ();
		float wsq = quatToSquare.getW() * quatToSquare.getW();

		return xsq + ysq + zsq + wsq;
	}

	inline Quat static invert(Quat quatToInvert)
	{
		//Conjugate the Quaternion
		quatToInvert.setX(-quatToInvert.getX());
		quatToInvert.setY(-quatToInvert.getY());
		quatToInvert.setZ(-quatToInvert.getZ());

		float LSq;

		LSq = lengthSquared(quatToInvert);

		quatToInvert.setX(quatToInvert.getX() / LSq);
		quatToInvert.setY(quatToInvert.getY() / LSq);
		quatToInvert.setZ(quatToInvert.getZ() / LSq);
		quatToInvert.setW(quatToInvert.getW() / LSq);

		return quatToInvert;
	}

	inline Vector3 static applyMat3RotToVec3(Matrix4 matToVec, Vector3 v)
	{
		Vector3 retVec = Vector3(0, 0, 0);
		retVec.setX((matToVec.getElem(0, 0)* v.getX()) + (matToVec.getElem(1, 0)* v.getY()) + (matToVec.getElem(2, 0) * v.getZ()));
		retVec.setY((matToVec.getElem(0, 1)* v.getX()) + (matToVec.getElem(1, 1)* v.getY()) + (matToVec.getElem(2, 1) * v.getZ()));
		retVec.setZ((matToVec.getElem(0, 2)* v.getX()) + (matToVec.getElem(1, 2)* v.getY()) + (matToVec.getElem(2, 2) * v.getZ()));

		return retVec;
	}


	//NOTE - please use normalised vectors
	//Added by Sam 
	static inline float get360AngleBetweenTwoVecs(Vector3  origin, Vector3  other)
	{
		float dotProductResult = dot(origin, other);

		Vector3 planeNormal = cross(origin, other);
	normalize(planeNormal);

		//xx*yy*zz + xy*yz*zx + xz*yx*zy - zx*yy*xz - zy*yz*xx - zz*yx*xy

		float det = (origin.getX() * other.getY() * planeNormal.getZ()) +
					(origin.getY() * other.getZ() * planeNormal.getX()) +
					(origin.getZ() * other.getX() * planeNormal.getY()) -
					(planeNormal.getX() * other.getY() * origin.getZ()) -
					(planeNormal.getY() * other.getZ() * origin.getX()) -
					(planeNormal.getZ() * other.getX() * origin.getY());

		float angle = atan2(det, dotProductResult);

		angle = RadToDeg(angle);

		if (planeNormal.getZ() > 0.0f)
		{
			angle = -angle;
		}

		return angle;
	}

	static inline Quat addQuats(Quat a, Quat b)
	{
		Quat c;

		c.setX(a.getX() + b.getX());
		c.setY(a.getY() + b.getY());
		c.setZ(a.getZ() + b.getZ());
		c.setW(a.getW() + b.getW());

		return c;
	}

	static inline Quat multQuatAndVec(Quat a, Vector3 b)
	{
		/*
		ans.w = -(x * b.x) - (y * b.y) - (z * b.z);
		ans.x =  (w * b.x) + (y * b.z) - (z * b.y);
		ans.y =  (w * b.y) + (z * b.x) - (x * b.z);
		ans.z =  (w * b.z) + (x * b.y) - (y * b.x);
		*/

		Quat c;

		//ans.w = -(x * b.x) - (y * b.y) - (z * b.z);
		//ans.x = (w * b.x) + (y * b.z) - (z * b.y);
		//ans.y = (w * b.y) + (z * b.x) - (x * b.z);
		//ans.z = (w * b.z) + (x * b.y) - (y * b.x);
		
		//ans.w =	-(x *			b.x) -		(y *		b.y) -		(z *		b.z);
		c.setW(		-(a.getX() *	b.getX()) - (a.getY() * b.getY()) - (a.getZ() * b.getZ()));
		
		//ans.x =	(w *		b.x) +		(y *		b.z) -		(z *		b.y);
		c.setX(		(a.getW() * b.getX()) + (a.getY() * b.getZ()) - (a.getZ() * b.getY()));

		//ans.y =	(w *		b.y) +		(z *		b.x) -		(x *		b.z);
		c.setY(		(a.getW() * b.getY()) + (a.getZ() * b.getX()) - (a.getX() * b.getZ()));

		//ans.z =	(w *		b.z) +		(x *		b.y) -		(y *		b.x);
		c.setZ(		(a.getW() * b.getZ()) + (a.getX() * b.getY()) - (a.getY() * b.getX()));

		return c;

	}



};