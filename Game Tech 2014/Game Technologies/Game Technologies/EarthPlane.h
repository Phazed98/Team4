#pragma once
#include "../../nclgl/Mesh.h"
#include "../../nclgl/SceneNode.h"

class EarthPlane : public SceneNode{

public:
	EarthPlane(float x, float y, float z, int size);
	~EarthPlane();

	virtual void Update(float msec);
	static void CreateMap(){
		Mesh* m = Mesh::GenerateQuad();
		GLuint tex = SOIL_load_OGL_texture("../../Textures/EarthPlane1.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
		m->SetTexture(tex);

	

		plane = m;

	}

	static void DeletePlane(){
			delete plane;

	}



protected:
	static Mesh* plane;
	float x_coord;
	float y_coord;
	float z_coord;
	SceneNode* floor;

};