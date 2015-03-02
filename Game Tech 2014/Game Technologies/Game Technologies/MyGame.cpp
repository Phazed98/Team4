#include "MyGame.h"
#include "InertialMatrixHelper.h"
#include "Spring.h"
#include "SpringDemo.h"
#include "NetworkClient.h"
#include "Powerups.h"
#include "Coins.h"
#include "ChaseCamera.h"


/*
Creates a really simple scene for our game - A cube robot standing on
a floor. As the module progresses you'll see how to get the robot moving
around in a physically accurate manner, and how to stop it falling
through the floor as gravity is added to the scene. 

You can completely change all of this if you want, it's your game!
*/

unsigned int MyGame::client_id;

MyGame::MyGame()	
{
	if (USE_NETWORKING)
	{
		if (IS_HOST)
		{
			//Init client list to nothing
			client_id = 0;

			//Spawn Server Which listens for clients
			networkServer = new NetworkServer();
		}

		if (IS_CLIENT)
		{
			//Spawn a New Client which will attempt to connect to a server
			networkClient = new NetworkClient();

			// send init packet
			const unsigned int packet_size = sizeof(Packet);
			char packet_data[packet_size];

			Packet packet;
			packet.packet_type = INIT_CONNECTION;
			packet.serialize(packet_data);
			NetworkServices::sendMessage(networkClient->ConnectSocket, packet_data, packet_size);
		}
	}


	Renderer::GetRenderer().RenderLoading(0, "Initializing...");
	setCurrentState(Game_LOADING);
	timer = 0;

	count_time = 0;    //new control shoot the bullets   4.2.2015 Daixi

	Speed_Player = 1;  //control the player speed

	Renderer::GetRenderer().RenderLoading(5, "Loading Elements...");

	elements.push_back(top);
	elements.push_back(right);
	elements.push_back(bottom);
	elements.push_back(left);

	Renderer::GetRenderer().RenderLoading(15,"Feeding Hamsters..." );

	reference.push_back(top);
	reference.push_back(right);
	reference.push_back(bottom);
	reference.push_back(left);

	Renderer::GetRenderer().RenderLoading(25,"Creating Obtsacles...");

	obstacleElements.resize(4);
	obstacleReference.resize(4);

	Renderer::GetRenderer().RenderLoading(30,"Referencing Obstacles..."); 
	for (int i = 0; i < 4; i++)
	{
		obstacleReference[i] = NULL;
	}
	Renderer::GetRenderer().RenderLoading(40,"Attaching Mooses...");

	Vehicle* player = PhysicsSystem::GetVehicle();
	gameCamera = new ChaseCamera(player, 200, 20, 12);


	Renderer::GetRenderer().RenderLoading(50, "I knew A Moose Once...");

	Renderer::GetRenderer().SetCamera(gameCamera);

	Renderer::GetRenderer().RenderLoading(70, "He was a really good moose...");

	Renderer::GetRenderer().RenderLoading(75, "The moose never bit anyone...");
	Renderer::GetRenderer().RenderLoading(76, "I called him George...");
	Renderer::GetRenderer().RenderLoading(91, "Sorry about that...");
	Renderer::GetRenderer().RenderLoading(92, "The person responsible for the loading screen...");
	Renderer::GetRenderer().RenderLoading(93, "...has been sacked...");

	//add all powerup variable!!!
	//AllCoins = new Coins(10);
	//AllPowerups = new Powerups();

	cube	= new OBJMesh(MESHDIR"cube.obj");
	quad	= Mesh::GenerateQuad();
	sphere	= new OBJMesh(MESHDIR"ico.obj");
	Renderer::GetRenderer().RenderLoading(95, "Lammas are nice creatures...");
	Renderer::GetRenderer().RenderLoading(96,"Hold it,  Were not getting into Lammas now...");
	Renderer::GetRenderer().RenderLoading(97, "The loading system is obviously faulty, we will continue now without any further updates...");
	Renderer::GetRenderer().RenderLoading(98, "Except this to say we love Mooses? Meese? Multiple Moose? ...");

	//-------------------------------------------------Planes---------------------------------------------------------//

	for (int i = 0; i < 7; i++)
	{
		ObjectType* plane = BuildObjectEntity(0, 2);
		plane->SetPos(Vector3(0, -500, -i * 800));
		if (i != 6)
			plane->setState(2);
		allEntities.push_back(plane);
	}

	for (int i = 0; i < 7; i++)
	{
		ObjectType* plane = BuildObjectEntity(0, 0);
		plane->SetPos(Vector3(0, 500, -i * 800));
		if (i != 6)
			plane->setState(2);
		allEntities.push_back(plane);
	}

	for (int i = 0; i < 7; i++)
	{
		ObjectType* plane = BuildObjectEntity(0, 1);
		plane->SetPos(Vector3(500, 0, -i * 800));
		if (i != 6)
			plane->setState(2);
		allEntities.push_back(plane);
	}

	for (int i = 0; i < 7; i++)
	{
		ObjectType* plane = BuildObjectEntity(0, 3);
		plane->SetPos(Vector3(-500, 0, -i * 800));
		if (i != 6)
			plane->setState(2);
		allEntities.push_back(plane);
	}

	setCurrentState(GAME_PLAYING);
	Renderer::GetRenderer().RenderLoading(100, "Done...");
	if (USE_NETWORKING && IS_HOST)
	{
		while (client_id <= 0)
		{
			// get new clients
			if (networkServer->acceptNewClient(client_id))
			{
				cout << "Client " << client_id << " has been connected to the server." << endl;
				client_id++;
			}
		}
	}
}

MyGame::~MyGame(void)	
{
	/*
	We're done with our assets now, so we can delete them
	*/
	delete cube;
	delete quad;
	delete sphere;
//	delete Enemy;   //new 5.2.2015 Daixi
	delete Car;
	delete bullet;
	delete AllCoins;
	delete AllPowerups;

	CubeRobot::DeleteCube();

	elements.clear();
	reference.clear();
	obstacleElements.clear();
	obstacleReference.clear();

	//GameClass destructor will destroy your entities for you...
	setCurrentState(GAME_EXIT);
}

/*
Here's the base 'skeleton' of your game update loop! You will presumably
want your games to have some sort of internal logic to them, and this
logic will be added to this function.
*/
void MyGame::UpdateGame(float msec) 
{
	if (currentGameState != GAME_PLAYING)
		return;

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_L)) //Build Cube
	{
		PhysicsSystem::SetTrackSpeed(PhysicsSystem::GetTrackSpeed() + 1);
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_K)) //Build Cube
	{
		PhysicsSystem::SetTrackSpeed(PhysicsSystem::GetTrackSpeed() - 1);
	}

	if(gameCamera) 
	{
		gameCamera->UpdateCamera(msec);
	}

	for(vector<GameEntity*>::iterator i = allEntities.begin(); i != allEntities.end(); ++i) 
	{
		(*i)->Update(msec);
	}


	Car = PhysicsSystem::GetPhysicsSystem().GetVehicle();
	/*
	AllPowerups->UpdatePowerup(Car, AllCoins, msec);
	AllCoins->UpdateCoins(Car);
	AllCoins->UpdatePickupCoins(Car);
	*/

	timer += 0.001;


	if (!USE_NETWORKING)
	{
		handlePlanes(timer);
	}

	if (USE_NETWORKING)
	{

		resetNumber = rand() % 100 + 1;
		randomPlane = rand() % 5 + 1;
		createObstacleNumber = rand() % 100 + 1;
		objectRandValue = rand() % 100 + 1;
		objectRandPlane = rand() % 5 + 1;
		obstacleRandNum = rand() % 2 + 1;

		if (IS_HOST)
		{
			//handlePlanesNetwork(resetNumber, randomPlane, createObstacleNumber, objectRandValue, objectRandPlane, obstacleRandNum);
			//handlePlanes(timer);
		}

		handleNetworking();
	}
}

GameEntity* MyGame::BuildPlayerEntity(float size, Vector3 pos)
{
	SceneNode* test = new SceneNode(PlayerMesh);
	test->SetModelScale(Vector3(size, size, size));
	test->SetBoundingRadius(size);
	test->SetColour(Vector4(0.2, 0.2, 0.5, 1));
	PhysicsNode*p = new PhysicsNode();

	p->SetUseGravity(false);
	pos = Vector3(0, 100, -400);
	p->SetPosition(pos);
	p->SetCollisionVolume(new CollisionSphere(size));


	GameEntity*g = new GameEntity(test, p);
	g->ConnectToSystems();
	return g;
}

/*
Makes Buff. it can speed up Player or slow down!
*/
GameEntity* MyGame::BuildBuffEntity(float radius, Vector3 pos)
{   //new 6.2.2015 Daixi
	SceneNode* test = new SceneNode(sphere);
	test->SetModelScale(Vector3(radius, radius, radius));
	test->SetBoundingRadius(radius);
	test->SetColour(Vector4(0.2, 0.2, 0.5, 1));
	PhysicsNode*p = new PhysicsNode();

	p->SetUseGravity(false);
	p->SetPosition(pos);

	GameEntity*g = new GameEntity(test, p);
	g->ConnectToSystems();
	return g;
}

/*
Makes an entity that looks like a CubeRobot! You'll probably want to modify
this so that you can have different sized robots, with different masses and
so on!
*/
GameEntity* MyGame::BuildRobotEntity() 
{
	GameEntity*g = new GameEntity(new CubeRobot(), new PhysicsNode());

	g->GetPhysicsNode().SetUseGravity(false);
	g->ConnectToSystems();
	return g;
}

/*
Makes a cube. Every game has a crate in it somewhere!
*/
GameEntity* MyGame::BuildCubeEntity(float size)
{
	GameEntity*g = new GameEntity(new SceneNode(cube), new PhysicsNode());
	SceneNode &test = g->GetRenderNode();

	test.SetModelScale(Vector3(size,size,size));
	test.SetBoundingRadius(size);

	g->ConnectToSystems();
	return g;
}
/*
Makes a sphere.
*/
GameEntity* MyGame::BuildSphereEntity(float radius, Vector3 pos, Vector3 vel) 
{
	SceneNode* s = new SceneNode(sphere);

	s->SetModelScale(Vector3(radius,radius,radius));
	s->SetBoundingRadius(radius);
	s->SetColour(Vector4(0.0,1.0,0.5,1));
	PhysicsNode*p = new PhysicsNode();
	p->SetPosition(pos);
	p->SetLinearVelocity(vel);
	p->SetAngularVelocity(Vector3(0,0,0.01f));
	p->SetInverseInertia(InertialMatrixHelper::createSphereInvInertial(1.0f, radius));
	p->SetInverseMass(1.0f);
	p->SetCollisionVolume(new CollisionSphere(radius));
	GameEntity*g = new GameEntity(s, p);
	g->ConnectToSystems();
	return g;
}

/*
Makes a flat quad, initially oriented such that we can use it as a simple
floor. 
*/
GameEntity* MyGame::BuildQuadEntity(float size) 
{
	SceneNode* s = new SceneNode(quad);

	s->SetModelScale(Vector3(size,size,size));
	//Oh if only we had a set texture function...we could make our brick floor again WINK WINK
	s->SetBoundingRadius(size);

	PhysicsNode*p = new PhysicsNode(Quaternion::AxisAngleToQuaterion(Vector3(1,0,0), 90.0f), Vector3());
	p->SetUseGravity(false);
	p->SetInverseMass(0.0f);
	p->SetInverseInertia(InertialMatrixHelper::createImmovableInvInertial());
	p->SetCollisionVolume(new CollisionPlane(Vector3(0,1,0), 0));
	GameEntity*g = new GameEntity(s, p);
	g->ConnectToSystems();
	return g;
}

ObjectType* MyGame::BuildObjectEntity(int type, int subType) 
{
	SceneNode* s = new SceneNode(cube);
	PhysicsNode* p = new PhysicsNode();
	p->SetUseGravity(false);

	ObjectType*g = new ObjectType(s, p, type, subType);
	g->ConnectToSystems();
	SceneNode &test = g->GetRenderNode();

	test.SetModelScale(Vector3(TILE_WIDTH, TILE_HEIGHT, TILE_DEPTH));
	test.SetBoundingRadius(TILE_DEPTH);

	//50% chance that we set the random value of this plane to a random value from 1 - 5
	if ((rand() % 100 + 1) > 50)
	{
		if (getDrawingPlanes(subType) > 1)
		{
			g->setRandom(rand() % 5 + 1);
		}
	}

	elements[subType].push_back(g);

	/*if reference vector of this subtype has objects inside delete them all
	  so it contains only the one that was created last */
	while(reference[subType].size() > 0)
	{
		reference[subType].pop_back();
	}
	reference[subType].push_back(g);
	
	return g;
}

ObjectType* MyGame::BuildObjectEntityNetwork(int type, int subType, int setRandValue, int randPlane)
{
	SceneNode* s = new SceneNode(cube);
	PhysicsNode* p = new PhysicsNode();
	p->SetUseGravity(false);

	ObjectType*g = new ObjectType(s, p, type, subType);
	g->ConnectToSystems();
	SceneNode &test = g->GetRenderNode();

	test.SetModelScale(Vector3(TILE_WIDTH, TILE_HEIGHT, TILE_DEPTH));
	test.SetBoundingRadius(TILE_DEPTH);

	//50% chance that we set the random value of this plane to a random value from 1 - 5
	if (setRandValue > 50)
	{
		if (getDrawingPlanes(subType) > 1)
		{
			g->setRandom(randPlane);
		}
	}

	elements[subType].push_back(g);

	/*if reference vector of this subtype has objects inside delete them all
	so it contains only the one that was created last */
	while (reference[subType].size() > 0)
	{
		reference[subType].pop_back();
	}
	reference[subType].push_back(g);

	return g;
}

//creates a new Obstacle
Obstacle* MyGame::BuildObstacleEntity(float size, int type, int subType, ObjectType* _obj, int _obstacle_type) 
{
	SceneNode* s = new SceneNode(sphere);
	PhysicsNode* p = new PhysicsNode();
	p->SetUseGravity(false);

	Obstacle *g = new Obstacle(_obj, s, p, type, subType, _obstacle_type);

	g->ConnectToSystems();
	SceneNode &test = g->GetRenderNode();

	test.SetModelScale(Vector3(size, size, size));
	test.SetBoundingRadius(size);

	obstacleElements[subType].push_back(g);

	return g;
}

GameEntity* MyGame::BuildBulletEntity(float radius, Vector3 pos)
{
	SceneNode* test = new SceneNode(sphere);
	test->SetModelScale(Vector3(radius, radius, radius));
	test->SetBoundingRadius(radius);
	test->SetColour(Vector4(0.2, 0.2, 0.5, 1));
	PhysicsNode*p = new PhysicsNode();
	p->SetUseDamping(false);
	p->SetUseGravity(false);
	p->SetPosition(pos);
	p->SetCollisionVolume(new CollisionSphere(radius));    //new 4.2.2015  Daixi

	GameEntity*g = new GameEntity(test, p);
	g->ConnectToSystems();
	return g;
}

int MyGame::getIndexOfAllEtities(GameEntity* _G)
{
	int index = -1;
	for (int i = 0; i < allEntities.size(); i++)
	{
		if (_G == allEntities[i])
		{
			index = i;
			break;
		}
	}

	return index;
}

int MyGame::getIndexOfElements(ObjectType* _G)
{
	for (int j = 0; j < elements[_G->getSubType()].size(); j++)
	{
		if (_G == elements[_G->getSubType()][j])
		{
			return j;
		}
	}
	return 0;
}

/* We get the number of planes that are not generating gaps infront of them 
   which should always be at least one*/
int MyGame::getDrawingPlanes(int _subType)
{
	int count = 0;
	for (int i = 0; i <4; i++)
	{
		if (reference[i].size() > 0)
		{
			if (reference[i][0]->getRandom() == 1 && i != _subType)
			{
				count++;
			}
		}
	}
	return count;
}

int MyGame::getEmptyIndex(int _subType)
{
	for (int i = 0; i < elements[_subType].size(); i++)
	{
		if (elements[_subType][i]->getState() == 4)
		{
			return i;
		}
	}
	return -1;
}

void MyGame::handlePlanes(float msec)
{
	for (int i = 0; i < 4; i++)
	{
		// iterate through all obstacles
		for (int j = 0; j < obstacleElements[i].size(); j++)
		{
			//check if the Obstacel has reached at the end
			if (obstacleElements[i][j]->getState() == 1)
			{
				//set the new state so that it can be re used
				obstacleElements[i][j]->setState(2);

				//remove from all Entities so that we dont waste the update calls when not moving.
				allEntities.erase(allEntities.begin() + getIndexOfAllEtities(obstacleElements[i][j]));
			}
		}
		// iterate through all Tiles
		for (int j = 0; j < elements[i].size(); j++)
		{
			if (elements[i][j]->getState() == 1)
			{

				//create new
				if (getEmptyIndex(i) == -1)
				{
					allEntities.push_back(BuildObjectEntity(0, i));
				}
				else
				{
					int x = getEmptyIndex(i);

					elements[i][x]->reset();
					while (reference[i].size()>0)
						reference[i].pop_back();

					reference[i].push_back(elements[i][x]);
					if ((rand() % 100 + 1) > 50)
					{
						if (getDrawingPlanes(i) > 0)
						{
							elements[i][x]->setRandom(rand() % 5 + 1);
						}
					}
					allEntities.push_back(elements[i][x]);

					//create obstacle
					if ((rand() % 100 + 1) < 50)
						CreateObstacle(elements[i][x]);
				}
				//change state
				elements[i][j]->setState(2);

			}
			else if (elements[i][j]->getState() == 3)
			{
				elements[i][j]->setState(4);
				allEntities.erase(allEntities.begin() + getIndexOfAllEtities(elements[i][j]));
			}
		}
	}
}


/*
resetNumber = rand() % 100 + 1;
randomPlane = rand() % 5 + 1;
createObstacleNumber = rand() % 100 + 1;
objectRandValue = rand() % 100 + 1;
objectRandPlane = rand() % 5 + 1;
obstacleRandNum = rand() % 2 + 1;
*/
void MyGame::handlePlanesNetwork(int resetNumber, int randomPlane, int createObstacleNumber, int objectRandValue, int objectRandPlane, int obstacleRandNum)
{
	for (int i = 0; i < 4; i++)
	{
		// iterate through all obstacles
		for (int j = 0; j < obstacleElements[i].size(); j++)
		{
			//check if the Obstacel has reached at the end
			if (obstacleElements[i][j]->getState() == 1)
			{
				//set the new state so that it can be re used
				obstacleElements[i][j]->setState(2);

				//remove from all Entities so that we dont waste the update calls when not moving.
				allEntities.erase(allEntities.begin() + getIndexOfAllEtities(obstacleElements[i][j]));
			}
		}
		// iterate through all Tiles
		for (int j = 0; j < elements[i].size(); j++)
		{
			if (elements[i][j]->getState() == 1)
			{

				//create new
				if (getEmptyIndex(i) == -1)
				{
					allEntities.push_back(BuildObjectEntityNetwork(0, i,objectRandValue, objectRandPlane));
				}
				else
				{
					int x = getEmptyIndex(i);

					elements[i][x]->reset();
					while (reference[i].size()>0)
						reference[i].pop_back();

					reference[i].push_back(elements[i][x]);
					if (resetNumber > 50)
					{
						if (getDrawingPlanes(i) > 0)
						{
							elements[i][x]->setRandom(randomPlane);
						}
					}
					allEntities.push_back(elements[i][x]);

					//create obstacle
					if (createObstacleNumber < 50)
						CreateObstacleNetwork(elements[i][x],obstacleRandNum);
				}
				//change state
				elements[i][j]->setState(2);

			}
			else if (elements[i][j]->getState() == 3)
			{
				elements[i][j]->setState(4);
				allEntities.erase(allEntities.begin() + getIndexOfAllEtities(elements[i][j]));
			}
		}
	}
}

void MyGame::CreateObstacle(ObjectType* _obj)
{
	int random_number = rand() % 2 + 1;
	Obstacle* temp = NULL;
	//select random obstacle 

	int obstacleType = rand() % 5 + 0;
	int empty = getObstacleEmptyIndex(_obj->getSubType(),obstacleType);
	// for the first obstacle created
	if (obstacleReference[_obj->getSubType()] == NULL)
	{
		temp = BuildObstacleEntity(50, 1, _obj->getSubType(), _obj, obstacleType);
		if (temp->getObstacleType() == 1)
		{
			GameEntity* bul = BuildBulletEntity(20, temp->GetPhysicsNode().GetPosition());
			temp->SetBullet(bul);
			temp->SetPlayer(PhysicsSystem::GetPhysicsSystem().GetPlayer());
			allEntities.push_back(bul);

		}
		obstacleElements[_obj->getSubType()].push_back(temp);
	}
	//reference exists, but everything is running/working
	else if (empty == -1)
	{
		if (obstacleReference[_obj->getSubType()] != NULL)
		{
			if (obstacleReference[_obj->getSubType()]->GetPhysicsNode().GetPosition().z > -4750.0f)
			{
				temp = BuildObstacleEntity(50, 1, _obj->getSubType(), _obj, obstacleType);
				if (temp->getObstacleType() == 1)
				{
					GameEntity* bul = BuildBulletEntity(20, temp->GetPhysicsNode().GetPosition());
					temp->SetBullet(bul);
					temp->SetPlayer(PhysicsSystem::GetPhysicsSystem().GetPlayer());
					allEntities.push_back(bul);

				}
				obstacleElements[_obj->getSubType()].push_back(temp);
			}
		}
	}
	//free obstacle exists 
	else 
	{
		//use the old object
		if (obstacleReference[_obj->getSubType()] != NULL)
		{
			if (obstacleReference[_obj->getSubType()]->GetPhysicsNode().GetPosition().z > -4750.0f)
			{
				temp = obstacleElements[_obj->getSubType()][empty];
				temp->SetTile(_obj);
			}
		}
	}
	//initialize values and push to allEntities
	if (temp != NULL)
	{
		int temp_lane = (_obj->getSubType() + 1) * 2 - random_number;
		temp->SetLane(temp_lane);
		temp->resetObstacle();
		obstacleReference[_obj->getSubType()] = temp;
		allEntities.push_back(temp);
	}
}

void MyGame::CreateObstacleNetwork(ObjectType* _obj, int random_number)
{
	Obstacle* temp = NULL;
	//select random obstacle 

	int obstacleType = rand() % 5 + 0;
	int empty = getObstacleEmptyIndex(_obj->getSubType(), obstacleType);
	// for the first obstacle created
	if (obstacleReference[_obj->getSubType()] == NULL)
	{
		temp = BuildObstacleEntity(50, 1, _obj->getSubType(), _obj, obstacleType);
		if (temp->getObstacleType() == 1)
		{
			GameEntity* bul = BuildBulletEntity(20, temp->GetPhysicsNode().GetPosition());
			temp->SetBullet(bul);
			temp->SetPlayer(PhysicsSystem::GetPhysicsSystem().GetPlayer());
			allEntities.push_back(bul);

		}
		obstacleElements[_obj->getSubType()].push_back(temp);
	}
	//reference exists, but everything is running/working
	else if (empty == -1)
	{
		if (obstacleReference[_obj->getSubType()] != NULL)
		{
			if (obstacleReference[_obj->getSubType()]->GetPhysicsNode().GetPosition().z > -4750.0f)
			{
				temp = BuildObstacleEntity(50, 1, _obj->getSubType(), _obj, obstacleType);
				if (temp->getObstacleType() == 1)
				{
					GameEntity* bul = BuildBulletEntity(20, temp->GetPhysicsNode().GetPosition());
					temp->SetBullet(bul);
					temp->SetPlayer(PhysicsSystem::GetPhysicsSystem().GetPlayer());
					allEntities.push_back(bul);

				}
				obstacleElements[_obj->getSubType()].push_back(temp);
			}
		}
	}
	//free obstacle exists 
	else
	{
		//use the old object
		if (obstacleReference[_obj->getSubType()] != NULL)
		{
			if (obstacleReference[_obj->getSubType()]->GetPhysicsNode().GetPosition().z > -4750.0f)
			{
				temp = obstacleElements[_obj->getSubType()][empty];
				temp->SetTile(_obj);
			}
		}
	}
	//initialize values and push to allEntities
	if (temp != NULL)
	{
		int temp_lane = (_obj->getSubType() + 1) * 2 - random_number;
		temp->SetLane(temp_lane);
		temp->resetObstacle();
		obstacleReference[_obj->getSubType()] = temp;
		allEntities.push_back(temp);
	}
}



//returns the index of the Obstacle which is free , if not , returns -1
int MyGame::getObstacleEmptyIndex(int _subType, int _obstacleType)
{
	for (int i = 0; i < obstacleElements[_subType].size(); i++)
	{
		if (obstacleElements[_subType][i]!=NULL)
		if (obstacleElements[_subType][i]->getState() == 2 && _obstacleType == obstacleElements[_subType][i]->getObstacleType())
		{
			return i;
		}
	}
	return -1;
}

void MyGame::receiveFromClients()
{
	Packet packet;

	// go through all clients
	std::map<unsigned int, SOCKET>::iterator iter;

	for (iter = networkServer->sessions.begin(); iter != networkServer->sessions.end(); iter++)
	{
		int data_length = networkServer->receiveData(iter->first, server_network_data);

		if (data_length <= 0)
		{
			//no data recieved
			continue;
		}

		int i = 0;
		while (i < (unsigned int)data_length)
		{
			packet.deserialize(&(server_network_data[i]));
			i += sizeof(Packet);

			switch (packet.packet_type)
			{

			case INIT_CONNECTION:

				cout <<"server received init packet from client" << endl;
				sendServerActionPackets();

				break;

			case ACTION_EVENT:

				cout <<"server received action event packet from client" << endl;
				sendServerActionPackets();

				break;

			default:

				cout <<"error in packet types" << endl;

				break;
			}
		}
	}
}


void MyGame::sendToClients()
{
	Packet packet;
	// go through all clients
	std::map<unsigned int, SOCKET>::iterator iter;

	for (iter = networkServer->sessions.begin(); iter != networkServer->sessions.end(); iter++)
	{
		int data_length = networkServer->receiveData(iter->first, server_network_data);

		if (data_length <= 0)
		{
			//no data recieved
			continue;
		}

		int i = 0;
		while (i < (unsigned int)data_length)
		{
			packet.deserialize(&(server_network_data[i]));
			i += sizeof(Packet);

			switch (packet.packet_type)
			{

			case INIT_CONNECTION:

				cout << "Server received init packet from client." << endl;

				//sendServerActionPackets();

				break;

			case ACTION_EVENT:

				cout << "Server received action event packet from client." << endl;

				//sendServerActionPackets();

				break;

			default:

				cout << "Error in packet types." << endl;

				break;
			}
		}
	}
}


void MyGame::sendServerActionPackets()
{
	sendData dts;
	
	dts.createObstacleNumber = createObstacleNumber;
	dts.objectRandPlane = objectRandPlane;
	dts.objectRandValue = objectRandValue;
	dts.obstacleRandNum = obstacleRandNum;
	dts.randomPlane = randomPlane;
	dts.resetNumber = resetNumber;
	
	networkServer->sendToAll((char*)&dts,sizeof(sendData));

	////vector <messageInfo> message;
	//messageInfo message[200];
	//for (int i = 0; i < allEntities.size(); ++i)
	//{
	//	messageInfo mi;
	//	mi.objectID = allEntities[i]->objectID;
	//	mi.Position = allEntities[i]->GetPhysicsNode().GetPosition();
	//	mi.Orientation = allEntities[i]->GetPhysicsNode().GetOrientation();
	//	mi.objectType = allEntities[i]->GetRenderNode().nodeType;
	//	message[i] = mi;
	//	//message.push_back(mi);
	//}
	//Packet packet;
	//packet.packet_type = ACTION_EVENT;
	//for (int x = 0; x < 200; x++)
	//	packet.data[x] = message[x];
	//const unsigned int packet_size = sizeof(Packet);
	//char packet_data[packet_size];
	//packet.serialize(packet_data);
	//networkServer->sendToAll(packet_data, packet_size);
	//if (message.size() != 0)
	//{
		//networkServer->sendToAll((char*)&message[0], sizeof(messageInfo) * message.size());
	//}
}

void MyGame::sendClientActionPackets()
{
	// send action packet
	const unsigned int packet_size = sizeof(Packet);
	char packet_data[packet_size];

	Packet packet;
	packet.packet_type = ACTION_EVENT;
	packet.packet_integer = 0;
	packet.serialize(packet_data);

	NetworkServices::sendMessage(networkClient->ConnectSocket, packet_data, packet_size);
}

void MyGame::handleNetworking()
{
	if (IS_HOST)
	{
		// get new clients
		if (networkServer->acceptNewClient(client_id))
		{
			cout << "Client " << client_id << " has been connected to the server." << endl;
			client_id++;
		}

		receiveFromClients();
	}


	if (IS_CLIENT)
	{
		sendClientActionPackets();
		int data_length = networkClient->receivePackets(client_network_data);
		sendData dts;
		memcpy(&dts, client_network_data, data_length);
		handlePlanesNetwork(dts.resetNumber, dts.randomPlane, dts.createObstacleNumber, dts.objectRandValue, dts.objectRandPlane, dts.obstacleRandNum);
	}

	//	Packet packet;
	//	int data_length = networkClient->receivePackets(client_network_data);

	//	if (data_length <= 0)
	//	{
	//		return;
	//	}

	//	int i = 0;
	//	while (i < (unsigned int)data_length)
	//	{
	//		packet.deserialize(&(client_network_data[i]));
	//		i += sizeof(Packet);

	//		switch (packet.packet_type)
	//		{

	//		case ACTION_EVENT:

	//			cout << "Client received action event packet from server" << endl;
	//			sendClientActionPackets();
	//			for (int x = 0; x < 200; x++)
	//			{
	//				bool exists = false;
	//				if (packet.data[x].objectID > 0)
	//				{
	//					for (int y = 0; y < allEntities.size(); y++) // For each existing Node
	//					{
	//						if (packet.data[x].objectID == allEntities[y]->objectID) // If the Node in message corresponds to the Existing Node
	//						{
	//							allEntities[y]->GetPhysicsNode().SetPosition(packet.data[x].Position);
	//							allEntities[y]->GetPhysicsNode().SetOrientation(packet.data[x].Orientation);
	//							exists = true;
	//							break;
	//						}
	//					}
	//				}
	//			}

	//			break;

	//		default:

	//			cout << "error in packet types" << endl;

	//			break;
	//		}
	//	}
	//}



	//	vector<messageInfo> message; //Create a vector to hold our data
	//	message.resize(data_length / sizeof(messageInfo)); //Resize it to hold the correct number of messageInfo
	//	memcpy(&message[0], client_network_data, data_length);	//Copy the clientData bytes into the vector
	//	std::cout << " Message Recieved from Server." << std::endl;
	//	std::cout << " Recieved " << data_length << " Bytes." << std::endl;
	//	bool exists = false;
	//	for (int x = 0; x < message.size(); x++) // For each Node in the message
	//	{
	//		for (int y = 0; y < allEntities.size(); y++) // For each existing Node
	//		{
	//			if (message[x].objectID == allEntities[y]->objectID) // If the Node in message corresponds to the Existing Node
	//			{
	//				allEntities[y]->GetPhysicsNode().SetPosition(message[x].Position);
	//				allEntities[y]->GetPhysicsNode().SetOrientation(message[x].Orientation);
	//				exists = true;
	//				break;
	//			}
	//		}
	//		if (!exists) //If not create it
	//		{
	//			if (message[x].objectType == OBJ_PLANE)
	//			{
	//				allEntities.push_back(BuildObjectEntity(0, 1));
	//			}
	//			else if (message[x].objectType = OBJ_SPHERE)
	//			{
	//				allEntities.push_back(BuildBuffEntity(20, Vector3(0, 0, 0)));
	//			}
	//		}
	//	}
	//}
}