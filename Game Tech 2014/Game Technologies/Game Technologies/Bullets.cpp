#include "Bullets.h"

Bullets::Bullets(Vector3 pos){
	BulletsPosition = pos;
	BulletsMesh = new OBJMesh(MESHDIR"ico.obj");
	Bullet0 = BuildBulletEntity(2,BulletsPosition);
	Bullet0->GetPhysicsNode().SetLinearVelocity(Vector3(0,0,-1));

}

Bullets::~Bullets(void){
	delete Bullet0;

}

GameEntity* Bullets::BuildBulletEntity(float radius, Vector3 pos) {
	SceneNode* test = new SceneNode(BulletsMesh);
	test->SetModelScale(Vector3(radius,radius,radius));
	test->SetBoundingRadius(radius);
	test->SetColour(Vector4(0.2,0.2,0.5,1));
	PhysicsNode*p = new PhysicsNode();

	p->SetUseGravity(false);
	p->SetPosition(pos);
	p->SetCollisionVolume(new CollisionSphere(radius));    //new 4.2.2015  Daixi


	GameEntity*g = new GameEntity(test, p);
	g->ConnectToSystems();
	return g;
}

void Bullets::ShootBullets(){
	Bullet0->GetPhysicsNode().SetPosition(BulletsPosition);
	Bullet0->GetPhysicsNode().SetLinearVelocity(Vector3(0,0,-1));
}
