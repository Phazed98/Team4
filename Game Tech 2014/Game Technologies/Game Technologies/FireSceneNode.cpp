#include "FireSceneNode.h"

FireSceneNode::FireSceneNode(Mesh* fireMesh){
	this->mesh = fireMesh;
	//currentPosition = new Vector3();
	fireNode = new SceneNode();
	//newZ = new float(-750.0f);
	//speed = new float(0);

	AddChild(fireNode);
	//float z = PhysicsSystem::GetPhysicsSystem().GetTrackSpeed();

	//tornado_system.InitParticleSystem(0, Vector3(tornadoNode->GetWorldTransform().GetPositionVector().x, 100, -200 ));
	//tornado_system.InitParticleSystem(0, GetTornadoPos());



	//tornado_system.InitParticleSystem(0, Vector3(0, -100, int(speed)));
}

void FireSceneNode::Update(float msec){
	delta_time_millis = msec;

	//tornado_system.InitParticleSystem(0, Vector3(0, 100, *newZ));
	SceneNode::Update(msec);
}