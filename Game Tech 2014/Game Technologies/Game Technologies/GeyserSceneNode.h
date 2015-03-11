#pragma once

#include "../../nclgl/SceneNode.h"
#include "FireParticleSystem.h"
#include "EarthParticleSystem.h"
#include "../../nclgl/Matrix4.h"



class GeyserSceneNode : public SceneNode{

public:
	GeyserSceneNode(Mesh* waterCube);

	virtual void Update(float msec);


	SceneNode* GetGeyserNode(){ return geyNode; }
	void SetFireoNode(SceneNode* s){ geyNode = s; }
	//void SetTornadoPos(Vector3* pos){ currentPosition = pos;}
	//Vector3* GetTornadoPos(){return currentPosition;}
	//void SetTornadoSpeed(float* z){speed = z;}

private:
	SceneNode* root;
	SceneNode* geyNode;
	float* speed;

	Vector3* currentPosition;
	float delta_time_millis;
	float* newZ;
};