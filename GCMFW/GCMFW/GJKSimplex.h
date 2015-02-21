#pragma once
#include <vectormath/cpp/vectormath_aos.h>
using namespace Vectormath::Aos;

//Created by Sam
//Stores GJK History between frames, used to optimise GJK algorithm

class GJKSimplex
{
public:
	GJKSimplex();

	//Setters
	//flag should be set upon storage
	void SetActiveStore(bool flag);
	//Setters for the support point indexes
	void SetPointA(int &pointAIndex0, int &pointAIndex1);
	void SetPointB(int &pointBIndex0, int &pointBIndex1);
	void SetPointC(int &pointCIndex0, int &pointCIndex1);
	void SetPointD(int &pointDIndex0, int &pointDIndex1);
	//Setter for Direction Vector


	//getters
	int GetActiveStore() { return activeStore; }
	//C++ is a PITA so need two seperate getters per point. Will be quicker than creating a heap vector/array, passing, then deleting. 
	int GetPointAIndex0() { return pointAIndex0; }
	int GetPointAIndex1() { return pointAIndex1; }

	int GetPointBIndex0() { return pointBIndex0; }
	int GetPointBIndex1() { return pointBIndex1; }

	int GetPointCIndex0() { return pointCIndex0; }
	int GetPointCIndex1() { return pointCIndex1; }

	int GetPointDIndex0() { return pointDIndex0; }
	int GetPointDIndex1() { return pointDIndex1; }


private:
	//boolean to flag if there is a stored simplex.
	bool activeStore;
	//the two indexes repressent the index in object 0 and object 1 in the GJK algorithm.
	int pointAIndex0, pointAIndex1;
	int pointBIndex0, pointBIndex1;
	int pointCIndex0, pointCIndex1;
	int pointDIndex0, pointDIndex1;

};

GJKSimplex::GJKSimplex()
{
	activeStore = false;
}

