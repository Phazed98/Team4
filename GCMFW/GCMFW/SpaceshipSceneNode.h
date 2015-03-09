#pragma once

#include "SceneNode.h"
//#include "FireParticleSystem.h"
#include <vectormath/cpp/vectormath_aos.h>
#include <vectormath/cpp/quat_aos.h>

using namespace Vectormath::Aos;
class ChaseCamera;

//this is the space ship render node
//basically it is for the particle system of the ship

class SpaceshipSceneNode :public SceneNode{
public:
	SpaceshipSceneNode(Mesh* shipMesh);
	//	virtual void Draw(const OGLRenderer & r);
	virtual void	Update(float msec);

//	FireParticleSystem afterburner_system[2];
//	SceneNode* GetAfterburnerNode(){ return afterburnerNode; }
private:
	SceneNode* root;
//	SceneNode* afterburnerNode;

	Matrix4 view_matrix;
	Matrix4 project_matrix;
	float delta_time_millis;
};