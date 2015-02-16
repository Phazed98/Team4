#include "Powerups.h"
#include "Vehicle.h"

Powerups::Powerups(){
	PowerupMesh = new OBJMesh(MESHDIR"ico.obj");

	SpeedboostPosition=Vector3(0,100,-200);

	InvulnerabilityPosition = Vector3(200,100,-200);

	size = 20.0f;

	Speed_boost = BuildPowerupEntity(size,SpeedboostPosition);

	Invulnerability = BuildPowerupEntity(size,InvulnerabilityPosition);

	Invulnerability->GetRenderNode().SetColour(Vector4(1,0,0,1));

	tempPosition = Vector3(0,350,-800);
}

Powerups::~Powerups(void){
	delete PowerupMesh;
	delete Speed_boost;
	delete Invulnerability;
}


GameEntity* Powerups::BuildPowerupEntity(float size, Vector3 pos) {   
	SceneNode* test = new SceneNode(PowerupMesh);
	test->SetModelScale(Vector3(size,size,size));
	test->SetBoundingRadius(size);
	test->SetColour(Vector4(0.2,0.2,0.5,1));
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

void Powerups::UpdatePowerup(Vehicle* Car, float msec){
	if(Speed_boost!=NULL){
		TestLength = (Speed_boost->GetPhysicsNode().GetPosition() - Car->GetPlayer()->GetPhysicsNode().GetPosition()).Length();
		if(TestLength<=size+Car->GetSize()){
			SpeedboostTime = 200*msec;
			Car->SetSpeedPlayer(2);
			ColorTemp = Speed_boost->GetRenderNode().GetColour();
			ColorTemp.w = 0;
			Speed_boost->GetRenderNode().SetColour(ColorTemp);
			
			
			//SoundSystem::GetSoundSystem() -> PlaySoundA(SoundManager::GetSound("D:/Team 4/Team4/Game Tech 2014/Sounds/36847__ecodtr__laserrocket2.wav"), Vector3());

		}
	}

	if(SpeedboostTime<=msec){
		Car->SetSpeedPlayer(1);
	}

	if(Invulnerability!=NULL){
		TestLength = (Invulnerability->GetPhysicsNode().GetPosition() - Car->GetPlayer()->GetPhysicsNode().GetPosition()).Length();
		if(TestLength<=size+Car->GetSize()){
			InvulnerabilityTime = 200*msec;
			Car->GetPlayer()->GetPhysicsNode().SetCollisionVolume(NULL);
			ColorTemp = Invulnerability->GetRenderNode().GetColour();
			ColorTemp.w = 0;
			Invulnerability->GetRenderNode().SetColour(ColorTemp);
		}
	}

	if(InvulnerabilityTime<=msec){
		Car->GetPlayer()->GetPhysicsNode().SetCollisionVolume(new CollisionSphere(size));
	}
	SpeedboostTime -= msec;
	InvulnerabilityTime -=msec;
}