#pragma once

#include "../../nclgl/SceneNode.h"
#include "FireParticleSystem.h"
#include "../../nclgl/Matrix4.h"


class SpaceshipSceneNode :public SceneNode{
public:
	SpaceshipSceneNode(Mesh* shipMesh, Camera* camera, Matrix4 pm);
//	virtual void Draw(const OGLRenderer & r);
	virtual void	Update(float msec);

	FireParticleSystem afterburner_system[2];
	SceneNode* GetAfterburnerNode(){ return afterburnerNode; }
private:
	SceneNode * root;
	SceneNode * afterburnerNode;
	
	Matrix4 view_matrix;
	Matrix4 project_matrix;
	float delta_time_millis;
	Camera* camera;
};