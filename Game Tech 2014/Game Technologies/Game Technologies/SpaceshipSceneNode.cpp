#include "SpaceshipSceneNode.h"

SpaceshipSceneNode::SpaceshipSceneNode(Mesh* shipMesh, Camera* camera, Matrix4 pm){

	//root = new SceneNode(shipMesh);

	this->mesh = shipMesh;
//	view_matrix = vm;
//	project_matrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);

	
	afterburnerNode = new SceneNode();//without mesh
	AddChild(afterburnerNode);

//	afterburnerNode->SetTransform(Matrix4::Rotation(-90,Vector3(1,0,0)));


	afterburner_system[0].InitParticleSystem(2, Vector3(-3.2, -2, 8.5), camera, project_matrix);

	afterburner_system[1].InitParticleSystem(2, Vector3( 3.2, -2, 8.5), camera, project_matrix);
}

//void SpaceshipSceneNode::Draw(const OGLRenderer &r){
//	//draw the afterburner
//	/*Matrix4 model_matrix = afterburnerNode->GetWorldTransform();
//	afterburner_system[0].Render(delta_time_millis, model_matrix);
//	afterburner_system[1].Render(delta_time_millis, model_matrix);*/
//	//draw itself
//	mesh->Draw();
//}

void SpaceshipSceneNode::Update(float msec){
//	project_matrix = 
	//update the delta_time_millis
	delta_time_millis = msec;
	//update afterburner world location
	SceneNode::Update(msec);
}