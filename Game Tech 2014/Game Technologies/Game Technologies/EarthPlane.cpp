#include "EarthPlane.h"
#include "MyGame.h"

Mesh* EarthPlane::plane = NULL;


EarthPlane::EarthPlane(float x, float y, float z, int size) : x_coord(x), y_coord(y), z_coord(z){

	SetBoundingRadius(10000);
	
	floor = new SceneNode(plane);

	floor->SetModelScale(Vector3(size, size, 1));
	floor->SetBoundingRadius(10000);

	floor->SetTransform(Matrix4::Translation(Vector3(x_coord, y_coord, z_coord)));


}

void EarthPlane::Update(float msec){

	


}