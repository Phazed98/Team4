#pragma once

#include "../../nclgl/SceneNode.h"
#include "EarthParticleSystem.h"
#include "../../nclgl/Matrix4.h"



class TornadoSceneNode : public SceneNode{

public:
	TornadoSceneNode(Mesh* airCube);

	virtual void Update(float msec);

	
	SceneNode* GetTornadoNode(){return tornadoNode ;}
	void SetTornadoNode(SceneNode* s){tornadoNode = s;}
	//void SetTornadoPos(Vector3* pos){ currentPosition = pos;}
	//Vector3* GetTornadoPos(){return currentPosition;}
	//void SetTornadoSpeed(float* z){speed = z;}

private:
	SceneNode* root;
	SceneNode* tornadoNode;
	float* speed ;
	
	Vector3* currentPosition;
	float delta_time_millis;
	float* newZ ;
};