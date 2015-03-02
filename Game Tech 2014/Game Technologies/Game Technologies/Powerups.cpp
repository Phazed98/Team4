#include "Powerups.h"
#include "Vehicle.h"
#include "Coins.h"

Powerups::Powerups(){
	PowerupMesh = new OBJMesh(MESHDIR"cube.obj");

	//SpeedboostPosition = Vector3(0, 100, -300);

	InvulnerabilityPosition = Vector3(200, 100, -300);

	PickupmagnetPosition = Vector3(-200, 100, -300);

	size = 20.0f;

	//Speed_boost = BuildPowerupEntity(size, SpeedboostPosition);

	Invulnerability = BuildPowerupEntity(size, InvulnerabilityPosition);

	Pickup_magnet = BuildPowerupEntity(size, PickupmagnetPosition);

	Invulnerability->GetRenderNode().SetColour(Vector4(1, 0, 0, 1));

	Pickup_magnet->GetRenderNode().SetColour(Vector4(0, 1, 0, 1));

	tempPosition = Vector3(0, 350, -800);
}

Powerups::~Powerups(void){
	delete PowerupMesh;
	//delete Speed_boost;
	delete Invulnerability;
}


GameEntity* Powerups::BuildPowerupEntity(float size, Vector3 pos) {
	SceneNode* test = new SceneNode(PowerupMesh);
	test->SetModelScale(Vector3(size, size, size));
	test->SetBoundingRadius(size);
	test->SetColour(Vector4(0.2f, 0.2f, 0.5f, 1.0f));
	PhysicsNode*p = new PhysicsNode();
	p->SetMass(10);
	p->SetUseGravity(false);
	//pos = Vector3(0,100,-200);
	p->SetPosition(pos);
	//p->SetCollisionVolume(new CollisionSphere(size));    
	GameEntity*g = new GameEntity(test, p);
	g->ConnectToSystems();
	return g;
}

void Powerups::UpdatePowerup(Vehicle* Car, Coins* coins, float msec){
	/*if (Speed_boost != NULL){
	TestLength = (Speed_boost->GetPhysicsNode().GetPosition() - Car->GetPhysicsNode()->GetPosition()).Length();
	if (TestLength <= size + 4){
	SpeedboostTime = 200 * msec;
	Car->SetSpeedPlayer(2);
	ColorTemp = Speed_boost->GetRenderNode().GetColour();
	ColorTemp.w = 0;
	Speed_boost->GetRenderNode().SetColour(ColorTemp);
	}
	}

	if (SpeedboostTime <= msec){
	Car->SetSpeedPlayer(1);
	}*/

	if (Invulnerability != NULL){
		TestLength = (Invulnerability->GetPhysicsNode().GetPosition() - Car->GetPhysicsNode()->GetPosition()).Length();
		if (TestLength <= size + 4){
			InvulnerabilityTime = 200 * msec;
			Car->GetPhysicsNode()->SetCollisionVolume(NULL);
			ColorTemp = Invulnerability->GetRenderNode().GetColour();
			ColorTemp.w = 0;
			Invulnerability->GetRenderNode().SetColour(ColorTemp);
		}
	}

	if (InvulnerabilityTime <= msec){
		Car->GetPhysicsNode()->SetCollisionVolume(new CollisionSphere(4));
	}

	if (Pickup_magnet != NULL){
		TestLength = (Pickup_magnet->GetPhysicsNode().GetPosition() - Car->GetPhysicsNode()->GetPosition()).Length();
		if (TestLength <= size + 4){
			PickupmagnetTime = 20000 * msec;
		}
	}

	if (PickupmagnetTime >= msec){
		for (int i = 0; i < coins->GetCoinsnum(); i++){
			if ((Car->GetPhysicsNode()->GetPosition() - coins->GetAllcoins()[i]->GetPhysicsNode().GetPosition()).Length() <= 300){
				temp1 = Car->GetPhysicsNode()->GetPosition() - coins->GetAllcoins()[i]->GetPhysicsNode().GetPosition();
				temp1.Normalise();
				temp1 = temp1 * 0.2f;
				coins->GetAllcoins()[i]->GetPhysicsNode().SetLinearVelocity(temp1);
			}
			else{
				coins->GetAllcoins()[i]->GetPhysicsNode().SetLinearVelocity(Vector3(0, 0, 0));
			}
		}
	}
	else{
		for (int i = 0; i < coins->GetCoinsnum(); i++){
			coins->GetAllcoins()[i]->GetPhysicsNode().SetLinearVelocity(Vector3(0, 0, 0));
		}
	}


	//SpeedboostTime -= msec;
	InvulnerabilityTime -= msec;
	PickupmagnetTime -= msec;
}