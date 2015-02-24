#include "Coins.h"
#include "Vehicle.h"

Coins::Coins(int coinsnum){
	Coins_num = coinsnum;
	coin_size = 6;
	CoinsMesh = new OBJMesh(MESHDIR"ico.obj");
	for (int i = 0; i < Coins_num; i++){
		Allcoins.push_back(BuildCoinsEntity(coin_size));
		Allcoins[i]->GetPhysicsNode().SetPosition(Vector3(0,100,0));
	}

	colourtemp1 = Vector4(0.2, 0.2, 0.5, 1);
}

Coins::~Coins(void){
	for (vector<GameEntity*>::iterator i = Allcoins.begin(); i != Allcoins.end(); ++i) {
		delete (*i);
	}
}

GameEntity* Coins::BuildCoinsEntity(float radius) {
	SceneNode* test = new SceneNode(CoinsMesh);
	test->SetModelScale(Vector3(radius, radius, radius));
	test->SetBoundingRadius(radius);
	test->SetColour(Vector4(0.2, 0.2, 0.5, 1));
	PhysicsNode*p = new PhysicsNode();

	p->SetMass(10);
	p->SetUseGravity(false);

	GameEntity*g = new GameEntity(test, p);
	g->ConnectToSystems();
	return g;
}

void Coins::UpdateCoins(Vehicle* car){

	srand((unsigned)time(NULL));

	for (int i = 0; i < Coins_num; i++){
		random_num0 = rand() % 600 - 200;
		random_num1 = rand() % 1200 ;
		temp = car->GetPlayer()->GetPhysicsNode().GetPosition();
		temp.x = random_num0;
		temp.z = temp.z + abs(random_num1);
		Allcoins[i]->GetPhysicsNode().SetPosition(temp);
		Allcoins[i]->GetRenderNode().SetColour(colourtemp1);
	}
	
}

void Coins::UpdatePickupCoins(Vehicle* car){
	for (int i = 0; i < Coins_num; i++){
		Test_length = (Allcoins[i]->GetPhysicsNode().GetPosition() - car->GetPlayer()->GetPhysicsNode().GetPosition()).Length();
		if (Test_length <= coin_size + car->GetSize())
		{
			colourtemp1 = Allcoins[i]->GetRenderNode().GetColour();
			colourtemp1.w = 0;
			Allcoins[i]->GetRenderNode().SetColour(colourtemp1);
			Allcoins[i]->GetPhysicsNode().SetPosition(Vector3(0,0,0));
			colourtemp1.w = 1;
		}
	}
}
