#include"Vehicle.h"

Vehicle::Vehicle(){
	PlayerMesh = new OBJMesh(MESHDIR"Player.obj");

	PlayerPosition=Vector3(0,100,-400);

	Player = BuildPlayerEntity(20.0f,PlayerPosition);

	Speed_Player = 1;

	Speed_Rotate = -0.0004;

	Speed_Reduce = 0.00013;

	tempPosition = Vector3(0,350,-800);
}

Vehicle::~Vehicle(void){
	delete PlayerMesh;
	delete Player;
}

void Vehicle::UpdatePlayer(float msec){
	PlayerPosition = Player->GetPhysicsNode().GetPosition();

	if(Player->GetPhysicsNode().GetOrientation().z<-0.3||Player->GetPhysicsNode().GetOrientation().z>0.3){
		Player->GetPhysicsNode().SetAngularVelocity(Vector3(0,0,0));
	}

	if(Player->GetPhysicsNode().GetPosition().y<=100){
		temp1 = Player->GetPhysicsNode().GetLinearVelocity();
		temp1.y = 0;
		Player->GetPhysicsNode().SetLinearVelocity(temp1);
		Player->GetPhysicsNode().SetUseGravity(FALSE);
	}

	if(Window::GetKeyboard()->KeyDown(KEYBOARD_A)) {
		if(Player->GetPhysicsNode().GetOrientation().z>=-0.33){
			Player->GetPhysicsNode().SetAngularVelocity(Vector3(0,0,Speed_Rotate));
		}			
		temp1 = Player->GetPhysicsNode().GetLinearVelocity();
		temp1.x=0.1*Speed_Player;
		Player->GetPhysicsNode().SetLinearVelocity(temp1);
		tempPosition.x = PlayerPosition.x;
		//temp = PlayerPosition - BuffEntity->GetPhysicsNode().GetPosition();//new just for buff    6.2.2015
		//if(temp.Length()<=20)
		//{
		//	Speed_Player = 2;
		//}
	}

	if(Window::GetKeyboard()->KeyDown(KEYBOARD_D)) {
		if(Player->GetPhysicsNode().GetOrientation().z<=0.33){
			Player->GetPhysicsNode().SetAngularVelocity(Vector3(0,0,-Speed_Rotate));
		}
		temp1 = Player->GetPhysicsNode().GetLinearVelocity();
		temp1.x=-0.1*Speed_Player;
		Player->GetPhysicsNode().SetLinearVelocity(temp1);
		tempPosition.x = PlayerPosition.x;
		//temp = PlayerPosition - BuffEntity->GetPhysicsNode().GetPosition();//new just for buff    6.2.2015
		//if(temp.Length()<=20)
		//{
		//	Speed_Player = 2;
		//}
	}

	if(Window::GetKeyboard()->KeyDown(KEYBOARD_W)) {
		temp1 = Player->GetPhysicsNode().GetLinearVelocity();
		temp1.z=0.1*Speed_Player;
		Player->GetPhysicsNode().SetLinearVelocity(temp1);             
		tempPosition.z = PlayerPosition.z-400; //new 5.2.2015 make the camera position follow the Player position   Daixi
		//temp = PlayerPosition - BuffEntity->GetPhysicsNode().GetPosition();//new just for buff    6.2.2015
		//if(temp.Length()<=20)
		//{
		//	Speed_Player = 2;
		//}
	}

	if(Window::GetKeyboard()->KeyDown(KEYBOARD_S)) {
		temp1 = Player->GetPhysicsNode().GetLinearVelocity();
		temp1.z=-0.1*Speed_Player;
		Player->GetPhysicsNode().SetLinearVelocity(temp1);             
		tempPosition.z = PlayerPosition.z-400;
		//temp = PlayerPosition - BuffEntity->GetPhysicsNode().GetPosition();    //new just for buff    6.2.2015
		//if(temp.Length()<=20)
		//{
		//	Speed_Player = 2;
		//}
	}

	if(!Window::GetKeyboard()->KeyDown(KEYBOARD_D)&&!Window::GetKeyboard()->KeyDown(KEYBOARD_A)) {		
		temp1 = Player->GetPhysicsNode().GetLinearVelocity();
		if(Player->GetPhysicsNode().GetOrientation().z<-0.002||Player->GetPhysicsNode().GetOrientation().z>0.002){
			if(Player->GetPhysicsNode().GetOrientation().z<0){
				Player->GetPhysicsNode().SetAngularVelocity(Vector3(0,0,-Speed_Rotate));
				//temp1.x = -temp1.x * (Player->GetPhysicsNode().GetOrientation().z/0.3);
				//Player->GetPhysicsNode().SetLinearVelocity(temp1);
			}
			if(Player->GetPhysicsNode().GetOrientation().z>0){
				Player->GetPhysicsNode().SetAngularVelocity(Vector3(0,0,Speed_Rotate));
				//temp1.x = temp1.x * (Player->GetPhysicsNode().GetOrientation().z/0.3);
				//Player->GetPhysicsNode().SetLinearVelocity(temp1);

			}
		}
		else{
			Player->GetPhysicsNode().SetAngularVelocity(Vector3(0,0,0));
			temp1.x=0;
			Player->GetPhysicsNode().SetLinearVelocity(temp1);
		}
		tempPosition.x = PlayerPosition.x;
	}
	if(!Window::GetKeyboard()->KeyDown(KEYBOARD_W)&&!Window::GetKeyboard()->KeyDown(KEYBOARD_S)) {		
		temp1 = Player->GetPhysicsNode().GetLinearVelocity();
		temp1.z=0;
		Player->GetPhysicsNode().SetLinearVelocity(temp1);
	}
	if(Player->GetPhysicsNode().GetLinearVelocity().y==0){
		if(Window::GetKeyboard()->KeyDown(KEYBOARD_SPACE)){
			temp1 = Player->GetPhysicsNode().GetLinearVelocity();
			Player->GetPhysicsNode().SetInverseMass(10);
			Player->GetPhysicsNode().SetUseGravity(TRUE);
			temp1.y = 0.6;
			Player->GetPhysicsNode().SetLinearVelocity(temp1);	
		}
	}

}

/*
Makes a Player. Every game has a crate in it somewhere!
*/


GameEntity* Vehicle::BuildPlayerEntity(float size, Vector3 pos) {   
	SceneNode* test = new SceneNode(PlayerMesh);
	test->SetModelScale(Vector3(size,size,size));
	test->SetBoundingRadius(size);
	test->SetColour(Vector4(0.2,0.2,0.5,1));
	PhysicsNode*p = new PhysicsNode();
	p->SetMass(10);
	p->SetUseGravity(false);
	pos = Vector3(0,100,-400);
	p->SetPosition(pos);
	p->SetCollisionVolume(new CollisionSphere(size));    
	GameEntity*g = new GameEntity(test, p);
	g->ConnectToSystems();
	return g;
}