#pragma once

#include "../../nclgl/SceneNode.h"
#include "FireParticleSystem.h"
#include "EarthParticleSystem.h"
#include "../../nclgl/Matrix4.h"



class LifeformSceneNode : public SceneNode{

public:
	LifeformSceneNode(Mesh* earthCube);

	virtual void Update(float msec);


	SceneNode* GetLifeFormSceneNode(){ return lifeNode; }
	void SetFireoNode(SceneNode* s){ lifeNode = s; }
	//void SetTornadoPos(Vector3* pos){ currentPosition = pos;}
	//Vector3* GetTornadoPos(){return currentPosition;}
	//void SetTornadoSpeed(float* z){speed = z;}

private:
	SceneNode* root;
	SceneNode* lifeNode;
	float* speed;

	Vector3* currentPosition;
	float delta_time_millis;
	float* newZ;
};