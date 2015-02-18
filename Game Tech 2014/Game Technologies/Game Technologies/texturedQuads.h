#pragma once
#include "../../nclgl/SceneNode.h"
#include "../../nclgl/OBJMesh.h"
#include "../../nclgl/common.h"
#include "../../nclgl/Mesh.h"

class textureQuads : public SceneNode{

public :
	textureQuads(float x, float y, float z, int size);
	~textureQuads();

	virtual void Update(float msec);

	static void CreateQuad(){
		Mesh* m = Mesh::GenerateQuad();
		GLuint tex = SOIL_load_OGL_texture("../../Textures/bbgrass.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
		m->SetTexture(tex);
		quad = m;
	}

	static void DeleteQuad(){
		delete quad;
	}

protected:
	
	static Mesh* quad;
	float x_coord;
	float y_coord;
	float z_coord;

	SceneNode* grassQuad;
};