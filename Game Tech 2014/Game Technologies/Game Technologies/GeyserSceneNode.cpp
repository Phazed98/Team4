#include "GeyserSceneNode.h"

GeyserSceneNode::GeyserSceneNode(Mesh* waterMesh){
	this->mesh = waterMesh ;
	geyNode = new SceneNode();

	AddChild(geyNode);

}

void GeyserSceneNode::Update(float msec){
	delta_time_millis = msec;

	SceneNode::Update(msec);
}