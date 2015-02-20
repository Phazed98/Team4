#include "BillboardTexture.h"

Mesh* BillboardTexture::quad = NULL;

BillboardTexture::BillboardTexture(){
	if (!quad){
		CreateBillboard();
	}

	SetMesh(quad);

	SceneNode* grass = new SceneNode(quad, Vector4(1, 1, 0, 1));
	grass->SetModelScale(Vector3(10,10,10));
	grass->SetTransform(Matrix4::Translation(Vector3(0, 300, 0)));
	grass->SetBoundingRadius(100.0f);
	AddChild(grass);

	//billShader = new Shader(SHADERDIR"billboardVS.glsl", SHADERDIR"billboardFS.glsl", SHADERDIR"billboardGS.glsl");

}

void BillboardTexture::Update(float msec){
	
	SceneNode::Update(msec);
}