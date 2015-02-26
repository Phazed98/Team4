#include "SpaceshipSceneNode.h"

SpaceshipSceneNode::SpaceshipSceneNode(Mesh* shipMesh){

	this->mesh = shipMesh;

	afterburnerNode = new SceneNode();//without mesh
	AddChild(afterburnerNode);



	afterburner_system[0].InitParticleSystem(2, Vector3(-3.2, -2, 8.5));

	afterburner_system[1].InitParticleSystem(2, Vector3( 3.2, -2, 8.5));
}


void SpaceshipSceneNode::Update(float msec){
	//update the delta_time_millis
	delta_time_millis = msec;
	//update afterburner world location
	SceneNode::Update(msec);
}