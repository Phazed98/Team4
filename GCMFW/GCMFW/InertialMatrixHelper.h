#pragma once
/**
* Author: Callum Rhodes <c.g.rhodes@ncl.ac.uk>
*/

#include <vectormath/cpp/vectormath_aos.h>

using namespace Vectormath::Aos;

class InertialMatrixHelper {
public:

	static Matrix4 createSphereInertial(float mass, float radius) 
	{
		float I = 0.4f*mass*radius*radius;

		Vector4 Row0(I, 0, 0, 0);
		Vector4 Row1(0, I, 0, 0);
		Vector4 Row2(0, 0, I, 0);
		Vector4 Row3(0, 0, 0, 1);

		Matrix4 resultMat;
		resultMat.setRow(0, Row0);
		resultMat.setRow(1, Row1);
		resultMat.setRow(2, Row2);
		resultMat.setRow(3, Row3);

		return resultMat;
	}

	static Matrix4 createSphereInvInertial(float mass, float radius) {
		float I = 2.5f / (mass*radius*radius);

		float elements[] = { I, 0, 0, 0, 0, I, 0, 0, 0, 0, I, 0, 0, 0, 0, 1 };

		Vector4 Row0(I, 0, 0, 0);
		Vector4 Row1(0, I, 0, 0);
		Vector4 Row2(0, 0, I, 0);
		Vector4 Row3(0, 0, 0, 1);

		Matrix4 resultMat;
		resultMat.setRow(0, Row0);
		resultMat.setRow(1, Row1);
		resultMat.setRow(2, Row2);
		resultMat.setRow(3, Row3);

		return resultMat;
	}

	static Matrix4 createCuboidInertial(float mass, float width, float height, float depth) {
		float Ixx = 0.0833f*mass*(depth*depth + height*height);
		float Iyy = 0.0833f*mass*(width*width + depth*depth);
		float Izz = 0.0833f*mass*(width*width + height*height);

		Vector4 Row0(Ixx,	0,	 0,		0);
		Vector4 Row1(0,		Iyy, 0,		0);
		Vector4 Row2(0,		0,	 Izz,	0);
		Vector4 Row3(0,		0,	 0,		1);

		Matrix4 resultMat;
		resultMat.setRow(0, Row0);
		resultMat.setRow(1, Row1);
		resultMat.setRow(2, Row2);
		resultMat.setRow(3, Row3);

		return resultMat;
	}

	static Matrix4 createCuboidInvInertial(float mass, float width, float height, float depth) {
		float Ixx = 12.0f / (mass*(depth*depth + height*height));
		float Iyy = 12.0f / (mass*(width*width + depth*depth));
		float Izz = 12.0f / (mass*(width*width + height*height));
		
		Vector4 Row0(Ixx, 0, 0, 0);
		Vector4 Row1(0, Iyy, 0, 0);
		Vector4 Row2(0, 0, Izz, 0);
		Vector4 Row3(0, 0, 0, 1);

		Matrix4 resultMat;
		resultMat.setRow(0, Row0);
		resultMat.setRow(1, Row1);
		resultMat.setRow(2, Row2);
		resultMat.setRow(3, Row3);

		return resultMat;
	}

	static Matrix4 createImmovableInvInertial() 
	{
		Vector4 Row0(0, 0, 0, 0);
		Vector4 Row1(0, 0, 0, 0);
		Vector4 Row2(0, 0, 0, 0);
		Vector4 Row3(0, 0, 0, 1);

		Matrix4 resultMat;
		resultMat.setRow(0, Row0);
		resultMat.setRow(1, Row1);
		resultMat.setRow(2, Row2);
		resultMat.setRow(3, Row3);

		return resultMat;
	}

};