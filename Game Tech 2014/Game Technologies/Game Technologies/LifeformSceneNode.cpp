#include "LifeformSceneNode.h"

LifeformSceneNode::LifeformSceneNode(Mesh* waterMesh){
	this->mesh = waterMesh;
	lifeNode = new SceneNode();

	AddChild(lifeNode);

}

void LifeformSceneNode::Update(float msec){
	delta_time_millis = msec;

	SceneNode::Update(msec);
}