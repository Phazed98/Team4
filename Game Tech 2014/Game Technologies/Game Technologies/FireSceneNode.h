#pragma once

#include "../../nclgl/SceneNode.h"
#include "FireParticleSystem.h"
#include "EarthParticleSystem.h"
#include "../../nclgl/Matrix4.h"



class FireSceneNode : public SceneNode{

public:
	FireSceneNode(Mesh* fireCube);

	virtual void Update(float msec);


	SceneNode* GetFireNode(){ return fireNode; }
	void SetFireoNode(SceneNode* s){ fireNode = s; }
	//void SetTornadoPos(Vector3* pos){ currentPosition = pos;}
	//Vector3* GetTornadoPos(){return currentPosition;}
	//void SetTornadoSpeed(float* z){speed = z;}

private:
	SceneNode* root;
	SceneNode* fireNode;
	float* speed;

	Vector3* currentPosition;
	float delta_time_millis;
	float* newZ;
};