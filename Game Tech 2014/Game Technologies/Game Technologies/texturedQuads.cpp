#include "texturedQuads.h"

Mesh* texturedQuads::quad = NULL;

textureQuads::textureQuads(float x, float y, float z, int size) : x_coord(x), y_coord(y), z_coord(z){
	
	SetBoundingRadius(10000);
	grassQuad = new SceneNode(quad);

	grassQuad->SetModelScale(Vector3(size, size, 1));
	grassQuad->SetBoundingRadius(1000);

	grassQuad->SetTransform(Matrix4::Translation(Vector3(x_coord, y_coord, z_coord)));

	AddChild(grassQuad);
}