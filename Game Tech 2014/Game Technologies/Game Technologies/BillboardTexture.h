#pragma once 
#include "../../nclgl/SceneNode.h"
#include "../../nclgl/Mesh.h"
#include "../../nclgl/common.h"


class BillboardTexture : public SceneNode{

public:
	
	BillboardTexture();
	virtual ~BillboardTexture(){};

	virtual void Update(float msec);

	static void CreateBillboard(){
		Mesh* m = Mesh::GenerateQuad();
		
		quad = m;
	}

	static void DeleteBillboard(){
		delete quad;
	}

protected:
	static Mesh* quad;
	Shader* billShader;

	
};